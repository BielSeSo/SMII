#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <string>

#include <GL/glut.h>
#include <opencv2/opencv.hpp>

// Variables globales
struct ButtonArea {
    float x1, x2, y1, y2;
};

extern ButtonArea buttonAreas[5]; // Para START, EXIT, CREDITS
extern GLuint buttonTextures[7];
extern GLuint fondoTexture;

extern int botonSeleccionado; // 0: START, 1: EXIT, 2: CREDITS
extern bool usandoTeclado; // Para saber si mostrar el resaltado
extern int ventana;

int inicializarImgRGB(cv::Mat *imgOrg, int option);
void ponerTextoBoton(cv::Mat &img, std::string texto);
void cargarFondoInicio(std::string ruta);
void crearTexturaBoton(int id, std::string texto);
void drawButton(float x, float y, float ancho, int id);
void dibujarTexto(float x, float y, std::string texto);
void ejecutarAccion(int id);

#endif