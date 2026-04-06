#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <string>


#include <GL/glut.h>
#include <AL/alut.h>

using namespace std;

// DEBUG TERMINAL
void DebugTerminal(void) {int value = system("stty sane"); (void)value;}

// Function prototypes 
void init(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void comprobateSound(void);
void comprobatePlayerPosition(void);
void updatePositionAmbulance(void);

// Class
class Player{
    public:
       ALfloat position[3] = {0.0, 0.0, 0.0};
       ALfloat velocity[3] = {0.0, 0.0, 0.0};
       ALfloat listenerOrientation[6] = {0.0, 0.0, 1.0, 0.0, 1.0, 0.0};
};
Player player;

// Global variables
#define NUM_SOURCES 4
#define NUM_ENVIRONMENTS 1

ALfloat sourcePos[NUM_SOURCES][3];
ALfloat sourceVel[NUM_SOURCES][3]=
                    {{ 0.0, 0.0, 0.0},
                    { 0.0, 0.0, 0.0},
                    { 0.0, 0.0, 0.0},
                    { 0.0, 0.0, 0.0}};

ALuint	buffer[NUM_SOURCES];
ALuint	source[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];
int 	GLwin;

string directoryMother = "code/audios/actividad_1/";
string  sourceRute[NUM_SOURCES] = {
    "BasketBall.wav", 
    "BasketBall.wav",  
    "Public.wav",
    "Ambulance.wav"};

 
bool isSound[NUM_SOURCES];
ALsizei size,freq;
ALenum 	format;
ALvoid 	*data;
int 	ch;

float deltaVel = 0.001;
float dopplerFactor = 1.0, dopplerVelocity = 343.0;
float velocityObject = 0.0;

bool isFullscreen = false;
int windowedWidth = 400, windowedHeight = 400;
int windowedPosX = 100, windowedPosY = 100;



int main(int argc, char **argv)
{
  glutInit(&argc, argv) ;
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH) ;   
  glutInitWindowSize(400,400) ;
  GLwin = glutCreateWindow("PIGE - OpenAL Example") ;

  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKeys);
  glutReshapeFunc(reshape);

  glutMainLoop();

  printf("Usuario saliendo\n");

  alDeleteBuffers(NUM_SOURCES, buffer);
  alutExit();

  DebugTerminal();
  return 0;
}


void init(void) 
{
  alutInit(0, NULL);

  alListenerfv(AL_POSITION,player.position);
  alListenerfv(AL_VELOCITY,player.velocity);
  alListenerfv(AL_ORIENTATION,player.listenerOrientation);
    
  alGetError(); // clear any error messages
    
  if(alGetError() != AL_NO_ERROR) 
  {
    printf("- Error creating buffers !!\n");
    exit(1);
  }
  else
  {
    printf("init() - No errors yet.\n");
  }
    
  alGenBuffers(NUM_SOURCES, buffer);

  for (int i = 0; i < NUM_SOURCES; i++)
  {
    sourceRute[i] = directoryMother + sourceRute[i];
    printf("Carregant fitxer \"%s\"\n", sourceRute[i].c_str());
    buffer[i] = alutCreateBufferFromFile(sourceRute[i].c_str());
  }

  alGetError(); /* clear error */
  alGenSources(NUM_SOURCES, source);

  if(alGetError() != AL_NO_ERROR) 
  {
    printf("- Error creating sources !!\n");
    exit(2);
  }
  else
  {
    printf("init - no errors after alGenSources\n");
  }

  for (int i = 0; i < NUM_SOURCES; i++)
  {
    alSourcef(source[i],AL_PITCH,1.0f);
    alSourcef(source[i],AL_GAIN,1.0f);
    alSourcefv(source[i],AL_POSITION,sourcePos[i]);
    alSourcefv(source[i],AL_VELOCITY,sourceVel[i]);
    alSourcei(source[i],AL_BUFFER,buffer[i]);
    alSourcei(source[i],AL_LOOPING,AL_TRUE);
  }
  
  alDopplerFactor(dopplerFactor);   // Aumenta o reduce la intensidad del efecto
  alDopplerVelocity(343.0);         // Velocidad del sonido en el aire

  srand(time(nullptr));
  for(int i=0; i<NUM_SOURCES-1; i++)
  {
    sourcePos[i][0] = -3.5f + static_cast<float>(rand()) / RAND_MAX * 7.0f;   // X entre -3.5 y 3.5
    sourcePos[i][1] = 0.0f;                                                   // Y fijo
    sourcePos[i][2] = -5.0f + static_cast<float>(rand()) / RAND_MAX * -15.0f; // Z entre -5.0 y -20.0
  }

  sourcePos[3][0] = 0.0;
  sourcePos[3][1] = 0.0;
  sourcePos[3][2] = -4.0;

  for(int i=0; i<NUM_SOURCES; i++)
  {
    isSound[i] = false;
  } 
}


