#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <string>

#include <GL/glut.h>
#include <opencv2/opencv.hpp>

// Variables globales
struct ButtonArea {
    float x1, x2, y1, y2;
};

int inicializarImgRGB(cv::Mat *imgOrg, int option);
void ponerTextoBoton(cv::Mat &img, std::string texto);
void cargarFondoInicio(std::string ruta);
void crearTexturaBoton(int id, std::string texto);
void drawButton(float x, float y, float ancho, int id);
void drawSelectedButton(float x, float y, float ancho, int id);
void dibujarTexto(float x, float y, std::string texto);
int ejecutarAccion(int id);
int areaButtonId(int x, int y);

#endif