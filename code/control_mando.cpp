#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

void girarDireccion(int angulo){
}

void acelerar(){
}

void frenar(){
}

void procesarControlMando(std::string url, std::string calibFile) {
    //La URL es en caso de usar cámara por IP
    //calibFile es de tipo YML

    cv::VideoCapture cap(url);
    if (!cap.isOpened()) return;

    cv::Mat camMatrix, distCoeffs;
    cv::FileStorage fs(calibFile, cv::FileStorage::READ);
    if (!fs.isOpened()) return;
    fs["camera_matrix"] >> camMatrix;
    fs["distortion_coefficients"] >> distCoeffs;

    cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL);
    cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();

    float markerLength = 0.05; 
    cv::Mat frame;
    static int ultimoAnguloFijo = -999;

    while (cap.read(frame)) {
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        std::vector<cv::Vec3d> rvecs, tvecs;

        cv::aruco::detectMarkers(frame, dict, corners, ids, params);

        if (ids.size() > 0) {
            cv::aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs, tvecs);

            double sumaAngulosReales = 0;
            int contadorVolante = 0;

            for (size_t i = 0; i < ids.size(); i++) {
                if (ids[i] >= 0 && ids[i] <= 2) {
                    
                    double anguloGradosReal = rvecs[i][1] * (180.0 / CV_PI);
                    
                    sumaAngulosReales += anguloGradosReal;
                    contadorVolante++;

                    cv::drawFrameAxes(frame, camMatrix, distCoeffs, rvecs[i], tvecs[i], markerLength * 0.8f);
                }

                if (ids[i] == 3) vistoID3 = true;
                if (ids[i] == 4) vistoID4 = true;
            }

            // ——— Lógica para control de aceleración/freno ———

            if (vistoID4 && !vistoID3) {
                std::cout << "ACCION: ACELERANDO" << std::endl;
                acelerar();
            } 
            // Mientras se vea la 3 y la 4 NO: Frena
            else if (vistoID3 && !vistoID4) {
                std::cout << "ACCION: FRENANDO" << std::endl;
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
                    cv::cout << "ANGULO REAL: " << promedioReal << " -> MOSTRADO: " << anguloDiscreto << cv::endl;
                    ultimoAnguloFijo = anguloDiscreto;

                    girarDireccion(anguloDiscreto);
                }
            }
        }
        // cv::imshow("Volante", frame);
        // if (cv::waitKey(1) == 'q') break;
    }
}

/*
int main() {
    procesarControlMando("IP", "calibracion_camara.yml");
    return 0;
}
*/