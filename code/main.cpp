#include <iostream>
#include <cmath>
#include <string>
#include <unistd.h>

#include <GL/glut.h>
#include <GL/glu.h>

#include "game/load_obj.h"
#include "game/load_map.h"
#include "game/player.h"
#include "interface.h"
#include "sound_maker.h"

using namespace std;

/* =================== VARIABLES GLOBALES =================== */

string routeFotoInicio  = "sources/images/Mario_kart.jpg",
       routeFotoCredits = "sources/images/Creditos.jpg";

GLuint imgList  = 0,
       mapList,
       kartList;

bool firstTime = true, 
     startGame = false, 
     isGame    = false,
     exitGame = false;

bool loadedImg1 = false,
     loadedImg2 = false;

int ventana          = 0,
    id               = -1,
    map_selected     = 0,
    vehicle_selected = 0;

float ancho = 0.8f;

Player player1(0.0f, 0.0f, 0.0f);
Map map_render;

const int total = NUM_BUTONS_INIT + NUM_BUTONS_MAP + NUM_BUTONS_VEHICLE + 1;
string textOptions[total] = {"START", "CREDITS", "EXIT", \
                        "MAP 1", "MAP 2", "MAP 3", \
                        "Vehicle 1", "Vehicle 2", "Vehicle 3", \
                        "Back"};

float coordinatesButtons [3][2] = {{0.0f, 0.5f}, {0.0f, 0.1f}, {0.0f, -0.3f}};

int windowedWidth = 800, windowedHeight = 600;
int windowedPosX = 100, windowedPosY = 100;
int isFullscreen = 0;

int marioWin;

/* =================== PROTOTIPOS =================== */

void init(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int, int);
void mouse(int button, int state, int x, int y);
void loadGame(void);
void startWindow(void);
void selectButton(void);
void closeGame(void);

/* =================== MAIN =================== */

int main(int argc, char** argv)
{
    //isGame = true;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 300);
    marioWin = glutCreateWindow("Mario Kart Interface");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    cout << "Presiona ESC para salir\n";
    glutMainLoop();

    return 0;
}

/* =================== INIT =================== */

void init(void)
{   
    // Initialize sounds
    alutInit(NULL, NULL);
    intSoundsMenu();   
    initSoundsGame();
    
    // Initialize all buttons
    for(int i=0; i<total; i++)
    {
        createButtonTexture(i, textOptions[i]);
    }
}

void loadGame(void)
{
    // Change view
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

    // Load kart
    player1.selectKart(vehicle_selected);
    kartList = player1.loadVehicle();

    // Load map
    map_render.selectMap(map_selected);
    mapList = map_render.loadMap();

    // Prepare lights
    map_render.setupLights();
}

/* =================== DISPLAY =================== */

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    if(ventana == -1) closeGame();
    else if (ventana == 0)
    {
        if(!loadedImg1)
        {
            loadImage(routeFotoInicio);
            loadedImg1 = true;
        }
        glCallList(drawImage());
        drawText(-0.35f, -0.8f, "Presione cualquier tecla para continuar");
    }
    else if (ventana == 1)
    {
        for(int i=0; i<3; i++)
        {
            drawButton(coordinatesButtons[i][0], coordinatesButtons[i][1], ancho, i);
        }
    }
    else if(ventana == 2)
    {
        for(int i=0; i<3; i++)
        {
            drawButton(coordinatesButtons[i][0], coordinatesButtons[i][1], ancho, i+3);
        }
    }
    else if(ventana == 3)
    {
        for(int i=0; i<3; i++)
        {
            drawButton(coordinatesButtons[i][0], coordinatesButtons[i][1], ancho, i+6);
        }
    }
    else if(ventana == 4)
    {
        glEnable(GL_DEPTH_TEST);

        if (!startGame)
        {
            isGame = true; startGame = true;
            loadGame();
        }
       
        Vec3 p = player1.getPos();
        float rad = player1.grados * M_PI / 180.0f;

        // Camara detras del kart
        float camX = p.x - sin(rad) * 10.0f;
        float camY = p.y + cos(rad) * 10.0f;
        float camZ = p.z + 5.0f;

        gluLookAt(
            camX, camY, camZ,
            p.x, p.y, p.z,
            0.0f, 0.0f, 1.0f
        );

        glCallList(mapList);

        glTranslatef(p.x, p.y, 0.0f);
        glRotatef(player1.grados, 0, 0, 1);
        glCallList(kartList);
        player1.move();

        if(exitGame)
        {
            map_render.destroyLights();
            glColor3f(1.0f, 1.0f, 1.0f); // Color Blanco

            startGame = false; isGame = false; exitGame = false;

            // Restart lists
            mapList = 0; kartList = 0;

            ventana = 3;
            reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        }
    }
    else if(ventana == 5)
    {
        if(!loadedImg2)
        {
            loadImage(routeFotoCredits);
            loadedImg2 = true;
        }
        glCallList(drawImage());
        drawButton(0.0f, -0.7f, 0.5f, total-1);
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

/* =================== RESHAPE =================== */

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)w / (float)h;

    if (!isGame)
    {
        gluOrtho2D(-aspect, aspect, -1, 1);
    }
    else
    {
        gluPerspective(60.0, aspect, 0.1, 500.0);
    }

    glMatrixMode(GL_MODELVIEW);
}

