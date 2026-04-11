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
       routeFotoCredits = "sources/images/Creditos.png";

GLuint imgList  = 0,
       mapList  = 0,
       kartList = 0;

bool firstTime = true, 
     startGame = false, 
     isGame    = false;

bool loadedImg1 = false,
     loadedImg2 = false;

int ventana          = 0,
    id               = -1,
    map_selected     = 0,
    vehicle_selected = 0;

float ancho = 0.8f;

Player player1(0.0f, 0.0f, 0.0f);
Map map_render;

const int total = NUM_BUTONS_INIT + NUM_BUTONS_MAP + NUM_BUTONS_VEHICLE;
string textOptions[total] = {"START", "CREDITS", "EXIT", \
                        "MAP 1", "MAP 2", "MAP 3", \
                        "Vehicle 1", "Vehicle 2", "Vehicle 3"};

float coordinatesButtons [3][2] = {{0.0f, 0.5f}, {0.0f, 0.1f}, {0.0f, -0.3f}};

int windowedWidth = 800, windowedHeight = 600;
int windowedPosX = 100, windowedPosY = 100;
int isFullscreen = 0;

/* =================== PROTOTIPOS =================== */

void init(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int, int);
void mouse(int button, int state, int x, int y);
void loadGame(void);
void startWindow(void);
void selectButton(void);

/* =================== MAIN =================== */

int main(int argc, char** argv)
{
    //isGame = true;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 300);
    glutCreateWindow("Mario Kart Interface");

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
    //alutInit(NULL, NULL);
    //intSoundsMenu();   
    
    // Initialize all buttons
    for(int i=0; i<total; i++)
    {
        createButtonTexture(i, textOptions[i]);
    }
    createButtonTexture(10, "Back");
}

void loadGame(void)
{
    player1.selectKart(vehicle_selected);
    kartList = player1.loadVehicle();
    if (kartList == 0)
    {
        cout << "Fallo al cargar el kart" << endl;
    }

    map_render.selectMap(map_selected);
    mapList = map_render.loadMap();
    if (mapList == 0)
    {
        cout << "Fallo al cargar el mapa" << endl;
    }

    map_render.setupLights();
    cout << "Juego cargado correctamente" << endl;
}

/* =================== DISPLAY =================== */

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    if(ventana == -1) exit(0);
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
        startGame = false;
        isGame = false;
    }
    else if(ventana == 4)
    {
        glEnable(GL_DEPTH_TEST);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if(!startGame)
        {
            //loadGame();
            startGame = true;
            isGame = true;
            cout << "Juego inicializado" << endl;
        }
       
        glCallList(mapList);

        Vec3 p = player1.getPos();
        float rad = player1.grados * M_PI / 180.0f;

        // Camara detras del kart
        float camX = p.x - sin(rad) * 10.0f;
        float camY = p.y - cos(rad) * 10.0f;
        float camZ = p.z + 5.0f;

        gluLookAt(
            camX, camY, camZ,
            p.x, p.y, p.z,
            0.0f, 0.0f, 1.0f
        );

        glTranslatef(p.x, p.y, 0.0f);
        glRotatef(player1.grados, 0, 0, 1);
        glCallList(kartList);
        player1.move();
    }
    else if(ventana == 5)
    {
        if(!loadedImg2)
        {
            loadImage(routeFotoCredits);
            loadedImg2 = true;
        }
        glCallList(drawImage());
        drawButton(coordinatesButtons[2][0], coordinatesButtons[2][1], 0.5f, 10);
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
    glMatrixMode(GL_MODELVIEW);
}

/* =================== KEYBOARD =================== */

void keyboard(unsigned char key, int, int)
{
    switch (key)
    {
        case 27:
            exit(0);
            break;

        case 'w': case 'W':
            if(isGame)
            {
                cout << "Broom" << endl;
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
                else if(ventana == 2) id = 3, map_selected = 1;
                else if(ventana == 3) id = 6, vehicle_selected = 1;
                else if(ventana == 5) id = 10;
            break;

        case '2': 
            if(!firstTime)
                if(ventana == 1) id = 1;
                else if(ventana == 2) id = 4, map_selected = 2;
                else if(ventana == 3) id = 7, vehicle_selected = 2;
            break;

        case '3':
            if(!firstTime)
                if(ventana == 1) id = 2;
                else if(ventana == 2) id = 5, map_selected = 3;
                else if(ventana == 3) id = 8, vehicle_selected = 3; 
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
            if(state == GLUT_DOWN)
            {
                id = areaButtonId(x, y);  
                if(ventana == 2) map_selected = id-3;
                else if(ventana == 3) vehicle_selected = id-6;

                // DEBUG
                // cout << "x: " << x << " y: " << y << endl << endl;
                // cout << "ID: " id << endl;}
            }
            break;

        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_UP)
            {
                id = 10;
            }
            break;

        default:
            break;
    }
    startWindow();
    selectButton();
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
        if(id % 2 == 0)
        {
            drawSelectedButton(coordinatesButtons[1][0], coordinatesButtons[1][1], ancho, id);
        }
        else if(id % 3 == 0)
        {
            drawSelectedButton(coordinatesButtons[2][0], coordinatesButtons[2][1], ancho, id);
        }
        else
        {
            drawSelectedButton(coordinatesButtons[0][0], coordinatesButtons[0][1], ancho, id);
        }
        ejecutarAccion(id, &ventana);
        id = -1;
    }
}
