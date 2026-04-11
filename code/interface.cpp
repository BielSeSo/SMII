#include <iostream>
#include <vector>
#include <string>

#include <GL/glut.h>
#include <opencv2/opencv.hpp>

#include "interface.h"

using namespace std;
using namespace cv;

// Definicion variables
const int total = NUM_BUTONS_INIT + NUM_BUTONS_MAP;

ButtonArea buttonAreas[5];
GLuint buttonTextures[total];
GLuint fondoTexture;

float alto = 0.25f;


// --- FUNCIONES DE OPENCV (Lógica de img) ---
int inicializarImgRGB(Mat *imgOrg, int option) 
{
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

void putTextButton(Mat &img, string texto) 
{
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
void loadImage(string ruta) 
{
    Mat img = imread(ruta);
    if (img.empty()) {
        cerr << "Error: No se pudo cargar la img " << ruta << endl;
    }
    cout << "Cargando img: " << ruta << endl;

    // IMPORTANTE: Para que no salga al revés y tenga colores correctos
    cvtColor(img, img, COLOR_BGR2RGB);
    flip(img, img, 0); 

    glGenTextures(1, &fondoTexture);
    glBindTexture(GL_TEXTURE_2D, fondoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void createButtonTexture(int id, string text) 
{
    Mat img(200, 600, CV_8UC3);
    inicializarImgRGB(&img, 0);
    putTextButton(img, text);

    // Convertir BGR (OpenCV) a RGB (OpenGL)
    cvtColor(img, img, COLOR_BGR2RGB);
    flip(img, img, 0); // Voltear para que no salga al revés en OpenGL

    glGenTextures(1, &buttonTextures[id]);
    glBindTexture(GL_TEXTURE_2D, buttonTextures[id]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

GLuint drawImage()
{
    GLuint imageList = glGenLists(1);
    glNewList(imageList, GL_COMPILE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fondoTexture);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-2, -1);
    glTexCoord2f(1, 0); glVertex2f( 2, -1);
    glTexCoord2f(1, 1); glVertex2f( 2,  1);
    glTexCoord2f(0, 1); glVertex2f(-2,  1);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glEndList();

    return imageList;
}

void drawButton(float x, float y, float ancho, int id) 
{
    buttonAreas[id] = {x - ancho/2, x + ancho/2, y - alto/2, y + alto/2};
   
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

void drawSelectedButton(float x, float y, float ancho, int id)
{
    // Si el botón está seleccionado por teclado, dibujamos un recuadro de enfoque
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

int areaButtonId(int x, int y)
{
    for(int id=0; id<3 ; id++)
    {
        // Comprobar que medidas cogen los botones
        // cout << "x1: " << buttonAreas[id].x1 << " x2: " << buttonAreas[id].x2 << endl;
        // cout << "y1: " << buttonAreas[id].y1 << " y2: " << buttonAreas[id].y2 << endl;

        if (((x > buttonAreas[id].x1) && (x < buttonAreas[id].x1)) && \
        ((y > buttonAreas[id].y1) && (y < buttonAreas[id].y2))) return id;
    }  
    return -1; 
}

void drawText(float x, float y, string texto)
{
    // Desactivar texturas para que el texto sea de un color sólido
    glColor3f(1.0f, 1.0f, 1.0f); // Color Blanco

    // Posicionar el "cursor" de dibujo
    glRasterPos2f(x, y);

    for (char c : texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void ejecutarAccion(int id, int *ventana) 
{
    switch(id)
    {
        case 0:
            *ventana = 2; // Cambiamos el estado
            break;

        case 1:
            *ventana = 5;
            break;

        case 2:
            cout << "Saliendo del juego..." << endl;
            *ventana = -1;
            break;

        case 3:
        case 4:
        case 5:
            *ventana = 3;
            break;

        case 6:
        case 7:
        case 8:
            *ventana = 4;
            break;

        // Logic of back button
        default: 
            if(*ventana == 2) *ventana = 1;
            if(*ventana == 3) *ventana = 2;
            if(*ventana == 4) *ventana = 3;
            if(*ventana == 5) *ventana = 1;
            break;
    }
}