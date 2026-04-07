//
// Fitxer opencv_graficsText.cpp
// 
// Asignatura SMII. 
// Versió 0 (Oct. 2018): M. Agustí.
//
// En este ejemplo se muestra el uso de primitivas gráficas para dibujar sobre una imagen.
//
// $ g++ opencv_graficsText.cpp -o _graficsText `pkg-config opencv --cflags --libs`
// $ make opencv__graficsText && opencv_graficsText
//

#include <stdio.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "comun.h"

using namespace cv;

Mat image;
int tecla = 0,
    cont = 0;


void save_image(char tecla);
void rectangles( Mat *image, char *nomFinestra ); // void rectangles( IplImage *image, char * nomFinestra );
void cercles( Mat *image, char *nomFinestra ); //void circles( IplImage *image, char *nomFinestra );
void polilinies( Mat *image, char *nomFinestra ); // void polilinies( IplImage *image, char *nomFinestra );
void printfXY( Mat *image, char *nomFinestra,  char *mensatge ); //void printfXY( IplImage* image, char *nomFinestra,  char *mensatge );
void  pintaCruz( Mat* image, int fila, int columna, int ancho );
void  pintaHistograma(Mat* image, int margeBaix, int margeEsquerra);


char textDelMensatge[1024];
// Unicode: wchar vs String ???
//String textDelMensatge;
/*
  The ISO/ANSI C standard contains, in an amendment which was added in 1995, a "wide character" type `wchar_t', a set of functions like those found in <string.h> and <ctype.h> (declared in <wchar.h> and <wctype.h>, respectively), and a set of conversion functions between `char *' and `wchar_t *' (declared in <stdlib.h>).*/ 


// https://stackoverflow.com/questions/8032080/how-to-convert-char-to-wchar-t
// https://www.dreamincode.net/forums/topic/177689-mbstowcs/
//Converts a multibyte character string from the array whose first element is pointed to by c
const wchar_t *GetWC(const char *c)
{
    const size_t cSize = strlen(c)+1;
    wchar_t* wc = new wchar_t[cSize];

    mbstowcs (wc, c, cSize); 

    return wc;
}

int main( int argc, char* argv[] ) 
{
 // Valors inicials:
  char nomFinestraPpal[1024] = "Exemples de text: áà";
 int ampleFinestra = 1024, 
   altFinestra = 768,
   nVoltes = 5,
   i, j;

 // printf( "%s\n", nomFinestraPpal.c_str() );  // Si està definida com a String
 //printf( "%s\n", nomFinestraPpal );
 
 //setlocale( LC_ALL, "" ); //?
 sprintf(textDelMensatge, "OpenCV %s == %d.%d.%d.%d.",
	  CV_VERSION,
	  CV_MAJOR_VERSION, CV_MINOR_VERSION,
	  CV_SUBMINOR_VERSION, CV_VERSION_REVISION );
 printf("%s\n", textDelMensatge );

 image = Mat::zeros(altFinestra, ampleFinestra, CV_8UC3);
 image = CV_RGB(255,255,255 );

 // https://docs.opencv.org/3.4/dc/d46/group__highgui__qt.html
 // Qt New Functions
  namedWindow(nomFinestraPpal, WINDOW_AUTOSIZE);
 printfXY( &image, nomFinestraPpal, textDelMensatge );
 
 sprintf( textDelMensatge,
	  "Sobre una finestra de : %dx%d (columnas x filas):",
	  ampleFinestra, altFinestra);
 printf( "%s\n", textDelMensatge );
 printfXY( &image, nomFinestraPpal, textDelMensatge );
 imshow(nomFinestraPpal, image);

 
 rectangles( &image, nomFinestraPpal );
 sprintf( textDelMensatge, "Rectangles!" );
 printfXY( &image, nomFinestraPpal, textDelMensatge );
 imshow( nomFinestraPpal, image );


 cercles( &image, nomFinestraPpal );
 sprintf( textDelMensatge, "Cercles! Una tecla ..." );
 printfXY( &image, nomFinestraPpal, textDelMensatge );
 imshow( nomFinestraPpal, image );
 //printf( "%s\n", textDelMensatge );
 tecla = waitKey(0); 
 save_image(tecla);

 polilinies( &image, nomFinestraPpal );
 sprintf(textDelMensatge, "Polilínees! Una tecla ..." );
 printfXY( &image, nomFinestraPpal, textDelMensatge );
 imshow(nomFinestraPpal, image);
 //printf( "%s\n", textDelMensatge );
 tecla = waitKey(0);
 save_image(tecla);


 // Una creu
 pintaCruz( &image, 512, 384, 30 );
 sprintf( textDelMensatge, "Creu en (512, 384), ample 30. Una tecla ...");
 printfXY( &image, nomFinestraPpal, textDelMensatge );
 imshow( nomFinestraPpal, image );
 //printf( "%s\n", textDelMensatge );
 tecla = waitKey(0);
 save_image(tecla);

 
 // Un principi d'histograma
 pintaHistograma( &image, 40, 20 );
 sprintf( textDelMensatge, "¿Y un histograma?\n" );
 printfXY( &image, nomFinestraPpal, textDelMensatge );
 imshow( nomFinestraPpal, image );
 //printf( "%s\n", textDelMensatge );
 tecla = waitKey(0);
 save_image(tecla);


 printfXY( &image, nomFinestraPpal, (char *)"Una tecla per acabar" );
 tecla = waitKey(0) & 255;
 save_image(tecla);

 // Liberar recursos
 image.release();
 destroyWindow( nomFinestraPpal );

 return 0;
} // Fi de "main( ..."


