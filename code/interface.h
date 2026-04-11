#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <string>

#include <GL/glut.h>
#include <opencv2/opencv.hpp>

#define NUM_BUTONS_INIT 3
#define NUM_BUTONS_MAP 3
#define NUM_BUTONS_VEHICLE 3

// Variables globales
struct ButtonArea {
    float x1, x2, y1, y2;
};

int inicializarImgRGB(cv::Mat *imgOrg, int option);
void putTextButton(cv::Mat &img, std::string texto);
void loadImage(std::string ruta);
void createButtonTexture(int id, std::string text);
GLuint drawImage();
void drawButton(float x, float y, float ancho, int id);
void drawSelectedButton(float x, float y, float ancho, int id);
int areaButtonId(int x, int y);
void drawText(float x, float y, std::string texto);
void ejecutarAccion(int id, int *ventana);

#endif