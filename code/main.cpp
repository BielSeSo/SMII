#include <iostream>
#include <cmath>
#include <string>

#include <GL/glut.h>
#include <GL/glu.h>

#include "game/load_obj.h"
#include "game/load_map.h"
#include "game/player.h"
#include "interface.h"

using namespace std;

/* =================== VARIABLES GLOBALES =================== */

string routeFotoInicio = "images/Mario_kart.jpg";
string routeKart1      = "assets/Kart_1.obj";

GLuint mapList  = 0;
GLuint kartList = 0;

bool isGame   = false;
bool firstTime = true;

int ventana = 0;
int id = -1;

Player player1(0.0f, 0.0f, 0.0f);
Map map_render;

float coordinatesButtons1 [3][2] = {{0.0f, 0.5f}, {0.0f, 0.1f}, {0.0f, -0.3f}};

int windowedWidth = 400, windowedHeight = 400;
int windowedPosX = 100, windowedPosY = 100;
int isFullscreen = 0;

/* =================== PROTOTIPOS =================== */

bool init();
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int, int);
void mouse(int button, int state, int x, int y);

/* =================== MAIN =================== */

int main(int argc, char** argv)
{
    if (argc == 2)
    {
        if (string(argv[1]) == "2")
            isGame = true;
    }
    else
    {
        cout << "Forma de uso: ./bin/proyecto <Tipo de inicio>\n";
        cout << "1 - Forma HUD\n2 - Juego carrera\n";
        return -1;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 300);
    glutCreateWindow("Mario Kart Interface");

    if (!init())
        return -1;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    cout << "Presiona ESC para salir\n";
    glutMainLoop();
    return 0;
}

/* =================== INIT =================== */

bool init()
{
    glEnable(GL_DEPTH_TEST);

    if (!isGame)
    {
        crearTexturaBoton(0, "START");
        crearTexturaBoton(1, "EXIT");
        crearTexturaBoton(2, "CREDITS");
        crearTexturaBoton(3, "map_loader1");
        crearTexturaBoton(4, "map_loader2");
        crearTexturaBoton(5, "map_loader3");
        crearTexturaBoton(6, "Back");

        cargarFondoInicio(routeFotoInicio);
        cout << "Interfaz cargada\n";
    }
    else
    {
        kartList = load_obj(routeKart1);
        if (kartList == 0)
            return false;

        map_render.selectMap(1);
        mapList = map_render.load_map();
        if (mapList == 0)
            return false;

        map_render.setupLights();

        cout << "Juego cargado correctamente\n";
    }

    return true;
}

/* =================== DISPLAY =================== */

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (!isGame)
    {
        glDisable(GL_DEPTH_TEST);

        if (ventana == 0)
        {
            glEnable(GL_TEXTURE_2D);

            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(-2, -1);
            glTexCoord2f(1, 0); glVertex2f( 2, -1);
            glTexCoord2f(1, 1); glVertex2f( 2,  1);
            glTexCoord2f(0, 1); glVertex2f(-2,  1);
            glEnd();

            glDisable(GL_TEXTURE_2D);
            dibujarTexto(-0.35f, -0.8f, "PRESIONE CUALQUIER TECLA PARA CONTINUAR");
        }
        else if (ventana == 1)
        {
            float ancho = 0.8f;

            for(int i=0; i<3; i++)
                drawButton(coordinatesButtons1[i][0], coordinatesButtons1[i][1], ancho, i);
            if(id != -1)
            {
                drawSelectedButton(coordinatesButtons1[id][0], coordinatesButtons1[id][1], ancho, id);
            }
        }
        else if(ventana == -1) exit(0);
    }
    else
    {
        glEnable(GL_DEPTH_TEST);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

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
        if (w >= h)
            gluOrtho2D(-aspect, aspect, -1, 1);
        else
            gluOrtho2D(-1, 1, -1 / aspect, 1 / aspect);
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
            exit(0);
            break;

        case 'w': case 'W':
            player1.velocidad = max(player1.velocidad - 0.05f, -1.5f);
            break;

        case 's': case 'S':
            player1.velocidad = min(player1.velocidad + 0.05f,  1.5f);
            break;

        case 'a': case 'A':
            player1.grados += 5.0f;
            break;

        case 'd': case 'D':
            player1.grados -= 5.0f;
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
            ventana = ejecutarAccion(0);
            break;

        case '2':
            ventana = ejecutarAccion(1);
            break;

        case '3':
            ventana = ejecutarAccion(2);
            break;

        default:
            if (firstTime)
            {
                ventana = 1;
                firstTime = false;
            }
            break;
    }
}


void mouse(int button, int state, int x, int y) 
{
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if (firstTime)
            {
                ventana = 1;
                firstTime = false;
            }
            else
            {   
                id = areaButtonId(x, y);

                // DEBUG
                // cout << "x: " << x << " y: " << y << endl << endl;
                // cout << "ID: " id << endl;

                if(id != -1)
                {
                    ventana = ejecutarAccion(id);
                    id = -1;
                }
            }
            break;

        case GLUT_RIGHT_BUTTON:
         if (firstTime)
            {
                ventana = 1;
                firstTime = false;
            }
            break;

        default:
            break;
    }

   
}