void save_image(char tecla)
{
  char nomDst[100];

  switch (tecla)
    {
        case 'F':
        case 'f':
            sprintf( nomDst, "bin/ejercicio_4_img/primitiva_%d.jpg", cont);
            cont = cont + 1;
            if(imwrite(nomDst, image))
            {
                printf("Guarda't com a %s.\n", nomDst );
            }
        break;
    }
}

/*
  El que s'ocupa d'anar ficant rectangles y variacions d'estos
*/
void rectangles( Mat *image, char *nomFinestra ) 
{
 int i, j;
 int altFinestra = image->rows,
  ampleFinestra = image->cols,
  fila, columna,
  thickness = 1,
    line_type = LINE_AA,

  shift = 0;

 fila = (int)( image->rows / 3 );
 columna = (int)( (10*image->cols) / 32 );

 rectangle( *image, Point(columna, fila), Point(2*columna, 2*fila), 
	     CV_RGB(255,0,0), thickness, line_type );

 sprintf( textDelMensatge,
	  "Rectángulo en (col, fila) %d,%d a %d,%d. Una tecla continua.",
	  columna, fila, 2*columna, 2*fila );
 printfXY(image, nomFinestra, textDelMensatge );
 imshow( nomFinestra, *image );
 tecla = waitKey(0);
 save_image(tecla);

/* <http://http://docs.opencv.org/2.4/modules/core/doc/drawing_functions.html>
 * 
 thickness ? Thickness of lines that make up the rectangle. Negative values, like CV_FILLED , mean that the function has to draw a filled rectangle.
 lineType ? Type of the line. See the line() description.
 shift ? Number of fractional bits in the point coordinates.
 */
 rectangle( *image, Point(columna, 2*fila), Point(2*columna, 3*fila), 
	     CV_RGB(0, 255,0), FILLED, line_type );
    
 sprintf( textDelMensatge,
	  "Rectángulo relleno en (col, fila) %dx%d..%dx%d",
	  columna, 2*fila, 2*columna, 3*fila );
 printfXY(image, nomFinestra, textDelMensatge );
 imshow( nomFinestra, *image );
 tecla = waitKey(0);
 save_image(tecla);

}// Fi de "void rectangles( ..."



/*
  El que se ocupa de ir poniendo círculos y derivados
*/
void cercles( Mat *image, char *nomFinestra ) 
{
 int altFinestra = image->rows,
  ampleFinestra = image->cols,
  fila,
  columna,
  radio;
 Scalar color;

 int thickness = 1,
  line_type = 8,
  shift = 0;
 Size axes;

 double angle = 0,
  start_angle = 45, 
  end_angle = -45;
 

 fila = (int)( image->rows / 3 );
 columna = (int)( image->cols / 4 );
 radio = MIN( image->cols/6, image->rows/6 );

 circle( *image, Point(columna, fila), radio, CV_RGB(128,0,0), 
         thickness, line_type, shift );
 sprintf( textDelMensatge,
	  "Circunferència en (fila, col) %dx%d. Una tecla ...",
	  fila, columna);
 printfXY( image, nomFinestra, textDelMensatge );
 imshow( nomFinestra, *image );
 tecla = waitKey(0);
 save_image(tecla);

 circle( *image, Point(3*columna, fila), radio, CV_RGB(255,0,0), 
 	  FILLED, line_type, shift );

 sprintf( textDelMensatge,
	  "Cercle en (fila, col) %dx%d",
	  fila, 3*columna);
 printfXY( image, nomFinestra, textDelMensatge );
 imshow( nomFinestra, *image );
 tecla = waitKey(0);
 save_image(tecla);

  
 fila = 2 * (int)( image->rows / 3 );
 axes = Size( 3*radio, radio);

 ellipse( *image, Point(2*columna, fila), axes, angle, start_angle, end_angle, 
	  CV_RGB(255,0, 255), thickness, line_type, shift );
 sprintf( textDelMensatge,
	 "Arc en (fila, col) %dx%d. Una tecla para continuar ...",
	 fila, columna);
 printfXY( image, nomFinestra, textDelMensatge );
 imshow( nomFinestra, *image );
 tecla = waitKey(0);
 save_image(tecla);

 axes = Size( image->cols/3, image->rows/3);
 ellipse( *image, Point(columna, fila), axes, angle, start_angle, end_angle, 
	   CV_RGB(0,0,255), FILLED, line_type );

 sprintf( textDelMensatge,
	  "Óval en (fila, col) %dx%d. Una tecla para continuar ...\n",
	  fila, 2*columna);
 printfXY( image, nomFinestra, textDelMensatge );
 imshow( nomFinestra, *image );
 tecla = waitKey(0);
 save_image(tecla);

 
  
imshow(nomFinestra, *image);
}// Fi de "void circles(..."



