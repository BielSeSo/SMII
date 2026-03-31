//
// opencv_events.cpp
// 
// Asignatura SMII. 
// Versió 1.0 (Feb. 2k21) M. Agsustí:: adaptacions a coexistir OpenCV 4 i OpencV 3.2
// Versió 0 (Oct. 2018): M. Agustí.
//
// En este ejemplo se manejan los eventos de usuario:
//  * Ratón: pinta sobre imagen en color (RGB) con el botón principal del ratón y se muestra el color del píxel bajo la posición del ratón con el botón secundario. También puede mover los controles para variar el color a aplicar.
//  * Teclado: Se puede cambiar el color para pintar con las teclas R, G, B (incrementan el valor de la componente) o r, g, b (decrementan cada componentes. Y terminar la aplicación con ESC o 'q'.
//
// $ g++ opencv_events.cpp -o opencv_events `pkg-config opencv --cflags --libs`
// $ make opencv_events && opencv_events 
//

#include <stdio.h>
// C++ o C?
#ifdef __cplusplus
// Les capsaleres canvien en la V4, ho comprobe'm
#if CV_MAJOR_VERSION == 4
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#else
// Versió 2 i 3 : #elif CV_MAJOR_VERSION == 3 ...
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#endif
#else
//__STDC__
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#endif

#include "comun.h"
using namespace cv;  // The new C++ interface API is inside this namespace. Import it.
//using namespace std;

#define fORG "Imagen original"
#define fDST  "Imagen umbralizada"

#define AMPLE 255
#define ALT 255

int valor[3] = {128, 128, 128};
int colorAnterior = 0;
BOOL estaPintado = FALSO;



// Declaraciones de prototipos locales
void cambioDeValor( int pos, void *userdata );
//int procesarImg( IplImage *imgOrg, IplImage *imgDst, int valor );
int procesarImg( Mat imgOrg, Mat imgDst, int valor );
void my_mouse_callback(int event, int x, int y, int flags, void* param  );


//
// Programa principal
//
int main(int argc, char* argv[]) 
{
  Mat imgOrg; //, imgDst; 

 int tecla, // Tecla leída
     salir; // Controla la condición de terminación de la aplicación
 
 
 printf("OpenCV %s == %d.%d.%d.%d.\n",
	CV_VERSION,
	CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION, CV_VERSION_REVISION );
 
 printf("Entrada de datos con OpenCV + control de desplaz. + posición del ratón\n\
 Inicializa la imagen al valor de la barra de desplz.\n\
 Dice qué color hay en la posición de la imagen donde se hace click con el botón secundario\n\
 Pinta en blanco sobre la imagen mientras se mantenga el boton principal pulsado\n\
[S/s - guarda la imatge. ESC, q ,Q - Salir].\n");

 if (argc > 1)
 {
  valor[R] = valor[G] = valor[B] = atoi( argv[1] );
 }

 imgOrg = Mat::zeros(ALT, AMPLE, CV_8UC3);
 if ( imgOrg.empty() ) //!imgOrg.data )
 {
   fprintf(stderr, "Problemas al crear la imagen de partida.\n");
   return( 1 );
 }
 printf("Se ha creado una imagen de %dx%d, de %d planos.\n", 
        imgOrg.rows, imgOrg.cols, imgOrg.channels() );
 namedWindow( fORG, WINDOW_NORMAL); //_AUTOSIZE ); 



 imshow( fORG, imgOrg ); 
 moveWindow( fORG, 0, 0 );

 createTrackbar( "R", fORG, &valor[R], MAXPIXEL, cambioDeValor, (void *)(long int)R );
 createTrackbar( "G", fORG, &valor[G], MAXPIXEL, cambioDeValor, (void *)(long int)G );
 createTrackbar( "B", fORG, &valor[B], MAXPIXEL, cambioDeValor, (void *)(long int)B );
 cambioDeValor( valor[R], (void *)(long int)R );
 cambioDeValor( valor[G], (void *)(long int)G );
 cambioDeValor( valor[B], (void *)(long int)B );
 
 setMouseCallback( fORG, my_mouse_callback, (void*)&imgOrg  );

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
     
  case 'r':
    if ( valor[R] > 0 )
    {
      cambioDeValor( --valor[R], (void *)(long int)R );
      setTrackbarPos("R", fORG, valor[R]);
    }
   break;
  case 'R':
    if ( valor[R] < 255 )
    {
      cambioDeValor( ++valor[R], (void *)(long int)R );
      setTrackbarPos("R", fORG, valor[R]);
    }
   break;
   
  case 'g':
    if ( valor[G] > 0 )
    {
      cambioDeValor( --valor[G], (void *)(long int)G );
      setTrackbarPos("G", fORG, valor[G]);
    }
   break;
  case 'G':
    if ( valor[G] < 255 )
    {
      cambioDeValor( ++valor[G], (void *)(long int)G );
      setTrackbarPos("G", fORG, valor[G]);
    }
   break;

  case 'b':
    if ( valor[B] > 0 )
    {
      cambioDeValor( --valor[B], (void *)(long int)B );
      setTrackbarPos("B", fORG, valor[B]);
    }
   break;
  case 'B':
    if ( valor[B] < 255 )
    {
      cambioDeValor( ++valor[B], (void *)(long int)B );
      setTrackbarPos("B", fORG, valor[B]);
    }
   break;
  default: ;    
  } // Fin de "switch ( tecla )"  
}// Fin de  "for(;;)"

 printf( "Preparados para salir\n" );

 return 0;
}



