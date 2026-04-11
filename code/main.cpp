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
       routeKart1       = "sources/assets/Kart_1.obj",
       routeFotoCredits = "sources/images/Creditos.png";

GLuint imgList = 0,
       mapList  = 0,
       kartList = 0;

bool firstTime = true, 
     startGame = false, 
     isGame = false;
bool isMenu[NUM_MENUS];

int ventana = 0,
    id = -1,
    map_selected = 0;

float ancho = 0.8f;

Player player1(0.0f, 0.0f, 0.0f);
Map map_render;

string textOptions[] = {"START", "CREDITS", "EXIT", \
                        "MAP 1", "MAP 2", "MAP 3"};

float coordinatesButtons [3][2] = {{0.0f, 0.5f}, {0.0f, 0.1f}, {0.0f, -0.3f}};

int windowedWidth = 400, windowedHeight = 400;
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
void drawMenu1(void);
void drawMenu2(void);

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
    alutInit(NULL, NULL);
    glEnable(GL_DEPTH_TEST);

    intSoundsMenu();                
    
    // Initalize bool of menus
    for(int i=0; i<NUM_MENUS; i++)
        isMenu[i] = false;
}

void loadGame(void)
{
    kartList = load_obj(routeKart1);
    if (kartList == 0)
    {
        cout << "Fallo al cargar el kart" << endl;
        exit(0);
    }

    map_render.selectMap(map_selected);
    mapList = map_render.load_map();
    if (mapList == 0)
    {
        cout << "Fallo al cargar el mapa" << endl;
        exit(0);
    }

    map_render.setupLights();

    cout << "Juego cargado correctamente\n";
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
        if(!isMenu[0])
        {
            loadImage(routeFotoInicio);
            isMenu[0] = !isMenu[0];
        }
        
        glCallList(drawImage());
        drawText(-0.35f, -0.8f, "Presione cualquier tecla para continuar");
    }
    else if (ventana == 1)
    {
        drawMenu1();
    }
    else if(ventana == 2)
    {
        drawMenu2();
    }
    else if(ventana == 3)
    {
        glEnable(GL_DEPTH_TEST);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if(!startGame)
        {
            createButtonTexture(6, "Back");
            loadGame();

            startGame = !startGame;
            isGame = !isGame;
        }

        Vec3 p = player1.get_pos();

        gluLookAt(
            p.x, p.y - 8.0f, 6.0f,
            p.x, p.y,        0.0f,
            0.0f, 0.0f,      1.0f
        );

        glCallList(mapList);

        glTranslatef(p.x, p.y, 0.0f);
        glRotatef(player1.grados, 0, 0, 1);
        glCallList(kartList);
        player1.move();

        drawButton(0.0f, 0.0f, 0.5f, 6);
    }
    else if(ventana == 5)
    {
        if(!isMenu[3])
        {
            loadImage(routeFotoCredits);
            createButtonTexture(6, "Back");
            isMenu[3] = !isMenu[3];
        }

        glCallList(drawImage());
        drawButton(coordinatesButtons[2][0], coordinatesButtons[2][1], 0.5f, 6);
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

    gluOrtho2D(-aspect, aspect, -1, 1);
    //gluOrtho2D(-1, 1, -1 / aspect, 1 / aspect);
    
    /*if (w >= h)
        gluOrtho2D(-aspect, aspect, -1, 1);
    else
        gluOrtho2D(-1, 1, -1 / aspect, 1 / aspect);*/

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
                else if (ventana == 2)
                {
                    map_selected = 1;
                    id = 3;
                }
                else if(ventana == 5) id = 6;
            break;

        case '2': 
            if(!firstTime)
                if(ventana == 1) id = 1;
                else if (ventana == 2)
                {
                    map_selected = 2;
                    id = 4;
                }
            break;

        case '3':
            if(!firstTime)
                if(ventana == 1) id = 2;
                else if (ventana == 2) 
                {
                    map_selected = 3;
                    id = 5;
                }
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
            if(state == GLUT_UP)
            {
                id = areaButtonId(x, y);  

                // DEBUG
                // cout << "x: " << x << " y: " << y << endl << endl;
                // cout << "ID: " id << endl;}
            }
            break;

        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_UP)
            {
                if(ventana == 2) ventana = 1;
                else if(ventana == 5) ventana = 1;
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
        drawSelectedButton(coordinatesButtons[id][0], coordinatesButtons[id][1], ancho, id);
        ejecutarAccion(id, &ventana);
        id = -1;
    }
}

void drawMenu1(void)
{
    if(!isMenu[1])
    {
        for(int i=0; i<3; i++)
        {
            createButtonTexture(i, textOptions[i]);
        }            
        isMenu[1] = !isMenu[1];
    }

    for(int i=0; i<3; i++)
    {
        drawButton(coordinatesButtons[i][0], coordinatesButtons[i][1], ancho, i);
    }
}

void drawMenu2(void)
{
    if(!isMenu[2])
    {
        for(int i=3; i<6; i++)
        {
            createButtonTexture(i, textOptions[i]);
        }
        isMenu[2] = !isMenu[2];
    }
    
    for(int i=0; i<3; i++)
    {
        drawButton(coordinatesButtons[i][0], coordinatesButtons[i][1], ancho, i+3);
    }
}