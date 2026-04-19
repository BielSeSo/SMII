#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;
using namespace cv;

void procesarControlMando(string url, string calibFile) {
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
                    // PRUEBA ESTO: Si rvecs[i][2] solo llega a 15, cambia el [2] por [0] o [1]
                    // El valor rvecs[i][2] viene en RADIANES. 
                    // 90 grados DEBERÍAN ser 1.57 radianes.
                    double anguloGradosReal = rvecs[i][1] * (180.0 / CV_PI);
                    
                    // Si el valor es muy bajo, es posible que la marca esté girada 90º físicamente
                    // Intentamos capturar el giro más fuerte:
                    sumaAngulosReales += anguloGradosReal;
                    contadorVolante++;

                    drawFrameAxes(frame, camMatrix, distCoeffs, rvecs[i], tvecs[i], markerLength * 0.8f);
                }
            }

            if (contadorVolante > 0) {
                double promedioReal = sumaAngulosReales / contadorVolante;

                // LÓGICA CORRECTA DE DISCRETIZACIÓN (Pasos de 5 en 5)
                // Primero limitamos el rango real de la cámara
                if (promedioReal > 90) promedioReal = 90;
                if (promedioReal < -90) promedioReal = -90;

                // Ahora redondeamos al múltiplo de 5 más cercano
                // Ejemplo: si detecta 37.4 grados -> 37.4 / 5 = 7.48 -> round(7.48) = 7 -> 7 * 5 = 35 grados.
                int anguloDiscreto = (int)(round(promedioReal / 5.0) * 5);

                if (anguloDiscreto != ultimoAnguloFijo) {
                    cout << "ANGULO REAL: " << promedioReal << " -> MOSTRADO: " << anguloDiscreto << endl;
                    ultimoAnguloFijo = anguloDiscreto;
                }
            }
        }
        imshow("Volante", frame);
        if (waitKey(1) == 'q') break;
    }
}

int main() {
    procesarControlMando("IP", "calibracion_camara.yml");
    return 0;
}