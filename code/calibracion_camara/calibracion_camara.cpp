#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

// Configuración del tablero: 9 columnas x 7 filas
// Puntos internos = (columnas-1) x (filas-1)
const Size PATTERN_SIZE = Size(8, 6); 
const float SQUARE_SIZE = 0.015f; // 15mm en metros

void calibrarAjedrezIP(string url, string outputFile) {
    VideoCapture cap(url);
    if (!cap.isOpened()) {
        cerr << "Error al abrir la cámara IP" << endl;
        return;
    }

    vector<vector<Point2f>> imagePoints; // Puntos 2D en la imagen
    vector<vector<Point3f>> objectPoints; // Puntos 3D reales
    
    // Generar coordenadas 3D del tablero (Z=0)
    vector<Point3f> obj;
    for (int i = 0; i < PATTERN_SIZE.height; i++)
        for (int j = 0; j < PATTERN_SIZE.width; j++)
            obj.push_back(Point3f(j * SQUARE_SIZE, i * SQUARE_SIZE, 0));

    Mat frame, gray;
    cout << "Controles:\n 'c' -> Capturar frame\n 'ESC' -> Finalizar y calibrar" << endl;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        vector<Point2f> corners;
        
        // Buscar esquinas
        bool found = findChessboardCorners(gray, PATTERN_SIZE, corners,
            CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);

        Mat display = frame.clone();
        if (found) {
            drawChessboardCorners(display, PATTERN_SIZE, corners, found);
        }

        imshow("Calibracion", display);
        char key = (char)waitKey(1);

        if (key == 27) break; // ESC para terminar
        if (key == 'c' && found) {
            // Refinar sub-píxeles para mayor precisión
            cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
                TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));

            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
            cout << "Frame capturado (" << imagePoints.size() << ")" << endl;
        }
    }

    if (imagePoints.size() < 10) {
        cerr << "Error: Se necesitan al menos 10 capturas." << endl;
        return;
    }

    // Proceso de calibración
    Mat cameraMatrix, distCoeffs;
    vector<Mat> rvecs, tvecs;
    cout << "Calibrando... por favor espere." << endl;
    
    double rms = calibrateCamera(objectPoints, imagePoints, gray.size(), 
                                 cameraMatrix, distCoeffs, rvecs, tvecs);

    // Guardar parámetros en formato XML/YML
    FileStorage fs(outputFile, FileStorage::WRITE);
    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs << "avg_reprojection_error" << rms;
    fs.release();

    cout << "Calibración completada con error RMS: " << rms << endl;
    cout << "Archivo guardado como: " << outputFile << endl;
}

int main() {
    string ip = "IP"; // Tu IP de control_mando.cpp
    calibrarAjedrezIP(ip, "calibracion_camara.yml");
    return 0;
}