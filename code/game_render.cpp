#include <iostream>
#include <cmath>
#include <string>
#include <unistd.h>

#include <GL/glut.h>
#include <GL/glu.h>

#include "game_render.h"
#include "game/player.h"
#include "game/load_map.h"
#include "interface.h"
#include "sound_maker.h"

using namespace std;

/* ============== GLOBAL VARIABLES =============== */

string routeFotoInicio  = "sources/images/Mario_kart.jpg",
       routeFotoCredits = "sources/images/Creditos.jpg";

GLuint imgList  = 0,
       mapList,
       kartList;

bool firstTime = true,
     exitFirstTime = false;

bool startGame = false, 
     exitGame = false;

bool loadedImg1 = false,
     loadedImg2 = false;

int ventana          = 0,  // Para ahorrrar tiempo empezamos en el juego
    id               = -1,
    map_selected     = 0,
    vehicle_selected = 0;

float ancho = 0.8f;

Player player1(0.0f, 0.0f, 0.0f);
Map map_render;

const int total = NUM_BUTONS_INIT + NUM_BUTONS_MAP + NUM_BUTONS_VEHICLE + 1;
string textOptions[total] = {"START", "CREDITS", "EXIT",
                        "MAP 1", "MAP 2", "MAP 3",
                        "Vehicle 1", "Vehicle 2", "Vehicle 3",
                        "Back"};

float coordinatesButtons [3][2] = {{0.0f, 0.5f}, 
                                   {0.0f, 0.1f}, 
                                   {0.0f, -0.3f}};

// ================== CUSTOM FUNCS ====================/
void init(void)
{   
    // Initialize sounds
    alutInit(NULL, NULL);
    intSoundsMenu();   
    initSoundsGame();
    
    // Initialize all buttons
    for(int i=0; i<total; i++)
    {
        createButtonTexture(i, textOptions[i]);
    }
}

void startWindow(void)
{
    if (firstTime)
    {
        ventana = 1;
        firstTime = false;
    }
}

void drawMenu(bool *needReshape, bool *isGame, int &marioWin)
{
    if(ventana == -1) closeGame(marioWin);
    else if (ventana == 0)
    {
        if(!loadedImg1)
        {
            loadImage(routeFotoInicio);
            loadedImg1 = true;
            playMenuSound(0);
        }
        glCallList(drawImage());
        drawText(-0.35f, -0.8f, "Presione cualquier tecla para continuar");
    }
    else if (ventana == 1)
    {
        if(!exitFirstTime)
        {
            stopMenuSound(0);
            exitFirstTime = true;
            playMenuSound(1);
        }

        for(int i=0; i<3; i++)
        {
            drawButton(coordinatesButtons[i][0], coordinatesButtons[i][1], ancho, i);
        }        
    }
    else if(ventana == 2)
    {
        for(int i=0; i<3; i++)
        {
            drawButton(coordinatesButtons[i][0], coordinatesButtons[i][1], ancho, i+3);
        }
    }
    else if(ventana == 3)
    {
        for(int i=0; i<3; i++)
        {
            drawButton(coordinatesButtons[i][0], coordinatesButtons[i][1], ancho, i+6);
        }
    }
    else if(ventana == 4)
    {
        renderGame(needReshape, isGame);
    }
    else if(ventana == 5)
    {
        if(!loadedImg2)
        {
            loadImage(routeFotoCredits);
            loadedImg2 = true;
        }
        glCallList(drawImage());
        drawButton(0.0f, -0.7f, 0.5f, total-1);
    }
}

