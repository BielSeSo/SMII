#include <GL/glut.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cv;

// --- ESTRUCTURAS Y GLOBALES ---
struct ButtonArea {
    float x1, x2, y1, y2;
};

ButtonArea buttonAreas[5]; // Para START, EXIT, CREDITS
GLuint buttonTextures[5];
GLuint fondoTexture;

int botonSeleccionado = -1; // 0: START, 1: EXIT, 2: CREDITS
bool usandoTeclado = false; // Para saber si mostrar el resaltado

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

void ponerTextoBoton(Mat &img, std::string texto) {
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
void cargarFondoInicio(const std::string& ruta) {
    cv::Mat imagen = cv::imread(ruta);
    if (imagen.empty()) {
        std::cerr << "Error: No se pudo cargar la imagen " << ruta << std::endl;
        return;
    }
    // IMPORTANTE: Para que no salga al revés y tenga colores correctos
    cv::cvtColor(imagen, imagen, cv::COLOR_BGR2RGB);
    cv::flip(imagen, imagen, 0); 

    glGenTextures(1, &fondoTexture);
    glBindTexture(GL_TEXTURE_2D, fondoTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imagen.cols, imagen.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, imagen.data);
}

void crearTexturaBoton(int id, std::string texto) {
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

void dibujarTexto(float x, float y, std::string texto) {
    // Desactivar texturas para que el texto sea de un color sólido
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f); // Color Blanco

    // Posicionar el "cursor" de dibujo
    glRasterPos2f(x, y);

    for (char c : texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}
// --- EVENTOS Y LÓGICA ---

void display() {
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

void ejecutarAccion(int id) {
    switch(id){
	case 0:
	    std::cout << "Cambiando a ventana de juego..." << std::endl;
            ventana = 2; // Cambiamos el estado
	    break;

	case 1:
            std::cout << "Saliendo del juego..." << std::endl;
            exit(0);
	    break;

	case 2:
            std::cout << "DESARROLLADO POR: TU NOMBRE" << std::endl;
	    break;

	case 3:
	    std::cout << "MAPA 1 SELECCIONADO" << std::endl;
	    exit(0);
	    break;

	case 4:
	    std::cout << "MAPA 2 SELECCIONADO" << std::endl;
	    exit(0);
	    break;
    }
    
    glutPostRedisplay(); // ¡CRUCIAL! Indica a GLUT que debe volver a dibujar la pantalla
}

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

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w / (float)h;
    if (w >= h) gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    else gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Mario Kart Interface");
    glutFullScreen();
    
    // Inicializar texturas antes de entrar al loop
    crearTexturaBoton(0, "START");
    crearTexturaBoton(1, "EXIT");
    crearTexturaBoton(2, "CREDITS");
    crearTexturaBoton(3, "MAP1");
    crearTexturaBoton(4, "MAP2");
    crearTexturaBoton(5, "MAP3");
    crearTexturaBoton(6, "Back");
    cargarFondoInicio("../Images/SMII.png");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    std::cout << "Interfaz cargada. Presiona ESC para salir." << std::endl;
    glutMainLoop();
    return 0;
}