// ===================================================================
// void display()
// ===================================================================
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;
  glPushMatrix() ;
  glRotatef(20.0,1.0,1.0,0.0);

  for(int i = 0; i < NUM_SOURCES; i++)
  {
    glPushMatrix();
    glTranslatef(sourcePos[i][0],sourcePos[i][1],sourcePos[i][2]);

    switch (i)
    {
      case 0: glColor3f(1.0,0.0,0.0); break;
      case 1: glColor3f(0.0,1.0,0.0); break;
      case 2: glColor3f(0.0,0.0,1.0); break;
      case 3: glColor3f(1.0,0.0,1.0); break;
      
      default: break;
    }

    glutWireCube(0.5);
    glPopMatrix();

    /*printf("Source %d pos = (%.2f, %.2f, %.2f)\n", i,
      sourcePos[i][0], sourcePos[i][1], sourcePos[i][2]);*/
  }
  updatePositionAmbulance();

  // Listener
  comprobatePlayerPosition();
  glPushMatrix();
  glTranslatef(player.position[0],player.position[1],player.position[2]);
  glColor3f(1.0,1.0,1.0);
  glutWireCube(0.5);
  glPopMatrix();

  glPopMatrix();
  glutSwapBuffers();

  glutPostRedisplay();
}

void updatePositionAmbulance(void)
{
  sourcePos[3][0] += sourceVel[3][0];
  if (sourcePos[3][0] > 3.5f) {
    sourcePos[3][0] = 3.5f;
    sourceVel[3][0] = -sourceVel[3][0];
  } else if (sourcePos[3][0] < -3.5f) {
    sourcePos[3][0] = -3.5f;
    sourceVel[3][0] = -sourceVel[3][0];
  }
  alSourcefv(source[3],AL_POSITION,sourcePos[3]);
}


void comprobatePlayerPosition(void)
{
  player.position[2] += player.velocity[2];
  if (player.position[2] >= 4.0f) {
    player.position[2] = 4.0f;
    player.velocity[2] = 0.0f;
  } else if (player.position[2] <= -19.5f) {
    player.position[2] = -19.5f;
    player.velocity[2] = 0.0f;
  }

  player.position[0] += player.velocity[0];
  if (player.position[0] > 3.5f) {
    player.position[0] = 3.5f;
    player.velocity[0] = 0.0f;
  } else if (player.position[0] < -3.5f) {
    player.position[0] = -3.5f;
    player.velocity[0] = 0.0f;
  }

  //printf("Listener position: (%.2f, %.2f, %.2f)\n", player.position[0], player.position[1], player.position[2]);
}

// ===================================================================
// void reshape(int w, int h)
// ===================================================================
void reshape(int w, int h) // the reshape function
{
  glViewport(0,0,(GLsizei)w,(GLsizei)h) ;
  glMatrixMode(GL_PROJECTION) ;
  glLoadIdentity() ;
  gluPerspective(60.0,(GLfloat)w/(GLfloat)h,1.0,30.0) ;
  glMatrixMode(GL_MODELVIEW) ;
  glLoadIdentity() ;
  glTranslatef(0.0,0.0,-6.6) ;
}


