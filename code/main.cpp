/*
    Autores:
        - Marco Robert Valverde 
        - Biel Selma Solans
    
    Para compilar "make all", en caso de error:
        1. sudo chmod +x build_all.sh
        2. ./build_all.sh

    Para ejecutar "./bin/proyecto"
*/

#include <iostream>

#include <GL/glut.h>
#include <GL/glu.h>

#include "game_render.h"

using namespace std;

int windowedWidth = 800, windowedHeight = 600;
int windowedPosX = 100, windowedPosY = 100;
int isFullscreen = 0;

int marioWin;

//bool isGame = false;

/* ============= MAIN FUNCS ============*/
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int, int);
void mouse(int button, int state, int x, int y);


int main(int argc, char** argv)
{
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


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    drawMenu(reshape, &isGame, marioWin);

    glutSwapBuffers();
    glutPostRedisplay();
}


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


void keyboard(unsigned char key, int, int)
{
    switch (key)
    {
        case 27:
            closeGame(marioWin);
            break;

        case 'w': 
        case 'W':
            keyW();
            break;

        case 's': 
        case 'S':
            keyS();
            break;

        case 'a': 
        case 'A':
            keyA();
            break;

        case 'd': 
        case 'D':
            keyD();
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
            key1();
            break;

        case '2': 
            key2();
            break;

        case '3':
            key3();
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
                // Tamaño de ventana
                float w = (float)glutGet(GLUT_WINDOW_WIDTH);
                float h = (float)glutGet(GLUT_WINDOW_HEIGHT);

                // Aspect ratio (mismo que usas en reshape)
                float aspect = w / h;

                // Convertir coordenadas del ratón (0..w, 0..h)
                // a coordenadas OpenGL (-aspect..aspect, -1..1)
                float glX = ( (float)x / w ) * (2.0f * aspect) - aspect;
                float glY = 1.0f - ( (float)y / h ) * 2.0f;

                leftClick(glX, glY);
            }
            break;

        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_UP)
            {
                rightClick();
            }
            break;

        default:
            break;
    }
    startWindow();
    selectButton();
}
