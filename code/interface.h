#ifndef INTERFACE_HPP
#define INTERFACE_HPP

    #include <GL/glut.h>
    #include <opencv2/opencv.hpp>
    #include <string>

    using namespace cv;

    // Variables globales
    struct ButtonArea {
        float x1, x2, y1, y2;
    };

    extern ButtonArea buttonAreas[5]; // Para START, EXIT, CREDITS
    extern GLuint buttonTextures[5];
    extern GLuint fondoTexture;

    extern int botonSeleccionado; // 0: START, 1: EXIT, 2: CREDITS
    extern bool usandoTeclado; // Para saber si mostrar el resaltado
    extern int ventana;

    int inicializarImgRGB(Mat *imgOrg, int option);
    void ponerTextoBoton(Mat &img, std::string texto);
    void cargarFondoInicio(const std::string& ruta);
    void crearTexturaBoton(int id, std::string texto);
    void drawButton(float x, float y, float ancho, int id);
    void dibujarTexto(float x, float y, std::string texto);
    void ejecutarAccion(int id);

#endif