/*
 OpenCV_OpenGL.cpp
 Versión 2.1 (2020) 
  Simplificant (tornat al nucli original de David, per deixar lloc a ampliacions)
  Adaptant-lo a OpenCV 4
  Sobre la versión de Zhan Z., Alcañiz J. i Anitei D. (2020)
  C++ i OpenCV 3.2
  
 Versión 1.0 (2008) D. Millan
 <http://blog.damiles.com/2008/10/opencv-opengl/>
*/
#include <string>
#include <GL/glut.h>
#include <GL/gl.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;  
using namespace std;


VideoCapture cap;
Mat imagenTextura;
string rute; 

GLuint texture; //the array for our texture
GLfloat angle = 0.0;


//La funció que converteix de Mat a textura d'OpenGl 
int loadTexture_Mat(Mat image, GLuint *texture) {

  if ( image.empty() ) return -1;
   
  glGenTextures(1, texture);

  glBindTexture( GL_TEXTURE_2D, *texture ); //bind the texture to it's array
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, image.data);
    
  return 0;
}

void plane (void) {
  //glBindTexture( GL_TEXTURE_2D, texture ); //bind the texture
  glRotatef( angle, 1.0f, 1.0f, 1.0f );
  glBegin (GL_QUADS);
  glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0); //with our vertices we have to assign a texcoord
  glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0); //so that our texture has some points to draw to
  glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
  glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
  glEnd();
	
}

void load_image()
{
  cap.grab();
  cap.retrieve(imagenTextura, 0 ); 

  flip(imagenTextura, imagenTextura, 0);    

  //Carregat Mat en una textura d'OpenGL
  loadTexture_Mat( imagenTextura, &texture ); 
}

void display (void) {	
  glClearColor (0.0,0.0,0.0,1.0);
  glClear (GL_COLOR_BUFFER_BIT);
  glLoadIdentity();  
  gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glEnable( GL_TEXTURE_2D ); //enable 2D texturing
    
  plane();
  load_image();

  glutSwapBuffers();
  // angle =angle+0.01;  // Fica-li un poc de canya
  angle =(angle+1.0);
}

void FreeTexture( GLuint texture ) {
  glDeleteTextures( 1, &texture );
    
}

void reshape (int w, int h) {
  glViewport (0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
  glMatrixMode (GL_MODELVIEW);
}

int main (int argc, char **argv) 
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize (500, 500);
  glutInitWindowPosition (100, 100);
  glutCreateWindow ("OpenGL + OpenCV");
    
  glutDisplayFunc (display);
  glutIdleFunc (display);
  glutReshapeFunc (reshape);
  
  if(argc == 2)
  {
    rute = argv[1];
    cap.open(rute);
    if(!cap.isOpened())
    {
      printf("Error opening video...\n");
      return -1;
    }
  }
  else
  {
    imagenTextura = imread("code/Img/SMII.png");
  }
  printf("Comenzamos...\n");
    
  glutMainLoop ();
	
  //Free our texture
  FreeTexture( texture );
	
  return 0;
} 
