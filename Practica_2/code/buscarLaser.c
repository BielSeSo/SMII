#include <opencv2/opencv.hpp>
#include <iostream>

#include "buscarLaser.h"

using namespace cv;
using namespace std;

void  pintaCruz(Mat *image, int columna, int fila, int ancho)
{
  int thickness = 5, 
      line_type = LINE_AA,
      shift = 0;

    line( *image, Point(columna-ancho,fila), Point(columna+ancho, fila), 
        Scalar(0,255,0), 3, line_type ); //thickness, line_type
    line( *image, Point(columna, fila-ancho), Point(columna, fila+ancho), 
        Scalar(0,255,0), 5, line_type); //thickness, line_type 
}

void Op_buscar_laser(Mat* origen, Mat* destino)
{
    // Calculamos los momentos de la imagen binaria
    Moments m = moments(*origen, true);

    if (m.m00 != 0.0)
    {
        int cx = static_cast<int>(m.m10 / m.m00);
        int cy = static_cast<int>(m.m01 / m.m00);

        // Dibujar la cruz
        pintaCruz(destino, cx, cy, 30);
    }
    else
    {
        std::cout << "No hay centro localizado (m00 = 0)" << std::endl;
    }
    
}