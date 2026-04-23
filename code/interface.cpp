#include <iostream>
#include <vector>
#include <string>

#include <GL/glut.h>
#include <opencv2/opencv.hpp>

#include "interface.h"

using namespace std;
using namespace cv;

// Definicion variables
const int total = NUM_BUTONS_INIT + NUM_BUTONS_MAP + \
                  NUM_BUTONS_VEHICLE + 1;

ButtonArea buttonAreas[total];
GLuint buttonTextures[total];
GLuint fondoTexture, fondoSemaphore, fondoWarning, fondo, videoTexture[NUM_BUTONS_MAP];

float alto = 0.25f;

VideoCapture outputVideo[NUM_BUTONS_MAP];
Mat frame;
bool videoReady[NUM_BUTONS_MAP];

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
    float halfWidth = ancho / 2;
    float halfHeight = alto / 2;

    buttonAreas[id] = {x - halfWidth, x + halfWidth, y-halfHeight, y + halfHeight};
   
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, buttonTextures[id]);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(buttonAreas[id].x1, buttonAreas[id].y1);
        glTexCoord2f(1, 0); glVertex2f(buttonAreas[id].x2, buttonAreas[id].y1);
        glTexCoord2f(1, 1); glVertex2f(buttonAreas[id].x2, buttonAreas[id].y2);
        glTexCoord2f(0, 1); glVertex2f(buttonAreas[id].x1, buttonAreas[id].y2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawSelectedButton(int id)
{
    float padding = 0.02f;

    // Si el botón está seleccionado por teclado, dibujamos un recuadro de enfoque
    glColor3f(1.0f, 1.0f, 0.0f); // Amarillo para el "foco"
    glLineWidth(5.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(buttonAreas[id].x1 - padding, buttonAreas[id].y1 - padding);
        glVertex2f(buttonAreas[id].x2 + padding, buttonAreas[id].y1 - padding);
        glVertex2f(buttonAreas[id].x2 + padding, buttonAreas[id].y2 + padding);
        glVertex2f(buttonAreas[id].x1 - padding, buttonAreas[id].y2 + padding);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f); // Resetear a blanco para la textura
}

int areaButtonId(float x, float y, int ventana)
{
    if(ventana == 1)
    {
        for(int i=0; i<3 ; i++)
        {
            if (((x >= buttonAreas[i].x1) && (x <= buttonAreas[i].x2)) && \
            ((y >= buttonAreas[i].y1) && (y <= buttonAreas[i].y2))) return i;
        }  
    }
    else if(ventana == 2)
    {
        for(int i=3; i<6 ; i++)
        {
            if (((x >= buttonAreas[i].x1) && (x <= buttonAreas[i].x2)) && \
            ((y >= buttonAreas[i].y1) && (y <= buttonAreas[i].y2))) return i;
        }  
    }
    else if(ventana == 3)
    {
        for(int i=6; i<9 ; i++)
        {
            if (((x >= buttonAreas[i].x1) && (x <= buttonAreas[i].x2)) && \
            ((y >= buttonAreas[i].y1) && (y <= buttonAreas[i].y2))) return i;
        }  
    }
    if(ventana == 5 || ventana == 4) 
    {
            if (((x >= buttonAreas[total-1].x1) && (x <= buttonAreas[total-1].x2)) && \
            ((y >= buttonAreas[total-1].y1) && (y <= buttonAreas[total-1].y2))) return total-1;
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
        // Button start
        case 0:
            *ventana = 2; // Cambiamos el estado
            break;

        // Button credits
        case 1:
            *ventana = 5;
            break;

        // Button exit
        case 2:
            *ventana = -1;
            break;

        // Choose map
        case 3:
        case 4:
        case 5:
            *ventana = 3;
            break;

        // Choose kart
        case 6:
        case 7:
        case 8:
            *ventana = 4;
            break;

        // Logic of back button
        default: 
            if(*ventana == 2) *ventana = 1;
            if(*ventana == 3) *ventana = 2;
            if(*ventana == 5) *ventana = 1;
            break;
    }
}

void showSemaphore(string ruta)
{
    Mat img = imread(ruta, IMREAD_UNCHANGED);
    if (img.empty()) {
        cerr << "Error: No se pudo cargar la img " << ruta << endl;
    }
    cout << "Cargando img: " << ruta << endl;

    // IMPORTANTE: Para que no salga al revés y tenga colores correctos
    cvtColor(img, img, COLOR_BGRA2RGBA);
    flip(img, img, 0); 

    glGenTextures(1, &fondoSemaphore);
    glBindTexture(GL_TEXTURE_2D, fondoSemaphore);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.cols, img.rows, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    float x = -1.25f;
    float width = 0.5f;
    float height = 1.0f;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fondoSemaphore);
    glColor3f(1.0f, 1.0f, 1.0f); // Blanco
    
    for(int i=1; i<4; i++)
    {
        float padding = width * i;
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(x + padding, 0.0f);
            glTexCoord2f(1, 0); glVertex2f(x + width + padding, 0.0f);
            glTexCoord2f(1, 1); glVertex2f(x + width + padding, height);
            glTexCoord2f(0, 1); glVertex2f(x + padding, height);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void showWarning(string ruta)
{
    Mat img = imread(ruta, IMREAD_UNCHANGED);
    if (img.empty()) {
        cerr << "Error: No se pudo cargar la img " << ruta << endl;
    }

    // IMPORTANTE: Para que no salga al revés y tenga colores correctos
    cvtColor(img, img, COLOR_BGRA2RGBA);
    flip(img, img, 0); 

    glGenTextures(1, &fondoWarning);
    glBindTexture(GL_TEXTURE_2D, fondoWarning);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.cols, img.rows, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fondoWarning);
    glColor3f(1.0f, 1.0f, 1.0f); // Blanco
    
    float x = 0.5f;
    float y = 1.0f; 

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(-x, -0.3f);
        glTexCoord2f(1, 0); glVertex2f(x, -0.3f);
        glTexCoord2f(1, 1); glVertex2f(x, y);
        glTexCoord2f(0, 1); glVertex2f(-x, y);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void showBackground(string ruta)
{
    Mat img = imread(ruta, IMREAD_UNCHANGED);
    if (img.empty()) {
        cerr << "Error: No se pudo cargar la img " << ruta << endl;
    }

    // IMPORTANTE: Para que no salga al revés y tenga colores correctos
    cvtColor(img, img, COLOR_BGRA2RGB);
    flip(img, img, 0); 

    glGenTextures(1, &fondo);
    glBindTexture(GL_TEXTURE_2D, fondo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 
                 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fondo);
    glColor3f(1.0f, 1.0f, 1.0f); // Blanco
    
    float radio = 500.0f;

    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricNormals(quad, GLU_NONE);

    glBindTexture(GL_TEXTURE_2D, fondo);
    gluSphere(quad, radio, 40, 40);

    gluDeleteQuadric(quad);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void initAnimation(int id, string ruta)
{   
    outputVideo[id].open(ruta);
    if (!outputVideo[id].isOpened())
    {
        cerr << "Error abriendo video" << endl;
        return;
    }

    outputVideo[id].read(frame);

    cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    flip(frame, frame, 0);

    glGenTextures(1, &videoTexture[id]);
    glBindTexture(GL_TEXTURE_2D, videoTexture[id]);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,
        frame.cols, frame.rows,
        0, GL_RGB, GL_UNSIGNED_BYTE,
        frame.data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    videoReady[id] = true;
}

void showAnimationButton(int id) 
{    
    if(!videoReady[id]) return;

    if (!outputVideo[id].read(frame))
    {
        outputVideo[id].set(cv::CAP_PROP_POS_FRAMES, 0); // loop
        outputVideo[id].read(frame);
    }
   
    cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    flip(frame, frame, 0);

    glBindTexture(GL_TEXTURE_2D, videoTexture[id]);
    glTexSubImage2D(
        GL_TEXTURE_2D, 0,
        0, 0,
        frame.cols, frame.rows,
        GL_RGB, GL_UNSIGNED_BYTE,
        frame.data
    );

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, videoTexture[id]);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(buttonAreas[id].x1, buttonAreas[id].y1);
        glTexCoord2f(1, 0); glVertex2f(buttonAreas[id].x2, buttonAreas[id].y1);
        glTexCoord2f(1, 1); glVertex2f(buttonAreas[id].x2, buttonAreas[id].y2);
        glTexCoord2f(0, 1); glVertex2f(buttonAreas[id].x1, buttonAreas[id].y2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