void  pintaCruz( Mat *image, int columna, int fila, int ancho )
{
  int thickness = 1,
     line_type = LINE_AA,

     shift = 0;

 line( *image, Point(columna-ancho,fila), Point(columna+ancho, fila), 
       Scalar(0,255,128, 1), 3, line_type ); //thickness, line_type );
 line( *image, Point(columna, fila-ancho), Point(columna, fila+ancho), 
       Scalar(0,255,128, 1), 5, line_type); //thickness, line_type );

}// Fi de "void  pintaCruz( ..."


void  pintaHistograma(Mat* image, int margeBaix, int margeEsquerra)
{
 int contours = 1, is_closed = 1,
  thickness = 1,
   line_type = 8, //CV_AA,
  shift = 0;
 // Punts en format (columna, fila)
 int histo[]={10, 0, 30, 0, 10, 0, 100, 0, 80, 0, 140, 0, 50, 0, 10};
 int base,
     i=0, 
    separacioEntrePunts=10, 
    nPunts=15, nPuntsHisto;
 // Punts (coordenaes de pantalla a on pintar-lo: en fique dos més per vore si 
 Point  *puntsHisto;

 nPuntsHisto = nPunts + 2;
 puntsHisto = (Point*)malloc( nPuntsHisto * sizeof(puntsHisto[0]));
 base = (int)image->rows - margeBaix; 

 puntsHisto[0] = Point(margeEsquerra +(1*separacioEntrePunts), base+5);
 for (i=1; i<=nPunts; i++)
 {
  puntsHisto[i] = Point(margeEsquerra +(i*separacioEntrePunts), base - histo[i-1]); 
  /*
  printf("Punto %d: (base %d, histo[i] %d) -> (col, fila): %d,%d\n",
 	 i, base, histo[i-1],
	 margeEsquerra +(i*separacioEntrePunts), 
	 base - histo[i-1] );
  */
 } 
 puntsHisto[nPunts+1] = Point(margeEsquerra +(nPunts*separacioEntrePunts), base+5);
 fillConvexPoly( *image, puntsHisto, nPunts+2, CV_RGB(255,128,64), line_type, shift);
 polylines( *image, (const Point**)&puntsHisto, (const int*)&nPuntsHisto,              
            contours, is_closed,
           CV_RGB(0,255,0), 1, 8, 0); //CV_FILLED, line_type, shift);

 
/*
  cvShowImage(nomFinestra, image);
  printf("Una tecla para continuar ...\n");
  tecla = waitKey(0);
*/

}// Fi de " void  pintaHistograma( ..."



