//
// opencv_ficheros.cpp
// 
// Asignatura SMII. 
// Versió 0 (Oct. 2018): M. Agustí.
//
// En este ejemplo se manejan los ficheros de imagen y vídeo de/desde OpenCV:
//  * Imagen estática:
//    ** lectura de los argumentos que recibe el programa en línea de órdenes
//    ** y escritura del resultado
//  * Vídeo: ???????????????????????????????????????????????????????????????????
//   ** escritura del proceso detallado (100 cuadros --> 0% .. 100 de "mezcla") de las dos imágenes de partida
//   ** y lectura del resultado, mostrando el número de fotograma o valor de "mezcla"
//
// $ g++ opencv_ficheros.cpp -o opencv_ficheros `pkg-config opencv --cflags --libs`
// $ make opencv_ficheros && opencv_ficheros WindowsLogo.jpg LinuxLogo.jpg
//
/**
 * Està basat en "LinearBlend.cpp" dels exemples d'OpenCV:
 * @brief Simple linear blender ( dst = alpha*src1 + beta*src2 )
 * @author OpenCV team

 // Adding a Trackbar to our applications!
 //  https://docs.opencv.org/3.4.3/da/d6a/tutorial_trackbar.html
 
 i modificat per traure les imatges des de la línia d'ordres

 Caldria considerar que poden ser de tamanys diferentes i reescalar-ne una.
 */
#include <stdio.h>
// Les capsaleres canvien en la V4, ho comprobe'm
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

#include "comun.h"

using namespace cv;
using std::cout;

/** Global Variables */
const int alpha_slider_max = 100;
int alpha_slider;
double alpha;

/** Matrices to store images */
Mat src1, src2, src3;
Mat zeros;

#define NOM_FINESTRA "Resta Variable"

//![on_trackbar]
/**
 * @function on_trackbar
 * @brief Callback for trackbar
 */
static void on_trackbar( int, void* )
{
  // Resta de la imagen sobre otra completamente negra y con un gamma blanco
  alpha = (double) alpha_slider/alpha_slider_max ;
  addWeighted( zeros, 1.0, src1, -alpha, 255.0, src2);
  imshow( NOM_FINESTRA, src2 );

  // Aplicación de la negativa de manera directa
  bitwise_not(src2, src3);
  imshow("Imagen negativa", src3);
}


/**
 * @function main
 * @brief Main function
 */
int main( int argc, char* argv[]) //void )
{
  int tecla, // Tecla leída
      salir; // Controla la condición de terminación de la aplicación
  char nomDst[1024];
  
  if (argc < 2)
  {
    printf("Faltan parametros: %s <Ruta_imagen>\n",
	    argv[0]);
    exit( 1 );
  }

  src1 = imread( argv[1] ); //"../data/LinuxLogo.jpg");
  zeros = Mat::zeros(src1.size(), src1.type());
  //![load]

  if( src1.empty() ) { cout << "Error loading source \n"; return 1; }
  
  /// Initialize values
  alpha_slider = 0;

  //![window]
  namedWindow( NOM_FINESTRA, WINDOW_AUTOSIZE); // Create Window
  //![window]

  //![create_trackbar]
  char TrackbarName[50];
  sprintf( TrackbarName, "Indice Resta");
  createTrackbar( TrackbarName, NOM_FINESTRA, &alpha_slider, alpha_slider_max, on_trackbar );
  //![create_trackbar]

  /// Show some stuff
  on_trackbar( alpha_slider, 0 );

  salir = FALSE;

  while ( !salir )
  { 
    tecla = waitKey(25) & 255;  // Espera una tecla los milisegundos que haga falta
  
    switch ( tecla )
    {
      case ESC:
      case 'q':
      case 'Q':    // Si 'ESC', q ó Q, ¡acabar!
        salir = TRUE;
        break;

      default: break;
    }
  }// Fi de  "while ( !salir )"

 return( 0 );
}

//Fi d'opencv_ficheros.cpp
