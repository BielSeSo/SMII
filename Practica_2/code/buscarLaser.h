#ifndef LASER_H
#define LASER_H

#include <opencv2/opencv.hpp>

using namespace cv;

void  pintaCruz(Mat *image, int columna, int fila, int ancho);
void Op_buscar_laser(Mat* origen, Mat* destino);

#endif