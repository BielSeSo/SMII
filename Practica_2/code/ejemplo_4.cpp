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
#if CV_VERSION_MAJOR == 4
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#else
#include <opencv2/opencv.hpp>
#endif
#include <iostream>

#include "comun.h"

using namespace cv;
using std::cout;

/** Global Variables */
const int alpha_slider_max = 100;
int alpha_slider;
double alpha;
double beta;

/** Matrices to store images */
Mat dst1,
    dst2,
    dst;

#define NOM_FINESTRA "Fusió: Linear Blend"

//![on_trackbar]
/**
 * @function on_trackbar
 * @brief Callback for trackbar
 */
static void on_trackbar( int, void* )
{
   alpha = (double) alpha_slider/alpha_slider_max ;
   beta = ( 1.0 - alpha );
   addWeighted( dst1, alpha, dst2, beta, 0.0, dst);
   imshow( NOM_FINESTRA, dst );
}
//![on_trackbar]


// Obtiene las versiones dst1 <-- src1 i dst2 <-- src2.
// Si es necesario las redimensiona a la menor de las dos
void comprobar_si_redimensionar( Mat src1, Mat src2 );


/**
 * @function main
 * @brief Main function
 */
int main( int argc, char* argv[]) //void )
{
  Mat src1, src2;
  int tecla, // Tecla leída
      salir; // Controla la condición de terminación de la aplicación
  char nomDst[1024];
  
  if (argc < 3)
  {
    printf("Faltan paràmetros: %s ruta_imagen1 ruta_imagen2\n",
	    argv[0]);
    exit( 1 );
  }

  /*
https://docs.opencv.org/3.4/d3/dff/samples_2cpp_2lsd_lines_8cpp-example.html#_a3
// Analitzar la línia d'ordres
    cv::CommandLineParser parser(argc, argv,
                                 "{input   i|../data/building.jpg|input image}"
                                 "{refine  r|false|if true use LSD_REFINE_STD method, if false use LSD_REFINE_NONE method}"
                                 "{canny   c|false|use Canny edge detector}"
                                 "{overlay o|false|show result on input image}"
                                 "{help    h|false|show help message}");
    if (parser.get<bool>("help"))
    {
        parser.printMessage();
        return 0;
    }
    parser.printMessage();
    String filename = parser.get<String>("input");
    bool useRefine = parser.get<bool>("refine");
    bool useCanny = parser.get<bool>("canny");
    bool overlay = parser.get<bool>("overlay");
    Mat image = imread(filename, IMREAD_GRAYSCALE);
    *****************************************************
*/

  printf("Comparant %s vs %s\n", argv[1], argv[2] );
   //![load]
   /// Read images ( both have to be of the same size and type )
  src1 = imread( argv[1] ); //"../data/LinuxLogo.jpg");
  src2 = imread( argv[2] ); //"../data/WindowsLogo.jpg");
   //![load]
  
   if( src1.empty() ) { cout << "Error loading src1 \n"; return 1; }
   if( src2.empty() ) { cout << "Error loading src2 \n"; return 2; }

   //Mostrar-les en pantalla
     /*
CV_LOAD_IMAGE_ANYDEPTH - If set, return 16-bit/32-bit image when the input has the corresponding depth, otherwise convert it to 8-bit.
CV_LOAD_IMAGE_COLOR - If set, always convert image to the color one
CV_LOAD_IMAGE_GRAYSCALE - If set, always convert image to the grayscale one

>0 Return a 3-channel color image
    */
   namedWindow( "IMG1", WINDOW_AUTOSIZE );
   imshow( "IMG1", src1 );
   namedWindow( "IMG2", WINDOW_AUTOSIZE );
   imshow( "IMG2", src2 );
  
   printf("#Canals? %d vs %d. Tamany? (%d,%d) vs (%d,%d)\n",
	  src1.channels(), src2.channels(),
 	  src1.rows, src1.cols, src2.rows, src2.cols );
   
   // Si són de diferents tamanys, però les dos Gris o les dos RGB --> redimensionar
   comprobar_si_redimensionar( src1, src2 );
   
   /// Initialize values
   alpha_slider = 0;

   //![window]
   namedWindow( NOM_FINESTRA, WINDOW_AUTOSIZE); // Create Window
   //![window]

   //![create_trackbar]
   char TrackbarName[50];
   sprintf( TrackbarName, "Alpha x %d", alpha_slider_max );
   createTrackbar( TrackbarName, NOM_FINESTRA, &alpha_slider, alpha_slider_max, on_trackbar );
   //![create_trackbar]

   /// Show some stuff
   on_trackbar( alpha_slider, 0 );

   /// Wait until user press some key
   //waitKey(0);

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
     
  case 'F':
  case 'f':
    // Guadar el fotograma actual en JPG
    sprintf( nomDst, "bin/ejemplo_4_img/%s_%03d.jpg", "fusio", alpha_slider );
    /*
    filename – Name of the file.
    image – Image to be saved.
    params –

    Format-specific save parameters encoded as pairs paramId_1, paramValue_1, paramId_2, paramValue_2, ... . The following parameters are currently supported:
        For JPEG, it can be a quality ( CV_IMWRITE_JPEG_QUALITY ) from 0 to 100 (the higher is the better). Default value is 95.
        For PNG, it can be the compression level ( CV_IMWRITE_PNG_COMPRESSION ) from 0 to 9. A higher value means a smaller size and longer compression time. Default value is 3.
        For PPM, PGM, or PBM, it can be a binary format flag ( CV_IMWRITE_PXM_BINARY ), 0 or 1. Default value is 1.
*/
    if ( imwrite( nomDst, dst ) ) {
      printf("Guarda't com a %s.\n", nomDst );
    }
    else {
      printf("Error al guarda't com a %s.\n", nomDst );
    }
   break;

   /*
CV_LOAD_IMAGE_ANYDEPTH - If set, return 16-bit/32-bit image when the input has the corresponding depth, otherwise convert it to 8-bit.
CV_LOAD_IMAGE_COLOR - If set, always convert image to the color one
CV_LOAD_IMAGE_GRAYSCALE - If set, always convert image to the grayscale one

>0 Return a 3-channel color image
    */
  case 'C':
  case 'c':
   src1 = imread( argv[1], IMREAD_COLOR ); 
   src2 = imread( argv[2], IMREAD_COLOR ); 
   comprobar_si_redimensionar( src1, src2 );
   break;

  case 'G':
  case 'g':
   src1 = imread( argv[1], IMREAD_GRAYSCALE ); 
   src2 = imread( argv[2], IMREAD_GRAYSCALE ); 
   comprobar_si_redimensionar( src1, src2 );

   /*   
  case 'V':
  case 'v':
    // Generar un fitxer de vídeo en el proceso detallat:
    // (100 cuadros --> 0% .. 100 de "mezcla") de las dos imágenes de partida
   break;
   
  case 'P':
  case 'p':
    // Play "especial": lectura del vídeo de resultado, mostrando el número de fotograma o valor de "mezcla"
   break;
   */
   
  default: ;    
  } // Fi de "switch ( tecla )"  
}// Fi de  "while ( !salir )"

 return( 0 );
}


