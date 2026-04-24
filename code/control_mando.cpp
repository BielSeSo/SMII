#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

#include "game/player.h"

using namespace std;
using namespace cv;

bool vistoID3, vistoID4;


void girarDireccion(int angulo)
{
    player1.grados += 5.0f;
}

void acelerar()
{

}

void frenar()
{

}

void procesarControlMando(string url, string calibFile) 
{
    //La URL es en caso de usar cámara por IP
    //calibFile es de tipo YML

    VideoCapture cap(url);
    if (!cap.isOpened()) return;

    Mat camMatrix, distCoeffs;
    FileStorage fs(calibFile, FileStorage::READ);
    if (!fs.isOpened()) return;
    fs["camera_matrix"] >> camMatrix;
    fs["distortion_coefficients"] >> distCoeffs;

    Ptr<aruco::Dictionary> dict = aruco::getPredefinedDictionary(aruco::DICT_ARUCO_ORIGINAL);
    Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();

    float markerLength = 0.05; 
    Mat frame;
    static int ultimoAnguloFijo = -999;

    while (cap.read(frame)) {
        vector<int> ids;
        vector<vector<Point2f>> corners;
        vector<Vec3d> rvecs, tvecs;

        aruco::detectMarkers(frame, dict, corners, ids, params);

        if (ids.size() > 0) {
            aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs, tvecs);

            double sumaAngulosReales = 0;
            int contadorVolante = 0;

            for (size_t i = 0; i < ids.size(); i++) {
                if (ids[i] >= 0 && ids[i] <= 2) {
                    
                    double anguloGradosReal = rvecs[i][1] * (180.0 / CV_PI);
                    
                    sumaAngulosReales += anguloGradosReal;
                    contadorVolante++;

                    drawFrameAxes(frame, camMatrix, distCoeffs, rvecs[i], tvecs[i], markerLength * 0.8f);
                }

                if (ids[i] == 3) vistoID3 = true;
                if (ids[i] == 4) vistoID4 = true;
            }

            // ——— Lógica para control de aceleración/freno ———

            if (vistoID4 && !vistoID3) {
                cout << "ACCION: ACELERANDO" << endl;
                acelerar();
            } 
            // Mientras se vea la 3 y la 4 NO: Frena
            else if (vistoID3 && !vistoID4) {
                cout << "ACCION: FRENANDO" << endl;
                frenar();
            }
            // Si se ven ambas o ninguna: No hace nada (punto muerto)
            else {
                // Opcional: podrías llamar a una función soltarPedales();
            }

            // ——— Lógica para control del volante ———

            if (contadorVolante > 0) {
                double promedioReal = sumaAngulosReales / contadorVolante;

                if (promedioReal > 90) promedioReal = 90;
                if (promedioReal < -90) promedioReal = -90;

                int anguloDiscreto = (int)(round(promedioReal / 5.0) * 5);

                if (anguloDiscreto != ultimoAnguloFijo) {
                    cout << "ANGULO REAL: " << promedioReal << " -> MOSTRADO: " << anguloDiscreto << endl;
                    ultimoAnguloFijo = anguloDiscreto;

                    girarDireccion(anguloDiscreto);
                }
            }
        }
        // imshow("Volante", frame);
        // if (waitKey(1) == 'q') break;
    }
}

/*
int main() {
    procesarControlMando("IP", "calibracion_camara.yml");
    return 0;
}
*/