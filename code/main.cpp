// Compilar: g++ viewer_assimp_glut.cpp -o viewer -lGL -lGLU -lglut -lassimp
// Ejecutar: ./viewer ruta/al/modelo.obj
//
// Características:
//  - Carga .obj con Assimp (triangula y genera normales).
//  - Pretransforma vértices (ignora jerarquía -> más simple de dibujar).
//  - Calcula bounding box para centrar y escalar a la vista.
//  - Rotación automática para ver el modelo.
//  - Teclas: [+] y [-] zoom, [ESC] salir.
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

#include <GL/glut.h>
#include <GL/glu.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "game/load_obj.h"
#include "game/load_minimap.h"
#include "game/player.h"
#include "interface.h"

string ruteFotoInicio = "images/SMII.png";

using namespace std;

// Variables globales
bool isGame = false, firstTime = true;
int radio = 6;

Player player1(0, 0 ,0);
Loader loader_kart;
Map map_loader;

// ================ FUNCTIONS PROTOTYPE ===================
void init(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int, int);


int main(int argc, char **argv) {
    if(argc == 2)
    {
        if(string(argv[1]) == "2") isGame = true;
    }
    else
    {
        cout << "Forma de uso: ./bin/proyecto <Tipo de inicio> " << endl;
        cout << "1 - Forma HUD" << endl << "2 - Juego carrera" << endl;
        return -1;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Mario Kart Interface");
    glutFullScreen();

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    cout << "Presiona ESC para salir" << endl;
    glutMainLoop();
    return 0;
}

void init(void)
{
    if(!isGame)
    {
        // Inicializar texturas antes de entrar al loop
        crearTexturaBoton(0, "START");
        crearTexturaBoton(1, "EXIT");
        crearTexturaBoton(2, "CREDITS");
        crearTexturaBoton(3, "map_loader1");
        crearTexturaBoton(4, "map_loader2");
        crearTexturaBoton(5, "map_loader3");
        crearTexturaBoton(6, "Back");
        cargarFondoInicio(ruteFotoInicio);

        cout << "Interfaz cargada" << endl;
    }
    else
    {
        cout << "Mapa cargado" << endl;
        map_loader.setupLights();
    }
}

void display(void) {
    if (!isGame)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        // Resetear áreas para evitar clics fantasma de ventanas anteriores
        for(int i=0; i<5; i++) buttonAreas[i] = {0.0f, 0.0f, 0.0f, 0.0f};

        if (ventana == 0){
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, fondoTexture);
            glBegin(GL_QUADS);

            glTexCoord2f(0, 0); glVertex2f(-2.0f, -1.0f);
            glTexCoord2f(1, 0); glVertex2f( 2.0f, -1.0f);
            glTexCoord2f(1, 1); glVertex2f( 2.0f,  1.0f);
            glTexCoord2f(0, 1); glVertex2f(-2.0f,  1.0f);
            glEnd();
            glDisable(GL_TEXTURE_2D);

            dibujarTexto(-0.35f, -0.8f, "PRESIONE CUALQUIER TECLA PARA CONTINUAR");
        } else if (ventana == 1){
            float anchoBoton = 0.8f; 
            drawButton(0.0f,  0.5f, anchoBoton, 0); // START
            drawButton(0.0f,  0.1f, anchoBoton, 1); // EXIT
            drawButton(0.0f, -0.3f, anchoBoton, 2); // CREDITS
            //cout << "Ventana 1" << endl;
        } else if (ventana == 2){
            float anchoBoton = 0.8f; 
            // Mostrar los distintos mapas
            drawButton(0.0f,  0.5f, anchoBoton, 3); // MAPS SELECTOR
            drawButton(0.0f,  0.1f, anchoBoton, 4); // MAPS SELECTOR(?)
            //cout << "Ventana 2" << endl;
        } else if (ventana == 3){
            //TODO
            //cout << "Ventana 3" << endl;
        } else if (ventana == 4){
            //TODO
            //cout << "Ventana 4" << endl;
        }
    }
    else
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        gluLookAt(player1.get_pos().x, player1.get_pos().y + 6, 8,   player1.get_pos().x , player1.get_pos().y, 0.5,   0.0, 0.0, 1.0);
        
        map_loader.selectMap(1);
        map_loader.load_map();

        // Rotación y translacion del kart
        glTranslatef(player1.get_pos().x, player1.get_pos().y, 0);
        glRotatef(player1.grados, 0.0, 0.0, 1.0);
        loader_kart.load_model("assets/Kart_1.obj");    // Dibujar
        player1.move();
    }
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w / (float)h;
    if (w >= h) gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    else gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: // ESC
            cout << "Hasta la proxima" << endl;
            exit(0);
            break;

        case 'w':
        case 'W':
            player1.velocidad -= 0.1;
            break;

        case 's':
        case 'S':
            player1.velocidad += 0.1;
            break;

        case 'a':
        case 'A':
            player1.grados += 5;
            break;

        case 'd':
        case 'D':
            player1.grados -= 5;
            break;

        default:
            if(firstTime)
            {
               ventana = 1; 
            }
            break;
    }

	if (ventana == 1){
            botonSeleccionado = (botonSeleccionado + 1) % 3; // Ciclar entre 0, 1, 2
            // Forzar redibujado para ver el cambio
	}
	else if (ventana == 2)
    {
	    if (botonSeleccionado < 3 || botonSeleccionado >= 4) botonSeleccionado = 3;
	    else botonSeleccionado = 4;
	}
    
    glutPostRedisplay();
}

