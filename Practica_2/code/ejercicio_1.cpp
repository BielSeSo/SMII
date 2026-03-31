//
// opencv_mat.cpp (UTF-8)
// 
// Asignatura SMII. 
// Versió 1.0 (Feb. 2k21) M. Agsustí:: adaptacions a coexistir OpenCV 4 i OpencV 3.2
// Versió 0 (Oct. 2018): M. Agustí:: portar desde la versión OpenCV 2.4.9.1 a OpenCV 3.0
//
// En este ejemplo se describirá cómo crear una imagen, recorrerla y mostrarla en pantalla.
//
//

/*
https://docs.opencv.org/4.0.0-beta/db/dfa/tutorial_transition_guide.html
OpenCV 4.0.0-beta OpenCV Tutorials > Introduction to OpenCV
Transition guide 
Changes overview 
This document is intended to software developers who want to migrate their code to OpenCV 3.0.

 OpenCV 3.2
 C++ --> compilar en g++: g++ opencv_mat.c -o opencv_mat `pkg-config opencv --cflags --libs`
 OpenCV 4
 C++ --> compilar en g++: g++ opencv_mat.c -o opencv_mat `pkg-config opencv4 --cflags --libs`
*/
#include <stdio.h>
#include <opencv2/opencv.hpp>

#include "comun.h"

using namespace cv; 


#define fGRIS "¡Hola, OpenCV en gris!"
#define fRGB  "¡Hola, OpenCV en color!"
#define fRGB_H "¡Hola, OpenCV en color con ralla!"

#define L MAXPIXEL
#define AMPLE (int)L
#define ALT (int)L

// Declaraciones de prototipos locales
int  inicializarImgGris( Mat *imgOrg );
int  inicializarImgRGB( Mat *imgOrg );
int  inicializarImgRGBHorizontal( Mat *imgOrg );


//
// Programa principal
//
int main(int argc, char* argv[]) 
{
 Mat imgOrgGris, imgOrgRGB, imgOrgRGB_H;
 Scalar colorDst;
 Mat rgbaPlanes[3];

 printf("Versión de OpenCV: %s\n", CV_VERSION);
 //CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION, CV_VERSION


 printf("Creando una imagen y mostrándola en pantalla [Cualquier tecla - Salir].\n");
 //https://docs.opencv.org/3.4.3/d1/dfb/intro.html
 colorDst.val[0] = 128;
 imgOrgGris.create( ALT, AMPLE, CV_8UC1); //, Scalar::all(0));
 imgOrgGris = colorDst; // Mat::zeros(ALT, AMPLE, CV_8UC1), Mat::ones ó Mat::eye
 if (!imgOrgGris.data ) 
 {
   fprintf(stderr, "Problemas al crear la imagen en grises\n");
   return( 1 );
 }
 printf("Se ha creado una imagen de %dx%d, de %d plano/s.\n", 
         imgOrgGris.cols, imgOrgGris.rows, imgOrgGris.channels() );
 inicializarImgGris( &imgOrgGris );

 namedWindow( fGRIS, WINDOW_NORMAL); //AUTOSIZE CV_WINDOW_AUTOSIZE ); 
 imshow(fGRIS, imgOrgGris ); 
 moveWindow( fGRIS, 0, 0 );



 // RGB
 colorDst.val[R] = 255;
 colorDst.val[G] = 255;
 colorDst.val[B] = 255;
 imgOrgRGB.create(imgOrgGris.rows, imgOrgGris.cols, CV_8UC3);
 imgOrgRGB_H.create(imgOrgGris.rows, imgOrgGris.cols, CV_8UC3);

 imgOrgRGB = colorDst;//, Scalar::all(0) );
 imgOrgRGB_H = colorDst;
 //imgOrgRGB = Scalar(255,255,255) - Scalar(128, 64, 32); // cv::Mat::ones(int rows, int cols, int type )		
 
 // Mat gray(color.rows, color.cols, color.th());
 if (!imgOrgRGB.data ) //!imgOrgRGB)
 {
   fprintf(stderr, "Problemas al crear la imagen en color\n");
   return( 2 );
 }
 printf("Se ha creado una imagen de %dx%d, de %d planos.\n", 
         imgOrgRGB.cols, imgOrgRGB.rows, imgOrgRGB.channels() );
 inicializarImgRGB( &imgOrgRGB);
 namedWindow( fRGB, WINDOW_AUTOSIZE ); //cvNamedWindow( fRGB, CV_WINDOW_AUTOSIZE ); 
 imshow( fRGB, imgOrgRGB ); //cvShowImage( fRGB, imgOrgRGB ); 
 moveWindow( fRGB, AMPLE+60, 0 ); //cvMoveWindow( fRGB, AMPLE+30, 0 );

 inicializarImgRGBHorizontal(&imgOrgRGB_H);
 namedWindow( fRGB_H, WINDOW_AUTOSIZE ); //cvNamedWindow( fRGB, CV_WINDOW_AUTOSIZE ); 
 imshow( fRGB_H, imgOrgRGB_H ); //cvShowImage( fRGB, imgOrgRGB ); 
 moveWindow( fRGB_H, AMPLE+60, 0 ); //cvMoveWindow( fRGB, AMPLE+30, 0 );


 // Split the Mat
 split(imgOrgRGB, rgbaPlanes);
 printf("Split: ha creado un vector de 3 imágenes de %dx%d, de %d planos.\n", 
         rgbaPlanes[R].cols, rgbaPlanes[R].rows, rgbaPlanes[R].channels() );
 imshow( "R", rgbaPlanes[R] );
 moveWindow( "R", AMPLE+60, ALT+30 ); 
 imshow( "G", rgbaPlanes[G] );
 moveWindow( "G", 2*(AMPLE+60), ALT+30 ); 
 imshow( "B", rgbaPlanes[B] );
 moveWindow( "B", 3*(AMPLE+60), ALT+30 ); 


 waitKey(0); // Espera una tecla los milisegundos que haga falta


 // Se puede cerrar una o todas las ventanas a voluntad
 destroyAllWindows( );

 // Se puede liberar memoria explícitamente o que lo haga el "recolector"
 imgOrgGris.release(); //cvReleaseImage( &imgOrgGris );
 imgOrgRGB.release(); //cvReleaseImage( &imgOrgRGB );
 imgOrgRGB_H.release();

 return 0;
}








