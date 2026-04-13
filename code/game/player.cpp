#include <cmath>
#include <iostream>
#include <ostream>
#include <string>

#include "player.h"
#include "load_obj.h"

using namespace std;

static string route1 = "sources/assets/Kart_1.obj";
static string route2 = "sources/assets/Kart_2.obj";
static string route3 = "sources/assets/Kart_3.obj";

Player::Player(float x, float y, float z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;

    velocidad = 0.0f; 
    grados = 0.0f;

    hitbox[0] = 0.5f;
    hitbox[1] = 0.5f;

    kartList = 0;
}

void Player::selectKart(int selectedKart)
{
    this->selectedKart = selectedKart;
}

GLuint Player::loadVehicle(void)
{
    switch (selectedKart)
    {
        case 1:
            kartList = loadObj(route1);
            break;

        case 2:
            kartList = loadObj(route2);
            break;

        case 3:
            kartList = loadObj(route3);
            break;
    
        default: break;
    }

    return kartList;
}

Vec3 Player::getPos(void)
{
    return pos;
}

void Player::move(void)
{
    double rad = grados * M_PI / 180.0;

    if (grados >= 360.0f || grados <= -360.0f)
    {
        grados = 0.0f;
    }

    pos.x += velocidad * sinf(rad);
    pos.y += velocidad * cosf(rad);
}




