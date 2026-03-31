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

#include "game/load_obj.hpp"
#include "game/load_minimap.hpp"
#include "game/player.hpp"
#include "interface.hpp"

using namespace std;

int radio = 6;

Player player1(0, 0 ,0);
Loader loader_kart;
Map map_loader;

void display() {
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
    
    if (argv[1] == "1")
    {
        // Inicializar texturas antes de entrar al loop
        crearTexturaBoton(0, "START");
        crearTexturaBoton(1, "EXIT");
        crearTexturaBoton(2, "CREDITS");
        crearTexturaBoton(3, "map_loader1");
        crearTexturaBoton(4, "map_loader2");
        crearTexturaBoton(5, "map_loader3");
        crearTexturaBoton(6, "Back");
        cargarFondoInicio("../Images/SMII.png");
    }
    else if (argv[1] == "2")
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        map_loader.setupLights();
    }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    std::cout << "Interfaz cargada. Presiona ESC para salir." << std::endl;
    glutMainLoop();
    return 0;
}