/* =================== KEYBOARD =================== */

void keyboard(unsigned char key, int, int)
{
    switch (key)
    {
        case 27:
            closeGame();
            break;

        case 'w': case 'W':
            if(isGame)
            {
                player1.velocidad = max(player1.velocidad - 0.05f, -1.5f);
            }
            break;

        case 's': case 'S':
            if(isGame)
            {
                player1.velocidad = min(player1.velocidad + 0.05f,  1.5f);
            }
            break;

        case 'a': case 'A':
            if(isGame)
            {
                player1.grados += 5.0f;
            }
            break;

        case 'd': case 'D':
            if(isGame)
            {
                player1.grados -= 5.0f;
            }
            break;

        // --- Pantalla completa ---
        case 'f':
        case 'F':
            if (!isFullscreen) {
                windowedWidth  = glutGet(GLUT_WINDOW_WIDTH);
                windowedHeight = glutGet(GLUT_WINDOW_HEIGHT);
                windowedPosX   = glutGet(GLUT_WINDOW_X);
                windowedPosY   = glutGet(GLUT_WINDOW_Y);
                glutFullScreen();
                isFullscreen = 1;
            } else {
                glutReshapeWindow(windowedWidth, windowedHeight);
                glutPositionWindow(windowedPosX, windowedPosY);
                isFullscreen = 0;
            }
            break;

        // First options
        case '1':
            if(!firstTime)
                if(ventana == 1) id = 0;
                else if(ventana == 2) id = 3;
                else if(ventana == 3) id = 6;
                else if(ventana == 5) id = 10;
            break;

        case '2': 
            if(!firstTime)
                if(ventana == 1) id = 1;
                else if(ventana == 2) id = 4;
                else if(ventana == 3) id = 7;
            break;

        case '3':
            if(!firstTime)
                if(ventana == 1) id = 2;
                else if(ventana == 2) id = 5;
                else if(ventana == 3) id = 8; 
            break;

        default: break;
    }
    startWindow();
    selectButton();
}


void mouse(int button, int state, int x, int y) 
{
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN && !firstTime)
            {   
                // Tamaño de ventana
                float w = (float)glutGet(GLUT_WINDOW_WIDTH);
                float h = (float)glutGet(GLUT_WINDOW_HEIGHT);

                // Aspect ratio (mismo que usas en reshape)
                float aspect = w / h;

                // Convertir coordenadas del ratón (0..w, 0..h)
                // a coordenadas OpenGL (-aspect..aspect, -1..1)
                float glX = ( (float)x / w ) * (2.0f * aspect) - aspect;
                float glY = 1.0f - ( (float)y / h ) * 2.0f;

                id = areaButtonId(glX, glY, ventana);  
            }
            break;

        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_UP)
            {
                id = 10;
                if(ventana == 4) exitGame = true;
            }
            break;

        default:
            break;
    }
    startWindow();
    selectButton();
}

void closeGame(void)
{
    // Destroy 3D models
    imgList  = 0;
    mapList = 0;
    kartList = 0;

    map_render.destroyLights();

    // Close sound
    destroySoundsMenu();
    destroySoundsGame();
    alutExit();

    glutDestroyWindow(marioWin);

    cout << "Saliendo del juego..." << endl;
    exit(0);  
}

void startWindow(void)
{
    if (firstTime)
    {
        ventana = 1;
        firstTime = false;
    }
}

void selectButton(void)
{
    if(!firstTime && id != -1)
    {   
        if(ventana == 1)
        {
            drawSelectedButton(coordinatesButtons[id][0], coordinatesButtons[id][1], ancho, id);
        }
        if(ventana == 2)
        {
            map_selected = id-2;
            drawSelectedButton(coordinatesButtons[id-3][0], coordinatesButtons[id-3][1], ancho, id);
        }
        if(ventana == 3)
        {
            vehicle_selected = id-5;
            drawSelectedButton(coordinatesButtons[id-6][0], coordinatesButtons[id-6][1], ancho, id);
        }
        ejecutarAccion(id, &ventana);
        id = -1;
    }
}