void renderGame(bool *needReshape, bool *isGame)
{
    if(ventana == 4)
    {
        glEnable(GL_DEPTH_TEST);

        if (!startGame)
        {
            *isGame = true; startGame = true;
            loadGame();
            stopMenuSound(1);
            
            *needReshape = true;
        }
       
        Vec3 p = player1.getPos();
        float rad = player1.grados * M_PI / 180.0f;

        // Camara detras del kart
        float camX = p.x - sin(rad) * 10.0f;
        float camY = p.y + cos(rad) * 10.0f;
        float camZ = p.z + 5.0f;

        gluLookAt(
            camX, camY, camZ,
            p.x, p.y, p.z,
            0.0f, 0.0f, 1.0f
        );

        glCallList(mapList);

        glTranslatef(p.x, p.y, 0.0f);
        glRotatef(player1.grados, 0, 0, 1);
        glCallList(kartList);
        player1.move();

        if(exitGame)
        {
            map_render.destroyLights();
            glColor3f(1.0f, 1.0f, 1.0f); // Color Blanco

            *isGame = false; startGame = false; exitGame = false;

            // Restart lists
            mapList = 0; kartList = 0;

            ventana = 3;
            playMenuSound(1);

            *needReshape = true;
        }
    }
}

void loadGame(void)
{
    // Load kart
    player1.selectKart(vehicle_selected);
    kartList = player1.loadVehicle();

    // Load map
    map_render.selectMap(map_selected);
    mapList = map_render.loadMap();

    // Prepare lights
    map_render.setupLights();
}

void selectButton(void)
{
    if(!firstTime && id != -1)
    {   
        if(ventana == 1 && id != 10)
        {
            drawSelectedButton(coordinatesButtons[id][0], coordinatesButtons[id][1], ancho, id);
        }
        if(ventana == 2 && id != 10)
        {
            map_selected = id-2;
            drawSelectedButton(coordinatesButtons[id-3][0], coordinatesButtons[id-3][1], ancho, id);
        }
        if(ventana == 3 && id != 10)
        {
            vehicle_selected = id-5;
            drawSelectedButton(coordinatesButtons[id-6][0], coordinatesButtons[id-6][1], ancho, id);
        }

        if(id != 10) playMenuSound(2);
        else playMenuSound(3);

        // Dibuja por un segundo
        glutSwapBuffers();
        glutPostRedisplay();
        usleep(100000); // 100 ms

        ejecutarAccion(id, &ventana);
        id = -1;
    }
}

void closeGame(int &marioWin)
{
    // Destroy 3D models
    imgList  = 0;
    mapList = 0;
    kartList = 0;

    map_render.destroyLights();

    // Close sound
    destroySoundsMenu();
    destroySoundsGame();
    alutExit();

    glutDestroyWindow(marioWin);

    cout << "Saliendo del juego..." << endl;
    exit(0);  
}

// KEYBOARD FUNCS
void keyW(void)
{
    if(ventana == 4)
    {
        player1.velocidad = max(player1.velocidad - 0.05f, -1.5f);
    }
}

void keyS(void)
{
    if(ventana == 4)
    {
        player1.velocidad = min(player1.velocidad + 0.05f,  1.5f);
    }
}

void keyA(void)
{
    if(ventana == 4)
    {
        player1.grados += 5.0f;
    }
}

void keyD(void)
{
    if(ventana == 4)
    {
        player1.grados -= 5.0f;
    } 
}

void key1(void)
{
    if(!firstTime)
        if(ventana == 1) id = 0;
        else if(ventana == 2) id = 3;
        else if(ventana == 3) id = 6;
        else if(ventana == 5) id = 10;
}

void key2(void)
{
    if(!firstTime)
        if(ventana == 1) id = 1;
        else if(ventana == 2) id = 4;
        else if(ventana == 3) id = 7;
}

void key3(void)
{
    if(!firstTime)
        if(ventana == 1) id = 2;
        else if(ventana == 2) id = 5;
        else if(ventana == 3) id = 8; 
}

// MOUSE FUNCS
void rightClick(void)
{
    id = 10;
    if(ventana == 4) exitGame = true;
}

void leftClick(float glX, float glY)
{
    id = areaButtonId(glX, glY, ventana);
}