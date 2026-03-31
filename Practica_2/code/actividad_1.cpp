#include <iostream>
#include <string>
#include <cstring>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include "comun.h"
#include "buscarLaser.h"
using namespace cv;
using namespace std;

#define fORG "Imagen original"
#define fDST  "Imagen umbralizada"
#define fOBJ "Imagen seguimiento"
#define fHisto "Histograma"

#define fCanalR "Canal R"
#define fCanalG "Canal G"
#define fCanalB "Canal B"

#define saveDirectory "bin/actividad_1_img/"

#define IMG_SIZE 312

Mat imgHist;
int umbral_value = 210;

// Para guardar un vídeo
bool saveOutput = false;

// Componente de histograma
int hist_w = 512; int hist_h = 256; int histSize = 125;
int bin_w = cvRound( (double) hist_w/histSize );
float range[] = { 0, 256 } ;
const float* histRange = { range };
bool accumulate_hist = false;

Mat b_hist, g_hist, r_hist;


// Declaraciones de prototipos locales
void reubicate_windows();
void umbralize_img(Mat* origen, Mat* destino);
void my_mouse_callback( int event, int x, int y, int flags, void* param);
void pintarHistogramaRGB(Mat* src, Mat b_hist, Mat g_hist, Mat r_hist, int histSize);
void mostrarHistograma(Mat* src);

//
// Programa principal
//
int main(int argc, char* argv[]) 
{
    Mat imgOrg, imgUmb, imgObj; 
    VideoCapture cap;
    String ruteVideo, ruteSaveVideo;
    VideoWriter writer;

    // Parametros para el video  
    int fps = 5;
    int delay = 1000 / fps;

    int tecla;  // Tecla leída
    bool salir = false, 
        endVideo = false, 
        pause = false; // Controla la condición de terminación de la aplicación

    if (argc == 2)
    {
        ruteVideo = argv[1];
        cap.open(ruteVideo);
        cout << "Loading video: " << ruteVideo << endl;
    }
    else if (argc == 3)
    {
        ruteVideo = argv[1];
        cap.open(ruteVideo);

        ruteSaveVideo = argv[2];
        writer.open(ruteSaveVideo, 
                    VideoWriter::fourcc('M','J','P','G'), 
                    fps, 
                    Size(IMG_SIZE, IMG_SIZE),
                    true);

        saveOutput = true;
        cout << "Loading video: " << ruteVideo << endl << "Saving video: " << ruteSaveVideo << endl;
    }
    else
    {
        cout << "Uso: " << saveDirectory << "buscarLaser <Ruta del vídeo> Opcional:<Ruta de guardado>" << endl;
        return -1;
    }
    

    if(!cap.isOpened())
    {
        printf("Error opening video...\n");
        return -1;
    }

    printf("OpenCV %s == %d.%d.%d.%d.\n",
    CV_VERSION,
    CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION, CV_VERSION_REVISION );

    // Inicializar tamanyo ventanas
    namedWindow(fORG, WINDOW_NORMAL);
    namedWindow(fDST, WINDOW_NORMAL);
    namedWindow(fOBJ, WINDOW_NORMAL);
    namedWindow(fHisto, WINDOW_NORMAL);

    // Ventanas canales colores
    namedWindow(fCanalB, WINDOW_NORMAL);
    namedWindow(fCanalG, WINDOW_NORMAL);
    namedWindow(fCanalR, WINDOW_NORMAL);

    while (!salir && !endVideo )
    { 
        if(!pause)
        {
            if (!cap.grab())
            {
                endVideo = true;
            }
            else
            {
                Mat imgOrgResize;
                cap.retrieve(imgOrg, 0 );
                resize(imgOrg, imgOrgResize, Size(IMG_SIZE, IMG_SIZE));

                imshow(fORG, imgOrgResize); 

                Mat canales[3];
                split(imgOrgResize, canales);   // Separa B,G,R

                imshow(fCanalB, canales[0]); 
                imshow(fCanalG, canales[1]); 
                imshow(fCanalR, canales[2]); 

                umbralize_img(&canales[2], &imgUmb);
                imshow(fDST, imgUmb); 

                imgOrgResize.copyTo(imgObj);
                Op_buscar_laser(&imgUmb, &imgObj);
                imshow(fOBJ, imgObj);
                if(saveOutput)
                {                    
                    writer.write(imgObj);
                }

                mostrarHistograma(&imgOrg);

                reubicate_windows();
            }
        }

        tecla = waitKey(delay) & 255;

        switch (tecla)
        {
        case ESC:
        case 'q':
        case 'Q':
            salir = true;
        break;

        case 'p':
        case 'P':
            pause = !pause;
            break;

        default: break;    
        }  
    }

    writer.release();

    if(salir)
    {
        cout << "Usuario saliendo" << endl;
    }
    else if(endVideo)
    {
        cout << "Video finalizado" << endl;
    }

    return 0;
}


