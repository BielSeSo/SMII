// Compilar: g++ viewer_assimp_glut.cpp -o viewer -lGL -lGLU -lglut -lassimp
// Ejecutar: ./viewer ruta/al/modelo.obj
//
// Características:
//  - Carga .obj con Assimp (triangula y genera normales).
//  - Pretransforma vértices (ignora jerarquía -> más simple de dibujar).
//  - Calcula bounding box para centrar y escalar a la vista.
//  - Rotación automática para ver el modelo.
//  - Teclas: [+] y [-] zoom, [ESC] salir.

#include <GL/glut.h>
#include <GL/glu.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>
#include <cmath>

#include "game/load_obj.h"
#include "game/load_minimap.h"
#include "game/player.h"
#include "interface.h"

using namespace std;

// Variables globales
bool isGame = false;
String op1 = "1", op2 = "2";
int radio = 6;

Player player1(0, 0 ,0);
Loader loader_kart;
Map map_loader;

void display() {
    if(isGame){
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

        glutSwapBuffers();
    }
    else
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
            std::cout << "Ventana 1" << std::endl;
        } else if (ventana == 2){
            float anchoBoton = 0.8f; 
            // Mostrar los distintos mapas
            drawButton(0.0f,  0.5f, anchoBoton, 3); // MAPS SELECTOR
            drawButton(0.0f,  0.1f, anchoBoton, 4); // MAPS SELECTOR(?)
            std::cout << "Ventana 2" << std::endl;
        } else if (ventana == 3){
        //TODO
        std::cout << "Ventana 3" << std::endl;
        } else if (ventana == 4){
        //TODO
        std::cout << "Ventana 4" << std::endl;
        }

        glutSwapBuffers();
    }   
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

void idle() {
    player1.move();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: // ESC
            printf("Hasta la próxima");
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

        default: break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Mario Kart Interface");
    glutFullScreen();

    if (argc == 2)
    {
        if (argv[1] == op1)
        {
            // Inicializar texturas antes de entrar al loop
            crearTexturaBoton(0, "START");
            crearTexturaBoton(1, "EXIT");
            crearTexturaBoton(2, "CREDITS");
            crearTexturaBoton(3, "map_loader1");
            crearTexturaBoton(4, "map_loader2");
            crearTexturaBoton(5, "map_loader3");
            crearTexturaBoton(6, "Back");
            cargarFondoInicio("code/Images/SMII.png");
        }
        else if (argv[1] == op2)
        {
            isGame = true;
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            map_loader.setupLights();
        }
    }
    else
    {
        cout << "Formato ejecucion: ./bin/proyecto <Tipo ejecución>" << endl;
        return -1;
    }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    std::cout << "Interfaz cargada. Presiona ESC para salir." << std::endl;
    glutMainLoop();
    return 0;
}