//void cambioDeValor( int posBarraDesplz )
void cambioDeValor( int posBarraDesplz, void *userdata )   
{
  int canal = (int)(long int)(userdata);
  valor[canal] = posBarraDesplz;
  printf("El nuevo valor[%c] de inicialización es %d (%d) \n",
	 (canal==R? 'R' : (canal==G? 'G' : 'B')), 
	 (canal==R? valor[R] : (canal==G? valor[G] : valor[B])),
	 posBarraDesplz);
}// Fin de "void cambioDeValor(...



//
// Inicializa una imagen a un valor
// Entrada: imgOrg - la imagen de partida (se obvia)
//          valor - el valor de color a usar
//
// Salida: imgDst - la imagen resultado (en color) resultante.
//
int procesarImg( Mat imgOrg, Mat imgDst, int valor )
{

  if (colorAnterior != valor)
  {
    colorAnterior = valor;

    imgDst = Mat::zeros(imgDst.rows, imgDst.cols, imgDst.depth());  
    add( imgDst, Scalar::all( valor ), imgDst); //, NULL );
  }
 return( 0 );
} // Fin de "int procesarImg( IplImage *imgOrg, int umbral, IplImage *imgDst )"




//
// Procesado de eventos del ratón
void my_mouse_callback( int event, int x, int y, int flags, void* param )
{
  Mat *image = (Mat *) param;
  Vec3b elColor;

  switch( event ) {

  case EVENT_MOUSEMOVE: 

    if( estaPintado == CIERTO )
    {
     // Mat guarda en orden BGR
     image->at<Vec3b>(y, x) = Vec3b( valor[B], valor[G], valor[R] );  
    }
    imshow( fORG, *image );
    break;

  case EVENT_LBUTTONDOWN: 
    estaPintado = CIERTO;
    image->at<Vec3b>(y, x) = Vec3b( valor[B], valor[G], valor[R] );
    break;

  case EVENT_LBUTTONUP: 

    estaPintado = FALSO;
    break;

  case EVENT_RBUTTONUP: 

    elColor = image->at<Vec3b>(y, x); //cvGet2D(image, y, x);
    if (image->channels() == 1)
      printf(" En coordenadas %d,%d color %d\n", y, x, (int)elColor.val[0]);
    else
      printf(" En coordenadas %d,%d color %d, %d, %d\n", y, x, 
             (int)elColor.val[R], (int)elColor.val[G], (int)elColor.val[B]);
    break;
  } // fi del switch
}// fi de "void my_mouse_callback( ..."




//
// Fi del fitxer opencv_events.cpp