void umbralize_img(Mat* origen, Mat* destino)
{
    threshold(*origen, *destino, umbral_value, 255, THRESH_BINARY);
}


void reubicate_windows()
{
    resizeWindow(fORG, IMG_SIZE, IMG_SIZE);
    moveWindow(fORG, IMG_SIZE, 0);

    resizeWindow(fDST, IMG_SIZE, IMG_SIZE);
    moveWindow(fDST, IMG_SIZE*2, 0);

    resizeWindow(fOBJ, IMG_SIZE, IMG_SIZE);
    moveWindow(fOBJ, IMG_SIZE*3, 0);

    resizeWindow(fHisto, IMG_SIZE*2, IMG_SIZE);
    moveWindow(fHisto, IMG_SIZE*1.5, IMG_SIZE*2.2);

    resizeWindow(fCanalB, IMG_SIZE, IMG_SIZE);
    moveWindow(fCanalB, IMG_SIZE, IMG_SIZE*1.1);

    resizeWindow(fCanalG, IMG_SIZE, IMG_SIZE);
    moveWindow(fCanalG, IMG_SIZE*2, IMG_SIZE*1.1);

    resizeWindow(fCanalR, IMG_SIZE, IMG_SIZE);
    moveWindow(fCanalR, IMG_SIZE*3, IMG_SIZE*1.1);
}


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


// Procesado de eventos del ratón
void my_mouse_callback(int event, int x, int y, int flags, void* param)
{ 
  char messageText[1024];
  int histSize = 120;

  switch( event ) {     
    case EVENT_MOUSEMOVE:
        pintarHistogramaRGB(&imgHist, b_hist, g_hist, r_hist, histSize );
        line(imgHist, Point( (x), 0 ), Point( (x), hist_h ), Scalar( 255, 255, 255), 2, 8, 0  );
        sprintf(messageText,
            "RGB (%d): (%d,%d,%d)", cvRound(x/bin_w), cvRound(r_hist.at<float>(cvRound(x/bin_w))),
            cvRound(g_hist.at<float>(cvRound(x/bin_w))), cvRound(b_hist.at<float>(cvRound(x/bin_w))) );
        printfXY(&imgHist, (char *)fHisto, messageText);
        imshow(fHisto, imgHist);
        break;

    default: break;
  }
}// fi de "void my_mouse_callback( ..."


void pintarHistogramaRGB(Mat* src, Mat b_hist, Mat g_hist, Mat r_hist, int histSize)
{ 
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
    imshow(fHisto, *src);
} 

void mostrarHistograma(Mat* src)
{
  vector<Mat> bgr_planes;
  bool uniform = true; 
  
  split(*src, bgr_planes);
  calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate_hist);
  calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate_hist);
  calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate_hist);

  pintarHistogramaRGB(src, b_hist, g_hist, r_hist, histSize);

  setMouseCallback(fHisto, my_mouse_callback, (void*)1);
} // Fi de  mostrarHistogramaRGB

// Fi del fitxer opencv_events.cpp
