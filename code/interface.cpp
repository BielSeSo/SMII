#include <iostream>
#include <vector>
#include <string>

#include <GL/glut.h>
#include <opencv2/opencv.hpp>

#include "interface.h"

using namespace std;
using namespace cv;

// Definicion variables
ButtonArea buttonAreas[5];
GLuint buttonTextures[7];
GLuint fondoTexture;

int botonSeleccionado = -1;
bool usandoTeclado = false;
int ventana = 0;


// --- FUNCIONES DE OPENCV (Lógica de Imagen) ---

int inicializarImgRGB(Mat *imgOrg, int option) {
    int x, y;
    const int TAM_BLOQUE = 20;
    if (imgOrg->channels() != 3) return 1;

    for (y = 0; y < imgOrg->rows; y++) {
        for (x = 0; x < imgOrg->cols; x++) {
            int grosorBrillo = (imgOrg->rows * 0.20 > 10) ? imgOrg->rows * 0.20 : 10;
            if (y < grosorBrillo) {
                imgOrg->at<Vec3b>(y, x) = Vec3b(230, 255, 240); // Brillo
            } else {
                if (((x / TAM_BLOQUE) + (y / TAM_BLOQUE)) % 2 == 0)
                    imgOrg->at<Vec3b>(y, x) = Vec3b(15, 145, 20);  // Verde Oscuro
                else
                    imgOrg->at<Vec3b>(y, x) = Vec3b(39, 210, 57);  // Verde Claro
            }
        }
    }
    rectangle(*imgOrg, Point(0,0), Point(imgOrg->cols-1, imgOrg->rows-1), Scalar(0,0,0), 2);
    return 0;
}

void ponerTextoBoton(Mat &img, string texto) {
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 1.2;
    int thickness = 3;
    int baseline = 0;
    Size textSize = getTextSize(texto, fontFace, fontScale, thickness, &baseline);
    Point textOrg((img.cols - textSize.width) / 2, (img.rows + textSize.height) / 2);
    
    putText(img, texto, textOrg + Point(2, 2), fontFace, fontScale, Scalar(0, 0, 0), thickness); // Sombra
    putText(img, texto, textOrg, fontFace, fontScale, Scalar(255, 255, 255), thickness);        // Texto
}

// --- FUNCIONES DE OPENGL (Dibujado y Texturas) ---
//La funció que converteix de Mat a textura d'OpenGl
void cargarFondoInicio(string ruta) {
    Mat imagen = imread(ruta);
    if (imagen.empty()) {
        cerr << "Error: No se pudo cargar la imagen " << ruta << endl;
        return;
    }
    cout << "Cargando imagen: " << ruta << endl;

    // IMPORTANTE: Para que no salga al revés y tenga colores correctos
    cvtColor(imagen, imagen, COLOR_BGR2RGB);
    flip(imagen, imagen, 0); 

    glGenTextures(1, &fondoTexture);
    glBindTexture(GL_TEXTURE_2D, fondoTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imagen.cols, imagen.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, imagen.data);
}

void crearTexturaBoton(int id, string texto) {
    Mat img(200, 600, CV_8UC3);
    inicializarImgRGB(&img, 0);
    ponerTextoBoton(img, texto);

    // Convertir BGR (OpenCV) a RGB (OpenGL)
    cvtColor(img, img, COLOR_BGR2RGB);
    flip(img, img, 0); // Voltear para que no salga al revés en OpenGL

    glGenTextures(1, &buttonTextures[id]);
    glBindTexture(GL_TEXTURE_2D, buttonTextures[id]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void drawButton(float x, float y, float ancho, int id) {
    float alto = 0.25f;
    buttonAreas[id] = {x - ancho/2, x + ancho/2, y - alto/2, y + alto/2};

    // Si el botón está seleccionado por teclado, dibujamos un recuadro de enfoque
    if (usandoTeclado && botonSeleccionado == id) {
        glColor3f(1.0f, 1.0f, 0.0f); // Amarillo para el "foco"
        glLineWidth(5.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(x - ancho/2 - 0.02f, y - alto/2 - 0.02f);
            glVertex2f(x + ancho/2 + 0.02f, y - alto/2 - 0.02f);
            glVertex2f(x + ancho/2 + 0.02f, y + alto/2 + 0.02f);
            glVertex2f(x - ancho/2 - 0.02f, y + alto/2 + 0.02f);
        glEnd();
        glColor3f(1.0f, 1.0f, 1.0f); // Resetear a blanco para la textura
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, buttonTextures[id]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(x - ancho/2, y - alto/2);
        glTexCoord2f(1, 0); glVertex2f(x + ancho/2, y - alto/2);
        glTexCoord2f(1, 1); glVertex2f(x + ancho/2, y + alto/2);
        glTexCoord2f(0, 1); glVertex2f(x - ancho/2, y + alto/2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void dibujarTexto(float x, float y, string texto) {
    // Desactivar texturas para que el texto sea de un color sólido
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f); // Color Blanco

    // Posicionar el "cursor" de dibujo
    glRasterPos2f(x, y);

    for (char c : texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void ejecutarAccion(int id) {
    switch(id){
	case 0:
	    cout << "Cambiando a ventana de juego..." << endl;
            ventana = 2; // Cambiamos el estado
	    break;

	case 1:
            cout << "Saliendo del juego..." << endl;
            exit(0);
	    break;

	case 2:
            cout << "DESARROLLADO POR: TU NOMBRE" << endl;
	    break;

	case 3:
	    cout << "MAPA 1 SELECCIONADO" << endl;
	    exit(0);
	    break;

	case 4:
	    cout << "MAPA 2 SELECCIONADO" << endl;
	    exit(0);
	    break;
    }
}


   
/*
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC

    if (ventana == 0) {
	ventana = 1; // Saltar al menú principal
	glutPostRedisplay();
        return;
    }

    if (key == 9) { // Tecla TAB
        usandoTeclado = true;

	if (ventana == 1){
            botonSeleccionado = (botonSeleccionado + 1) % 3; // Ciclar entre 0, 1, 2
            glutPostRedisplay(); // Forzar redibujado para ver el cambio
	}
	else if (ventana == 2){
	    if (botonSeleccionado < 3 || botonSeleccionado >= 4) botonSeleccionado = 3;
	    else botonSeleccionado = 4;
            glutPostRedisplay(); // Forzar redibujado para ver el cambio
	}
    }

    if (key == 13) { // Tecla ENTER
        if (usandoTeclado) {
            ejecutarAccion(botonSeleccionado);
        }
    }
}
*/