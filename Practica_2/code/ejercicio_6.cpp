/*
 opencv_Histo1D.cpp
 En este ejemplo se describirá cómo mostrar el histograma de una imagen

 El càlcul i pintat de l'histograma s'ha tret de l'exemple que acompanya a OpenCV: 
  <https://docs.opencv.org/3.2.0/d8/dbc/tutorial_histogram_calculation.html>

 Per a OpenCV 4.X:
 $ g++ opencv_mostrarHisto1D.cpp -o opencv_mostrarHisto1D -DOPENCV4 ` pkg-config opencv4 --cflags --libs` && opencv_mostrarHisto1D al.pgm

 S'executa en 
 $ opencv_Histo1D lena.jpg

*/
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <string>
#include <cstring>

using namespace std;
using namespace cv;

#define fGRIS "¡OpenCV en gris!"
#define fHistoGRIS "Histograma de grises"

#define fYcrCb "¡OpenCV en YcrCb!"
#define fHistoYcrCb "Histograma de YcrCb"

#define fHSV "¡OpenCV en HSV!"
#define fHistoHSV "Histograma de HSV"

#define fHLS "¡OpenCV en HLS!"
#define fHistoHLS "Histograma de HLS"

#define fRGB  "¡OpenCV en color!"
#define fHistoRGB "Histograma de color"

// Ample i alt de la finestra per a mostrar els histogrames
int hist_w = 512; int hist_h = 256; // 400;
// Histogrames per a les components de color original
Mat b_hist, g_hist, r_hist;

Mat v1_hist, v2_hist, v3_hist;

// Histograma per a la imatge de nivells de gris i la de color
Mat histGris;
// Imatges sobre les que pintar els histogrames
Mat imgHistRGB, imgHistGris, imgHistTansformed;

Mat transformed_src;

int transform_num = 0;

// Prototips de les funcions
void mostrarHistogramaRGB( Mat src);
void mostrarHistogramaTransformed( Mat src, String title);
void mostrarHistogramaGris( Mat src );
void accioRatoli(int event, int x, int y, int flags, void* param  );


int main(int argc, char** argv)
{
  Mat src;
  String imageName = "code/images/lena.jpg"; // Imagen default
  
  if (argc > 2) 
  {
    transform_num = stoi(argv[1]);
    imageName = argv[2];
  }
  else if (argc > 1)
  {
    transform_num = stoi(argv[1]);
  }
  else
  {
    printf("Cogiendo valores default\n");
    printf("Uso: ./bin/ejercicio_6 <Transformacion> <Ruta imagen>\n");
    printf("Transformaciones disponibles:\n  0-No transforma\n  1-Transforma a grises\n  2-Transforma TcrCb\n");
    printf("  3-Transforma HSV\n  4-Transforma HLS\n");
    return -1;
  }

  // Comprueba que la transformación sea correcta
  if (transform_num > 4)
  {
    printf("Valor de transformacion incorrecto\n");
    return -1;
  }
  
  src = imread( imageName, IMREAD_UNCHANGED); //Ficava: IMREAD_COLOR );
  
  if( src.empty() )
  { 
    return -1; 
  }
  
  namedWindow( fRGB, WINDOW_AUTOSIZE );
  imshow( fRGB, src );
  mostrarHistogramaRGB(src);

  switch(transform_num)
  {
    case 1:
      cvtColor(src, transformed_src, COLOR_BGR2GRAY);

      namedWindow( fGRIS, WINDOW_AUTOSIZE );
      imshow( fGRIS, transformed_src );
      mostrarHistogramaGris( transformed_src );
      break;

    case 2:
      cvtColor(src, transformed_src, COLOR_BGR2YCrCb);

      namedWindow( fYcrCb, WINDOW_AUTOSIZE );
      imshow( fYcrCb, transformed_src );
      mostrarHistogramaTransformed( transformed_src, fHistoYcrCb);
      break;

    case 3:
      cvtColor(src, transformed_src, COLOR_BGR2HSV);

      namedWindow( fHSV, WINDOW_AUTOSIZE );
      imshow( fHSV, transformed_src );
      mostrarHistogramaTransformed( transformed_src, fHistoHSV);
      break;

    case 4:
      cvtColor(src, transformed_src, COLOR_BGR2HLS);

      namedWindow( fHLS, WINDOW_AUTOSIZE );
      imshow( fHLS, transformed_src );
      mostrarHistogramaTransformed( transformed_src, fHistoHLS);
      break;

    default: 
      printf("Argumento transformacion invalido");
    break;
  }

  waitKey(0);
  exit( 0 ); //  return 0;
} // Fi de main



