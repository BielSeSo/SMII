//
// opencv_events.cpp
//
// $ g++ opencv_events.cpp -o opencv_events `pkg-config opencv --cflags --libs`
//
#include <iostream>
#include <string>
#include <cstring>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include "comun.h"
using namespace cv;
using namespace std;

#define fORG "Imagen original"
#define fDST  "Imagen umbralizada"
#define fHisto "Histograma"

#define IMG_SIZE 250
#define MAX_UMB 255

Mat imgOrg, imgDst, imgHist; 
int umbral_value = 0;
int umbral_bar = 3;
bool umbralize = false;

// Componente de histograma
int hist_w = 512; int hist_h = 256;
int histSize = 256;
int bin_w = cvRound( (double) hist_w/histSize );
float range[] = { 0, 256 } ;
const float* histRange = { range };
bool accumulate_hist = false;


Mat b_hist, g_hist, r_hist;
Mat histGris;

// Declaraciones de prototipos locales
void cambioDeValor( int pos, void *userdata );
int procesarImg( Mat imgOrg, Mat imgDst, int valor );
void my_mouse_callback(int event, int x, int y, int flags, void* param  );
int inicializarImgGris( Mat *imgOrg );
void pintarHistogramaRGB( Mat b_hist, Mat g_hist, Mat r_hist, int histSize );
void pintarHistogramaGris( Mat histGris, int histSize );
void mostrarHistograma( Mat src);


//
// Programa principal
//
int main(int argc, char* argv[]) 
{
  String rute;

  int tecla;  // Tecla leída
  bool salir; // Controla la condición de terminación de la aplicación

  if (argc == 2)
  {
    rute = argv[1];
    printf("Loading image: %s", argv[1]);
    imgOrg = imread(rute);
  }
  else
  {
    imgOrg.create(IMG_SIZE, IMG_SIZE, CV_8UC1); 
    inicializarImgGris(&imgOrg);
  }

  imgDst.create(IMG_SIZE, IMG_SIZE, CV_8UC1);
 
  printf("OpenCV %s == %d.%d.%d.%d.\n",
  CV_VERSION,
  CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION, CV_VERSION_REVISION );

  if (imgOrg.empty())
  {
    fprintf(stderr, "Problemas al crear la imagen de partida.\n");
    return( 1 );
  }

  namedWindow( fORG, WINDOW_NORMAL); //_AUTOSIZE ); 
  moveWindow( fORG, 0, 0 );
  imshow( fORG, imgOrg ); 

  namedWindow( fDST, WINDOW_NORMAL);
  moveWindow( fDST, 0, 0 );
  imshow( fDST, imgDst ); 


  createTrackbar( "Umbral value", fORG, NULL, MAX_UMB, cambioDeValor, (void *)(long int)umbral_bar );
  cambioDeValor( umbral_value, (void *)(long int)umbral_bar);

  setMouseCallback( fORG, my_mouse_callback, (void*)(long) imgOrg.channels());

  mostrarHistograma(imgOrg);

  salir = false;

  while ( !salir )
  { 
    tecla = waitKey(25) & 255;  // Espera una tecla los milisegundos que haga falta
  
    switch ( tecla )
    {
      case ESC:
      case 'q':
      case 'Q':    // Si 'ESC', q o Q, ¡acabar!
        salir = true;
        break;
          
      case 'a':
        if ( umbral_value > 0 )
        {
          cambioDeValor( --umbral_value, (void *)(long int)umbral_bar );
          setTrackbarPos("Umbral value", fORG, umbral_value);
        }
        break;

      case 'd':
        if ( umbral_value < 255 )
        {
          cambioDeValor( ++umbral_value, (void *)(long int)umbral_bar );
          setTrackbarPos("Umbral value", fORG, umbral_value);
        }
        break;

      default: break;    
    } // Fin de "switch ( tecla )"  
  }// Fin de  "for(;;)"

  printf( "Preparados para salir\n" );

  return 0;
}


void cambioDeValor( int posBarraDesplz, void *userdata )   
{
  umbral_value = posBarraDesplz;
} // Fin de "void cambioDeValor(...


# define ALT_FILA 30

void printfXY( Mat *image, char *nomFinestra, char *mensatge )
{
  double hScale = 1.0, 
    vScale = 0.5, 
    lineWidth = 1.0, 
    italicScale = 1.0;
  int tipoLletra = FONT_HERSHEY_SIMPLEX,

  lineType = LINE_AA; 

  // static
  int columna = cvRound(hist_w / 3), fila = 30;                      
  
  putText( *image, mensatge, Point( columna, fila ),	   
	        tipoLletra, vScale, CV_RGB(255,0,255), lineWidth ); //, lineType);
  imshow(nomFinestra, *image);

  fila += ALT_FILA;
}// Fi de "voi


