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

#define NUM_LUCES_SEM 4

using namespace std;

/* ============== GLOBAL VARIABLES =============== */

string routeFotoInicio  = "sources/images/Mario_kart.jpg",
       routeFotoCredits = "sources/images/Creditos.jpg";

string ruteSemaphores[4] = {"sources/images/Semaforo_0.png",
                            "sources/images/Semaforo_1.png",
                            "sources/images/Semaforo_2.png",
                            "sources/images/Semaforo_3.png"};

string routeWarning = "sources/images/warningTourtle.png";

string routeSky = "sources/images/Cielo.jpg";

GLuint imgList  = 0,
       mapList,
       kartList;

bool waiting = false;
int t0 = 0;


bool firstTime = true,
     exitFirstTime = false;

bool initGame = false, 
     destroyGame = false,
     startGame = false;

bool loadedImg1 = false,
     loadedImg2 = false;

bool isRunning = false,
     stopRunning = false;

// Para ahorrrar tiempo empezamos en el juego
int ventana          = 0,  
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

void drawMenu(void (*reshape)(int, int), bool *isGame, int &marioWin)
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
        renderGame(reshape, isGame);
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

void enableShowImg(void)
{
    // Guardar matrices
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // DESACTIVAR profundidad
    glDisable(GL_DEPTH_TEST);
}

void disableShowImg(void)
{
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void renderGame(void (*reshape)(int, int), bool *isGame)
{
    if(ventana == 4)
    {
        glEnable(GL_DEPTH_TEST);

        if (!initGame)
        {
            *isGame = true; 
            initGame = true;
            startGame = true;

            loadGame();
            stopMenuSound(1);            
            
            reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        }

        showBackground(routeSky);
        
        Vec3 p = player1.getPos();
        float rad = player1.grados * M_PI / 180.0f;

        // Camara detras del kart
        float camX = p.x - sin(rad) * 5.0f;
        float camY = p.y + cos(rad) * 5.0f;
        float camZ = p.z + 2.5f;

        gluLookAt(
            camX, camY, camZ,
            p.x, p.y, p.z,
            0.0f, 0.0f, 1.0f
        );

        glCallList(mapList);

        comprobateLimits();

        glTranslatef(p.x, p.y, 0.0f);
        glRotatef(player1.grados, 0, 0, 1);
        glCallList(kartList);

        if(!startGame)
        {
            player1.move();
            
            if(isRunning)
            {
                playGameSound(0);
                isRunning = false;
            }
            if(stopRunning)
            {
                stopGameSound(0);
                stopRunning = false;
            }
        }
        else
        {
            enableShowImg();
            for(int i=0; i<NUM_LUCES_SEM; i++)
            {
                showSemaphore(ruteSemaphores[i]);
                
                if(i != 0)
                {
                    if(i == NUM_LUCES_SEM-1) playGameSound(4);
                    else playGameSound(3);
                }

                glutSwapBuffers();
                glutPostRedisplay();

                sleep(1.2);
            }
            disableShowImg();

            startGame = false;
            playGameSound(1);
        }

        if(destroyGame)
        {
            map_render.destroyLights();
            glColor3f(1.0f, 1.0f, 1.0f); // Color Blanco

            *isGame = false; 
            initGame = false; 
            destroyGame = false;

            // Restart lists
            mapList = 0; 
            kartList = 0;

            ventana = 3;
            stopGameSound(1);
            playMenuSound(1);

            reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
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

void comprobateLimits()
{
    Coordinates4f mapSize = map_render.getMapSize();
    Hitbox kartSize = player1.getHitbox();
    Vec3 p = player1.getPos();
    bool isTurning = false;

    if(p.x <= mapSize.x1)
    {
        player1.velocidad = 0;
        isTurning = true;

        p.x += 0.1f;
        player1.editPos(p);
    }
    else if(p.x >= mapSize.x2)
    {
        player1.velocidad = 0;
        isTurning = true;

        p.x -= 0.1f;
        player1.editPos(p);
    }
    else if(p.y <= mapSize.y1) 
    {
        player1.velocidad = 0;
        isTurning = true;

        p.y += 0.1f;
        player1.editPos(p);
    }
    else if(p.y >= mapSize.y2) 
    {
        player1.velocidad = 0;
        isTurning = true;

        p.y -= 0.1f;
        player1.editPos(p);
    }
    
    if(isTurning) player1.grados += 180.0f;
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
        player1.velocidad = max(player1.velocidad + 0.05f, 0.0f);
        if(player1.velocidad > 0.0f) isRunning = true;
    }
}

void keyS(void)
{
    if(ventana == 4)
    {
        player1.velocidad = min(player1.velocidad - 0.05f,  0.0f);
        playGameSound(2);
        if(player1.velocidad == 0.0f) stopRunning = true;
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
    if(ventana == 4) destroyGame = true;
}

void leftClick(float glX, float glY)
{
    id = areaButtonId(glX, glY, ventana);
}