void pintarHistogramaTransformed( String title, Mat v1_hist, Mat v2_hist, Mat v3_hist, int histSize )
{  
  // Draw the histograms for B, G and R
  int bin_w = cvRound( (double) hist_w/histSize );
  
  imgHistTansformed.create( hist_h, hist_w, CV_8UC3 );
  imgHistTansformed = Scalar( 0,0,0);

  normalize(v1_hist, v1_hist, 0, imgHistTansformed.rows, NORM_MINMAX, -1, Mat() );
  normalize(v2_hist, v2_hist, 0, imgHistTansformed.rows, NORM_MINMAX, -1, Mat() );
  normalize(v3_hist, v3_hist, 0, imgHistTansformed.rows, NORM_MINMAX, -1, Mat() );
  for( int i = 1; i < histSize; i++ )
    {
      line( imgHistTansformed, Point( bin_w*(i-1), hist_h - cvRound(v1_hist.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(v1_hist.at<float>(i)) ),
	    Scalar( 255, 0, 0), 2, 8, 0  );
      line( imgHistTansformed, Point( bin_w*(i-1), hist_h - cvRound(v2_hist.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(v2_hist.at<float>(i)) ),
	    Scalar( 0, 255, 0), 2, 8, 0  );
      line( imgHistTansformed, Point( bin_w*(i-1), hist_h - cvRound(v3_hist.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(v3_hist.at<float>(i)) ),
	    Scalar( 0, 0, 255), 2, 8, 0  );
    }
  namedWindow(title, WINDOW_AUTOSIZE );
  imshow(title, imgHistTansformed );
} // Fi de  mostrarHistogramaRGB

void mostrarHistogramaTransformed( Mat src, String title )
{
  vector<Mat> bgr_planes;
  int histSize = 256;
  float range[] = { 0, 256 } ;
  const float* histRange = { range };
  bool uniform = true; bool accumulate = false;
    
  split( src, bgr_planes );
  calcHist( &bgr_planes[0], 1, 0, Mat(), v1_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[1], 1, 0, Mat(), v2_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[2], 1, 0, Mat(), v3_hist, 1, &histSize, &histRange, uniform, accumulate );
  
  pintarHistogramaTransformed( title, v1_hist, v2_hist, v3_hist, histSize );
  
  setMouseCallback( title, accioRatoli, (void *)1 ); // 3 canals --> Hist. de RGB

} // Fi de  mostrarHistogramaRGB

void pintarHistogramaRGB( Mat b_hist, Mat g_hist, Mat r_hist, int histSize )
{  
  // Draw the histograms for B, G and R
  int bin_w = cvRound( (double) hist_w/histSize );
  
  // imgHistRGB.create( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  imgHistRGB.create( hist_h, hist_w, CV_8UC3 );
  imgHistRGB = Scalar( 0,0,0);

  normalize(b_hist, b_hist, 0, imgHistRGB.rows, NORM_MINMAX, -1, Mat() );
  normalize(g_hist, g_hist, 0, imgHistRGB.rows, NORM_MINMAX, -1, Mat() );
  normalize(r_hist, r_hist, 0, imgHistRGB.rows, NORM_MINMAX, -1, Mat() );
  for( int i = 1; i < histSize; i++ )
    {
      line( imgHistRGB, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
	    Scalar( 255, 0, 0), 2, 8, 0  );
      line( imgHistRGB, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
	    Scalar( 0, 255, 0), 2, 8, 0  );
      line( imgHistRGB, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
	    Scalar( 0, 0, 255), 2, 8, 0  );
    }
  namedWindow(fHistoRGB, WINDOW_AUTOSIZE );
  imshow(fHistoRGB, imgHistRGB );
} // Fi de  mostrarHistogramaRGB

void mostrarHistogramaRGB( Mat src)
{
  vector<Mat> bgr_planes;
  int histSize = 256;
  float range[] = { 0, 256 } ;
  const float* histRange = { range };
  bool uniform = true; bool accumulate = false;
    
  split( src, bgr_planes );
  calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );
  
  pintarHistogramaRGB(b_hist, g_hist, r_hist, histSize );
  
  setMouseCallback( fHistoRGB, accioRatoli, (void *)3 ); // 3 canals --> Hist. de RGB

} // Fi de  mostrarHistogramaRGB


// void pintarHistogramaGris( Mat hist, int histSize, Mat histImage )
void pintarHistogramaGris( Mat histGris, int histSize )
{  
  // Dibuixa el histograma
  //int hist_w = 512; int hist_h = 256; // 400;
  int bin_w = cvRound( (double) hist_w/histSize );
  //Mat
  imgHistGris.create( hist_h, hist_w, CV_8UC1);
  imgHistGris = Scalar( 0,0,0);
  normalize(histGris, histGris, 0, imgHistGris.rows, NORM_MINMAX, -1, Mat() );
  for( int i = 1; i < histSize; i++ )
    {
      line( imgHistGris, Point( bin_w*(i-1), hist_h - cvRound(histGris.at<float>(i-1)) ) ,
	    Point( bin_w*(i), hist_h - cvRound(histGris.at<float>(i)) ),
	    Scalar( 255, 255, 255), 2, 8, 0  );
    }
  namedWindow(fHistoGRIS, WINDOW_AUTOSIZE );
  imshow(fHistoGRIS, imgHistGris );
} // Fi de  mostrarHistogramaRGB


void mostrarHistogramaGris( Mat src )
{
  int histSize = 256;
  float range[] = { 0, 256 } ;
  const float* histRange = { range };
  bool uniform = true; bool accumulate = false;
    
  calcHist( &src, 1, 0, Mat(), histGris, 1, &histSize, &histRange, uniform, accumulate );
  
  pintarHistogramaGris( histGris, histSize );
  
  setMouseCallback( fHistoGRIS, accioRatoli, (void *)1 ); // 1 canal --> Hist. de gris
} // Fi de  mostrarHistogramaGris


# define ALT_FILA 30
// Exemples de la font Hershey <https://www.codesofinterest.com/2017/07/more-fonts-on-opencv.html>
void printfXY( Mat *image, char *nomFinestra, char *mensatge )
{
  double hScale = 1.0, 
    vScale = 0.5, 
    lineWidth = 1.0, 
    italicScale = 1.0;
  int tipoLletra = FONT_HERSHEY_SIMPLEX,

  lineType = LINE_AA; 

  // static
  int columna = cvRound(hist_w / 3), fila = 30; //ALT_FILA;                      
  
  putText( *image, mensatge, Point( columna, fila ),	   
	        tipoLletra, vScale, CV_RGB(255,0,255), lineWidth ); //, lineType);
  imshow(nomFinestra, *image);

  fila += ALT_FILA;
}// Fi de "void printfXY( ..."



void accioRatoli( int event, int x, int y, int flags, void* param  ) {
  long int nCanals = (long int) param;
  Mat imgTemporal;
  char textDelMensatge[1024];
  char nomFinestraPpal[1024] = fHistoGRIS; 

  char title[1024];

  int histSize = 256;
  int bin_w = cvRound( (double) hist_w/histSize );
    
  switch(event) 
  {
    case EVENT_MOUSEMOVE:     
      if (nCanals == 3)
      {
        pintarHistogramaRGB(b_hist, g_hist, r_hist, histSize );
        line( imgHistRGB, Point( (x), 0 ), Point( (x), hist_h ), Scalar( 255, 255, 255), 2, 8, 0  );
        sprintf( textDelMensatge,
                "RGB (%d): (%d,%d,%d)", cvRound(x/bin_w), cvRound(r_hist.at<float>(cvRound(x/bin_w))),
                  cvRound(g_hist.at<float>(cvRound(x/bin_w))), cvRound(b_hist.at<float>(cvRound(x/bin_w))) );
        
        printfXY( &imgHistRGB, (char *)fHistoRGB, textDelMensatge );
        imshow( fHistoRGB, imgHistRGB);
      }
      else
      {    
        switch(transform_num)
        {
          case 1:
            pintarHistogramaGris( histGris, histSize );
            line( imgHistGris, Point( (x), 0 ), Point( (x), hist_h ), Scalar( 255, 255, 255), 2, 8, 0  );
            sprintf( textDelMensatge,
                    "Gris (%d): %d", cvRound(x/bin_w), cvRound(histGris.at<float>(cvRound(x/bin_w))) );
            
            printfXY( &imgHistGris, (char *)fHistoGRIS, textDelMensatge );
            imshow( fHistoGRIS, imgHistGris); 
            break;
          
          case 2:
            strcpy(title, fHistoYcrCb);
            break;   
          
          case 3:
            strcpy(title, fHistoHSV);
            break;   

          case 4:
            strcpy(title, fHistoHLS);
            break;   

          default: break;
        }  

        if(transform_num != 1)
        {
          pintarHistogramaTransformed(title, v1_hist, v2_hist, v3_hist, histSize );
          line( imgHistTansformed, Point( (x), 0 ), Point( (x), hist_h ), Scalar( 255, 255, 255), 2, 8, 0  );
          sprintf( textDelMensatge,
                  "RGB (%d): (%d,%d,%d)", cvRound(x/bin_w), cvRound(v3_hist.at<float>(cvRound(x/bin_w))),
                    cvRound(v2_hist.at<float>(cvRound(x/bin_w))), cvRound(v1_hist.at<float>(cvRound(x/bin_w))) );
        
          printfXY( &imgHistTansformed, (char *)title, textDelMensatge );
          imshow( title, imgHistTansformed);
        }
      } 
      break;     
  } // fi del switch
} // Fi de void accióRatolí
