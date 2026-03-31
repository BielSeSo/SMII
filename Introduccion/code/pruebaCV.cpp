#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;

#include "comun.h"
#define fGRIS "¡Hola, OpenCV en gris!"
#define fRGB  "¡Hola, OpenCV en color!"

#define L MAXPIXEL
#define AMPLE (int)L
#define ALT (int)L

int  inicializarImgGris( Mat *imgOrg );
int  inicializarImgRGB( Mat *imgOrg );


int main(int argc, char* argv[]) 
{
  Mat imgOrgGris, imgOrgRGB;
  Scalar colorDst;
  Mat rgbaPlanes[3];

  printf("Versión de OpenCV: %s\n", CV_VERSION);

  printf("Creando una imagen y mostrándola en pantalla [Cualquier tecla - Salir].\n");
  colorDst.val[0] = 128;
  imgOrgGris.create( ALT, AMPLE, CV_8UC1);
  imgOrgGris = colorDst;
  if (!imgOrgGris.data ) 
  {
    fprintf(stderr, "Problemas al crear la imagen en grises\n");
    return( 1 );
  }
  printf("Se ha creado una imagen de %dx%d, de %d plano/s.\n", 
          imgOrgGris.cols, imgOrgGris.rows, imgOrgGris.channels() );
  inicializarImgGris( &imgOrgGris );
  namedWindow( fGRIS, WINDOW_NORMAL);
  imshow(fGRIS, imgOrgGris ); 
  moveWindow( fGRIS, 0, 0 );

  colorDst.val[R] = 255;
  colorDst.val[G] = 255;
  colorDst.val[B] = 255;
  imgOrgRGB.create(imgOrgGris.rows, imgOrgGris.cols, CV_8UC3);
  imgOrgRGB = colorDst;

  if (!imgOrgRGB.data ) 
  {
  fprintf(stderr, "Problemas al crear la imagen en color\n");
  return( 2 );
  }
  printf("Se ha creado una imagen de %dx%d, de %d planos.\n", 
          imgOrgRGB.cols, imgOrgRGB.rows, imgOrgRGB.channels() );
  inicializarImgRGB( &imgOrgRGB );
  namedWindow( fRGB, WINDOW_AUTOSIZE );
  imshow( fRGB, imgOrgRGB ); 
  moveWindow( fRGB, AMPLE+60, 0 ); 

  split(imgOrgRGB, rgbaPlanes);
  printf("Split: ha creado un vector de 3 imágenes de %dx%d, de %d planos.\n", 
          rgbaPlanes[R].cols, rgbaPlanes[R].rows, rgbaPlanes[R].channels() );
  imshow( "R", rgbaPlanes[R] );
  moveWindow( "R", AMPLE+60, ALT+30 ); 
  imshow( "G", rgbaPlanes[G] );
  moveWindow( "G", 2*(AMPLE+60), ALT+30 ); 
  imshow( "B", rgbaPlanes[B] );
  moveWindow( "B", 3*(AMPLE+60), ALT+30 ); 

  waitKey(0);

  destroyAllWindows( );

  imgOrgGris.release(); 
  imgOrgRGB.release(); 

  return 0;
}

int  inicializarImgGris( Mat *imgOrg ) 
{
  int x, y; 

  if ( imgOrg->channels() != 1)
    return( 1 );
  else
  {

    for ( y = 0; y < imgOrg->rows; y++ )
      for ( x = 0; x < imgOrg->cols; x++ )
   {
    imgOrg->at<uchar>(y, x) = x;
   } 
  }

} 

int  inicializarImgRGB( Mat *imgOrg ) 
{
  int x, y; 
  Scalar colorDst;

  if ( imgOrg->channels() != 3) 
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
  }
} 