void comprobar_si_redimensionar( Mat src1, Mat src2 ) {
   if ( (src1.channels() != src2.channels()) ) {
     printf("Las imágenes han de ser del mismo número de canales:  src1(%d) vs src2(%d)\n",
	    src1.channels(), src2.channels() );
     exit( 3 );
   }
   else {
     // explicitly specify dsize=dst.size(); fx and fy will be computed from that.
     if ( (src1.rows != src2.rows) OR (src1.cols != src2.cols)) {
      if (src1.rows*src1.cols < src2.rows*src2.cols) {
       printf("Redimensionant a %dx%d\n", src1.cols,src1.rows );
       resize(src2,dst2, Size(src1.cols,src1.rows), 0, 0,INTER_NEAREST  );
       src1.copyTo(dst1);
       dst =  Mat::zeros( src1.rows, src1.cols, src1.depth() );
      }
      else {
       printf("Redimensionant a %dx%d\n", src2.cols,src2.rows );	
       resize(src1,dst1, Size(src2.cols,src2.rows), 0, 0, INTER_NEAREST );
       src2.copyTo(dst2);
       dst =  Mat::zeros( src2.rows, src2.cols, src2.depth() );
      }
     }
     else {
       src1.copyTo(dst1);
       src2.copyTo(dst2);
       dst =  Mat::zeros( src2.rows, src2.cols, src2.depth() );
     }
   }
} // Fi de "comprobar_si_redimensionar( ..."

//Fi d'opencv_ficheros.cpp
