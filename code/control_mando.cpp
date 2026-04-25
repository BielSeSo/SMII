#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <atomic>

#include "game/player.h"
#include "control_mando.h"
#include "sound_maker.h"

using namespace std;
using namespace cv;

bool vistoID3 = false, vistoID4 = false;

void girarDireccion(int angulo)
{
    player1.grados += angulo;
}

void acelerar()
{
    player1.velocidad += 0.005f;
}

void frenar()
{
    player1.velocidad -= 0.005f;
    playGameSound(2);
}

void procesarControlMando(string url, atomic<bool> &evento) 
{
    VideoCapture cap(url);
    if (!cap.isOpened()) return;

    Mat camMatrix, distCoeffs;
    FileStorage fs(calibFile, FileStorage::READ);
    if (!fs.isOpened()) return;
    fs["camera_matrix"] >> camMatrix;
    fs["distortion_coefficients"] >> distCoeffs;

    Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
    params->adaptiveThreshWinSizeMin = 5;
    params->adaptiveThreshWinSizeMax = 21;
    Ptr<aruco::Dictionary> dict = aruco::getPredefinedDictionary(aruco::DICT_ARUCO_ORIGINAL);

    // --- VARIABLES DE FILTRADO ---
    float factorSuavizado = 0.15f; 
    int framesVisto = 0;
    const int UMBRAL_CONFIANZA = 3; // Mínimo 3 fotos viendo el ArUco para actuar
    float markerLength = 0.05; 
    Mat frame;
    static int ultimoAnguloFijo = -999;

    while (cap.read(frame) && !evento) {

        vistoID3 = false;
        vistoID4 = false;

        vector<int> ids;
        vector<vector<Point2f>> corners;
        vector<Vec3d> rvecs, tvecs;

        aruco::detectMarkers(frame, dict, corners, ids, params);

        if (ids.size() > 0) {
            // Incrementamos la confianza porque hay marcas en pantalla
            framesVisto++;

            // FILTRO DE CONFIANZA: Solo entramos si llevamos varios frames viendo algo
            if (framesVisto >= UMBRAL_CONFIANZA)
            {
                aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs, tvecs);

                double sumaAngulosReales = 0;
                int contadorVolante = 0;

                for (size_t i = 0; i < ids.size(); i++) {
                    
                    // FILTRO DE ÁREA: Ignora detecciones minúsculas (reflejos/fantasmas)
                    double area = cv::contourArea(corners[i]);
                    if (area < 1000) continue; 

                    // Lógica Volante (IDs 0, 1, 2)
                    if (ids[i] >= 0 && ids[i] <= 2) {
                        double anguloGradosReal = rvecs[i][1] * (180.0 / CV_PI);
                        sumaAngulosReales += anguloGradosReal;
                        contadorVolante++;
                    }

                    // Lógica Pedales
                    if (ids[i] == 3) vistoID3 = true;
                    if (ids[i] == 4) vistoID4 = true;
                }

                // --- ACCIÓN ACELERAR / FRENAR ---
                if (vistoID4 && !vistoID3) acelerar();
                else if (vistoID3 && !vistoID4) frenar();

                // --- ACCIÓN GIRO CON SUAVIZADO ---
                if (contadorVolante > 0) {
                    double promedioReal = sumaAngulosReales / contadorVolante;
                    
                    // Limitamos el ángulo
                    if (promedioReal > 90) promedioReal = 90;
                    if (promedioReal < -90) promedioReal = -90;

                    // FILTRO DE SUAVIZADO (Interpolación lineal)
                    // En lugar de saltar al ángulo nuevo, nos acercamos un 15% (factorSuavizado)
                    player1.grados = (player1.grados * (1.0f - factorSuavizado)) + (promedioReal * factorSuavizado);
                    
                    // Solo imprimimos si el cambio es notable (para no saturar la consola)
                    if (abs(promedioReal - ultimoAnguloFijo) > 2) {
                        cout << "GIRO SUAVE: " << player1.grados << endl;
                        ultimoAnguloFijo = promedioReal;
                    }
                }
            }
        } else {
            // Si no se ve nada, la confianza vuelve a cero inmediatamente
            framesVisto = 0;
            // Opcional: Enderezar el volante poco a poco si se pierde la marca
            player1.grados *= 0.95f; 
        }

        waitKey(1);
        // imshow("Volante", frame); // Descomenta para ver la cámara y los ejes
    }
}

/*
int main() {
    procesarControlMando("IP", "calibracion_camara.yml");
    return 0;
}
*/