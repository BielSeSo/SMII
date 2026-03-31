#ifndef INTERFACE_H
#define INTERFACE_H

#include <GL/glut.h>
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;

int inicializarImgRGB(Mat *imgOrg, int option);
void ponerTextoBoton(Mat &img, std::string texto);
void cargarFondoInicio(const std::string& ruta);
void crearTexturaBoton(int id, std::string texto);
void drawButton(float x, float y, float ancho, int id);
void dibujarTexto(float x, float y, std::string texto);
void ejecutarAccion(int id);

#endif