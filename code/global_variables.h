#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <string>

#include "game/player.h"
#include "game/load_map.h"
#include "interface.h"
#include "sound_maker.h"

/* ============== GLOBAL VARIABLES =============== */

std::string routeFotoInicio  = "sources/images/Mario_kart.jpg",
            routeFotoCredits = "sources/images/Creditos.jpg";

GLuint imgList  = 0,
       mapList,
       kartList;

bool firstTime = true,
     exitFirstTime = false;

bool startGame = false, 
     isGame    = false,
     exitGame = false;

bool loadedImg1 = false,
     loadedImg2 = false;

int ventana          = 0,
    id               = -1,
    map_selected     = 0,
    vehicle_selected = 0;

float ancho = 0.8f;

Player player1(0.0f, 0.0f, 0.0f);
Map map_render;

const int total = NUM_BUTONS_INIT + NUM_BUTONS_MAP + NUM_BUTONS_VEHICLE + 1;
std::string textOptions[total] = {"START", "CREDITS", "EXIT",
                        "MAP 1", "MAP 2", "MAP 3",
                        "Vehicle 1", "Vehicle 2", "Vehicle 3",
                        "Back"};

float coordinatesButtons [3][2] = {{0.0f, 0.5f}, 
                                   {0.0f, 0.1f}, 
                                   {0.0f, -0.3f}};

int marioWin;

#endif