//
// Procesado de eventos del ratón
void my_mouse_callback( int event, int x, int y, int flags, void* param )
{ 
  long int nCanals = (long int) param;
  Mat imgTemporal;
  char textoDelMensaje[1024];

  char title[1024];

  int histSize = 256;

  switch( event ) {
    case EVENT_LBUTTONDOWN: 
      umbralize = true;

      if (umbralize)
      {
        threshold(imgOrg, imgDst, umbral_value, 255, THRESH_BINARY);
        imshow( fDST, imgDst);
      }
      break;

    case EVENT_LBUTTONUP:
      umbralize = false;
      break;
      
    case EVENT_MOUSEMOVE:
    
      if(nCanals == 1)   
      {
        pintarHistogramaGris( histGris, histSize );
        line( imgHist, Point( (x), 0 ), Point( (x), hist_h ), Scalar( 255, 255, 255), 2, 8, 0  );
        sprintf( textoDelMensaje,
                "Gris (%d): %d", cvRound(x/bin_w), cvRound(histGris.at<float>(cvRound(x/bin_w))));
        
        printfXY( &imgHist, (char *)fHisto, textoDelMensaje );
        imshow( fHisto, imgHist);
      } 
      else 
      {
        pintarHistogramaRGB(b_hist, g_hist, r_hist, histSize );
        line( imgHist, Point( (x), 0 ), Point( (x), hist_h ), Scalar( 255, 255, 255), 2, 8, 0  );
        sprintf( textoDelMensaje,
                "RGB (%d): (%d,%d,%d)", cvRound(x/bin_w), cvRound(r_hist.at<float>(cvRound(x/bin_w))),
                  cvRound(g_hist.at<float>(cvRound(x/bin_w))), cvRound(b_hist.at<float>(cvRound(x/bin_w))) );
        
        printfXY( &imgHist, (char *)fHisto, textoDelMensaje );
        imshow( fHisto, imgHist);
      }
      break;

    default: break;
  }
}// fi de "void my_mouse_callback( ..."


int  inicializarImgGris( Mat *imgOrg ) 
{
  int x, y;

  for ( y = 0; y < imgOrg->rows; y++ )
    for ( x = 0; x < imgOrg->cols; x++ )
    {
      imgOrg->at<uchar>(y, x) = x;
    }
 return( 0 );
} 

void pintarHistogramaRGB( Mat b_hist, Mat g_hist, Mat r_hist, int histSize )
{  
  imgHist.create( hist_h, hist_w, CV_8UC3 );
  imgHist = Scalar( 0,0,0);

  normalize(b_hist, b_hist, 0, imgHist.rows, NORM_MINMAX, -1, Mat() );
  normalize(g_hist, g_hist, 0, imgHist.rows, NORM_MINMAX, -1, Mat() );
  normalize(r_hist, r_hist, 0, imgHist.rows, NORM_MINMAX, -1, Mat() );
  for( int i = 1; i < histSize; i++ )
    {
      line( imgHist, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
	    Scalar( 255, 0, 0), 2, 8, 0  );
      line( imgHist, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
	    Scalar( 0, 255, 0), 2, 8, 0  );
      line( imgHist, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
	    Scalar( 0, 0, 255), 2, 8, 0  );
    }
  namedWindow(fHisto, WINDOW_AUTOSIZE );
  imshow(fHisto, imgHist );
} 

void pintarHistogramaGris( Mat histGris, int histSize )
{  
  imgHist.create( hist_h, hist_w, CV_8UC1);
  imgHist = Scalar( 0,0,0);
  normalize(histGris, histGris, 0, imgHist.rows, NORM_MINMAX, -1, Mat() );
  for( int i = 1; i < histSize; i++ )
    {
      line( imgHist, Point( bin_w*(i-1), hist_h - cvRound(histGris.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(histGris.at<float>(i)) ),
	    Scalar( 255, 255, 255), 2, 8, 0  );
    }
  namedWindow(fHisto, WINDOW_AUTOSIZE );
  imshow(fHisto, imgHist );
}


void mostrarHistograma( Mat src)
{
  vector<Mat> bgr_planes;
  bool uniform = true; 
  
  if (src.channels() == 1) 
  {
    // Histograma en gris
    calcHist( &src, 1, 0, Mat(), histGris, 1, &histSize, &histRange, uniform, accumulate_hist );
    pintarHistogramaGris( histGris, histSize );
  
    setMouseCallback( fHisto, my_mouse_callback, (void *)1 ); // 1 canal --> Hist. de gri
  } 
  else 
  {
    // Histograma BGR
    vector<Mat> bgr_planes;
    split(src, bgr_planes);
    calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate_hist);
    calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate_hist);
    calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate_hist);

    pintarHistogramaRGB(b_hist, g_hist, r_hist, histSize);

    setMouseCallback(fHisto, my_mouse_callback, (void*)3); // 3 canales -> histograma RGB
  }
} // Fi de  mostrarHistogramaRGB


//
// Fi del fitxer opencv_events.cpp