//
// Inicializa una imagen de un sólo plano de color (imagen de grises)
// con un gradiente de valores de gris
//
// Entrada: imgOrg - la imagen de partida.
//
// Salida: imgOrg - la imagen resultados que se devuelve modificando la original.
//
int  inicializarImgGris( Mat *imgOrg ) 
{
  int x, // indice de las columnas
      y; // indice de las filas

  if ( imgOrg->channels() != 1)
    return( 1 );
  else
  {

    for ( y = 0; y < imgOrg->rows; y++ )
      for ( x = 0; x < imgOrg->cols; x++ )
   {
    imgOrg->at<uchar>(y, x) = x;
   } // Fin de "   for ( y = 0; y < imgOrg->height; y++ )"
  }// Fin de if-else ( imgOrg.channels() != 1)

 return( 0 );
} // Fin de "int  inicializarImgGris( Mat *imgOrg )"

/*
http://opencvexamples.blogspot.com/2013/10/assessing-pixel-values-of-image.html
Assessing the pixel values of an image 
*/




int  inicializarImgRGB( Mat *imgOrg ) 
{
  int x, // indice de las columnas
      y; // indice de las filas
  
    Scalar colorDst;

  if ( imgOrg->channels() != 3) // -> #nChanels
    return( 1 );
  else
  {

    for ( y = 0; y < imgOrg->rows; y++ )
      for ( x = 0; x < imgOrg->cols; x++ )
   {
     imgOrg->at<Vec3b>(y, x)[R] = y;
     imgOrg->at<Vec3b>(y, x)[G] = x;
     imgOrg->at<Vec3b>(y, x)[B] = (uchar)((y+x) % (int)L);
     // Tb.
     // imgOrg->at<Vec3b>(y, x) = Vec3b( y, x, (uchar)((y+x) % (int)L) );
     //
   } // Fin de "   for ( y = 0; y < imgOrg->height; y++ )"
  }// Fin de if-else ( imgOrg->nChannels != 3)
  
 return( 0 );
} // Fin de "int  inicializarImgRGB( Mat *imgOrg )"

int  inicializarImgRGBHorizontal( Mat *imgOrg ) 
{
    int x, // indice de las columnas
        y; // indice de las filas

    int half_x = imgOrg->rows/2,
        half_y = imgOrg->cols/2;

    Scalar colorDst;

  if ( imgOrg->channels() != 3) // -> #nChanels
    return( 1 );
  else
  {

    for ( y = 0; y < imgOrg->rows; y++ )
      for ( x = 0; x < imgOrg->cols; x++ )
   {
     imgOrg->at<Vec3b>(y, x)[R] = y;
     imgOrg->at<Vec3b>(y, x)[G] = x;
     imgOrg->at<Vec3b>(y, x)[B] = (uchar)((y+x) % (int)L);
   } 
 
    for (y = half_x-10; y < half_x+10; y++)
      for (x = 0; x < imgOrg->cols; x++)
   {
     imgOrg->at<Vec3b>(y, x)[R] = 255;
     imgOrg->at<Vec3b>(y, x)[G] = 255;
     imgOrg->at<Vec3b>(y, x)[B] = 255;
   }
  }
  
 return( 0 );
} // Fin de "int  inicializarImgRGB( Mat *imgOrg )"

//
// Fi del fitxer opencv_mat.cpp
