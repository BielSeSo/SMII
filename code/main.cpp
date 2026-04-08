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

Player player1(0.0f, 0.0f, 0.0f);
Map map_render;

/* =================== PROTOTIPOS =================== */

bool init();
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int, int);

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
    glutInitWindowSize(800, 600);
    glutCreateWindow("Mario Kart Interface");
    glutFullScreen();

    if (!init())
        return -1;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

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

        for (int i = 0; i < 5; i++)
            buttonAreas[i] = {0, 0, 0, 0};

        if (ventana == 0)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, fondoTexture);

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
            drawButton(0.0f,  0.5f, ancho, 0);
            drawButton(0.0f,  0.1f, ancho, 1);
            drawButton(0.0f, -0.3f, ancho, 2);
        }
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

    default:
        if (firstTime)
        {
            ventana = 1;
            firstTime = false;
        }
        break;
    }
}