// ===================================================================
// void keyboard(int key, int x, int y)
// ===================================================================
void keyboard(unsigned char key, int x, int y) 
{
  switch(key)
    {
    case 'h':
    case 'H': // Menu: opciones
      printf(" 1 Plays/Stop BasketBall_1\n 2 Plays/Stop BasketBall_2\n 3 Plays/Stop Ambulance\n");
      printf("La posición del oyente se cambia con las teclas de cursor o con 'asqz'\n");
      break;

    case '1': isSound[0] = !isSound[0]; comprobateSound(); break;

    case '2': isSound[1] = !isSound[1]; comprobateSound(); break;

    case '3': isSound[2] = !isSound[2]; comprobateSound(); break;

    case '4': isSound[3] = !isSound[3]; comprobateSound(); break;
    
    // Teclas de navegación modificadas para mayor intuicion del usuario
    case 'a':
    case 'A':
      player.velocity[0] -= deltaVel;
      alListenerfv(AL_POSITION,player.position);
      break ;

    case 's':
    case 'S':
      player.velocity[0] += deltaVel;
      alListenerfv(AL_POSITION,player.position);
      break ;

    case 'q':
    case 'Q':
      player.velocity[2] -= deltaVel;
      alListenerfv(AL_POSITION,player.position);
      break ;

    case 'z':
    case 'Z':
      player.velocity[2] += deltaVel;
      alListenerfv(AL_POSITION,player.position);
      break ;

    // Teclas funcion Doppler
    case 'd':
      dopplerFactor -= 0.1f;
      dopplerFactor = max(dopplerFactor, 0.1f); 
      alDopplerFactor(dopplerFactor);   

    case 'D':
      dopplerFactor += 0.1f;
      alDopplerFactor(dopplerFactor);   
      break;

    case 'e':
      dopplerVelocity -= 10.0f;
      dopplerVelocity = max(dopplerVelocity, 10.0f);
      alDopplerVelocity(dopplerVelocity);
      break;

    case 'E':
      dopplerVelocity += 10.0f;
      alDopplerVelocity(dopplerVelocity);
      break;

    // Simulacón velocidad
    case 'v':
      sourceVel[3][0] -= deltaVel;
      alSourcefv(source[3],AL_VELOCITY,sourceVel[3]);
      break;

    case 'V':
      sourceVel[3][0] += deltaVel;
      alSourcefv(source[3],AL_VELOCITY,sourceVel[3]);
      break;

    // Ventana completa
    case 'f':
    case 'F':
      if (!isFullscreen) {
        // Guardar tamaño y posición actuales
        windowedWidth  = glutGet(GLUT_WINDOW_WIDTH);
        windowedHeight = glutGet(GLUT_WINDOW_HEIGHT);
        windowedPosX   = glutGet(GLUT_WINDOW_X);
        windowedPosY   = glutGet(GLUT_WINDOW_Y);

        glutFullScreen();   // Activar fullscreen
        isFullscreen = true;
      } else {
        // Volver al modo ventana
        glutReshapeWindow(windowedWidth, windowedHeight);
        glutPositionWindow(windowedPosX, windowedPosY);

        isFullscreen = false;
      }
      break;

    // Salir del programa
    case 27:
      for (int i = 0; i < NUM_SOURCES; i++)
      {
        alSourceStop(source[i]);
      }

      alutExit();
      glutDestroyWindow(GLwin) ;
      exit(0) ;
      break ;

    default: break;
  }    
  glutPostRedisplay();
}


void comprobateSound(void)
{
  if(isSound[0]) alSourcePlay(source[0]), printf("Playing: BasketBall.wav\n");
  else if(!isSound[0]) alSourceStop(source[0]), printf("Stopped: BasketBall.wav\n");

  if(isSound[1])  alSourcePlay(source[1]), printf("Playing: BasketBall.wav\n");
  else if(!isSound[1]) alSourceStop(source[1]), printf("Stopped: BasketBall.wav\n");

  if (isSound[2]) alSourcePlay(source[2]), printf("Playing: Ambulance.wav\n");
  else if(!isSound[2]) alSourceStop(source[2]), printf("Stopped: Ambulance.wav\n");

  if (isSound[3]) alSourcePlay(source[3]), printf("Playing: Public.wav\n");
  else if(!isSound[3]) alSourceStop(source[3]), printf("Stopped: Public.wav\n");

  printf("----------------------------------------\n");
}


// ===================================================================
// void specialKeys(int key, int x, int y)
// =================================================================== 
void specialKeys(int key, int x, int y)
{
  switch(key)
    {
    case GLUT_KEY_RIGHT:
      player.velocity[0] += deltaVel;
      alListenerfv(AL_POSITION,player.position);
      glutPostRedisplay() ;
      break;
    case GLUT_KEY_LEFT:
      player.velocity[0] -= deltaVel;
      alListenerfv(AL_POSITION,player.position);
      glutPostRedisplay() ;
      break;
    case GLUT_KEY_UP:
      player.velocity[2] -= deltaVel;
      alListenerfv(AL_POSITION,player.position);
      glutPostRedisplay() ;
      break;
    case GLUT_KEY_DOWN:
      player.velocity[2] += deltaVel;
      alListenerfv(AL_POSITION,player.position);
      glutPostRedisplay() ;
      break;
    }
}