/*
  El que s'ocupa d'anar ficant Polilínees
*/
void polilinies( Mat *image, char *nomFinestra )
{
    int fila, columna, nPunts, i;
    Point *puntos;                //Point puntos[10];
    int thickness = 1,
    line_type = LINE_AA,
    shift = 1;
    bool relleno = false;
 
 nPunts = 10; //100;
 puntos=(Point*)malloc( nPunts * sizeof(puntos[0]));

 fila = image->rows;
 columna = image->cols;
 // printf("polilinees %dx%d\n", fila, columna);

 // Punts en format (columna, fila)
 puntos[0] = Point(0, 0);
 puntos[1] = Point(columna/4, 0);
 puntos[2] = Point(columna/4, fila/3); 
 puntos[3] = Point(columna/2, fila/2);
 puntos[4] = Point(columna-1, fila-1); 
 puntos[5] = Point(columna-1, 20);
 puntos[6] = Point(columna/2,fila-20);
 puntos[7] = Point(columna/2, 80);
 puntos[8] = Point(columna/3, 90);
 puntos[9] = Point(columna/2, 40);
 polylines( *image, (const Point**)&puntos, (const int*)&nPunts, 1, relleno,
	    CV_RGB(255,0,0), 1, 8, 0);//thickness, 5*line_type, shift);

 sprintf( textDelMensatge,
	  "Polilínea: (%d, %d), (%d,%d), (%d, %d), (%d,%d), ... Una tecla ...",
          puntos[0].x, puntos[0].y, puntos[1].x, puntos[1].y,
          puntos[2].x, puntos[2].y, puntos[3].x, puntos[3].y );
 printfXY( image, nomFinestra, textDelMensatge );
 imshow( nomFinestra, *image );
 tecla = waitKey(0);
 save_image(tecla);

  
// Si la rellenas, el último punto lo une con el primero, sin preguntar.
 
// Pueden dibujarse más de una a la vez:
// "Introduction to programming with OpenCV";   Gady Agam;   Department of Computer Science;   January 27, 2006;  Illinois Institute of Technology
// http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html
int  curve1[8]={10,10,  10,100,  100,100,  100,10};
int  curve2[10]={30,30,  30,130,  130,130,  130,30,  150,10};
Point* curveArr[2]={(Point*)curve1, (Point*)curve2};
int      nCurvePts[2]={4,5};
int      nCurves=2;
int      isCurveClosed=1;
int      lineWidth=4;


 polylines(*image, (const Point**)curveArr, nCurvePts, nCurves, isCurveClosed,
            Scalar(0,128,128, 0), lineWidth, line_type );

 sprintf( textDelMensatge,
	 "Pueden dibujarse más de una a la vez." );
 printfXY( image, nomFinestra, textDelMensatge );
 imshow( nomFinestra, *image );
 tecla = waitKey(0);
 save_image(tecla);

 // Deplaçant-ho al 2on quadrant, per vore les versions reomplides
 for (i=0; i<nCurvePts[0]; i++) {
   curve1[2*i] = curve1[2*i] + (3*columna/4);
   // No toca les files curve1[(2*i)+] = curve1[(2*i)+1] + 0; 
 } 
 for (i=0; i<nCurvePts[1]; i++) {
   curve2[2*i] = curve2[2*i] +(3*columna/4);
   // No toca les files! curve2[(2*i)+1] = curve2[(2*i)+1] + (fila/2); 
 }
 fillConvexPoly(*image, (const Point*)curve1, nCurvePts[0], 
                Scalar(128,64, 32, 0), line_type ); //, shift);
 fillConvexPoly(*image, (const Point*)curve2, nCurvePts[1], 
                Scalar(128,64, 32, 0), line_type ); //, shift);
 sprintf( textDelMensatge,
	 "Pueden dibujarse polígonos rellenos ..." );
 printfXY( image, nomFinestra, textDelMensatge );
 imshow( nomFinestra, *image );
 tecla = waitKey(0);
 save_image(tecla);


 // Pintant varies a l'hora (al 3er quadrant)
 for (i=0; i<nCurvePts[0]; i++) {
   curve1[2*i] = curve1[2*i] +(-columna/4);
   curve1[(2*i)+1] = curve1[(2*i)+1] + (3*fila/4); 
 }
 for (i=0; i<nCurvePts[1]; i++) {
   curve2[2*i] = curve2[2*i] +(-columna/4);
   curve2[(2*i)+1] = curve2[(2*i)+1] + (3*fila/4); 
 }
 fillPoly(*image, (const Point**)curveArr, nCurvePts, nCurves, 
          Scalar(0,128,64, 0), line_type, shift, Point(0,0) );
 sprintf( textDelMensatge,
	 "i varios de ellos. Una tecla?"  );
 printfXY( image, nomFinestra, textDelMensatge );
 imshow( nomFinestra, *image );
 tecla = waitKey(0);
 save_image(tecla);

}// Fi de "void Polilinies( ..."



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

  static int columna = 35,
             fila = ALT_FILA;
  
  putText( *image, (const String&)mensatge, 
           Point( columna, fila ),	   
	       tipoLletra, vScale, CV_RGB(255,0,255), lineWidth ); //, lineType);
  imshow(nomFinestra, *image);

  fila += ALT_FILA;
  
}// Fi de "void printfXY( ..."




	
//
// Fi de opencv_graficsText.cpp
// 
