#include <cmath>
#include <iostream>
#include <ostream>
#include <string>

#include "player.h"
#include "load_obj.h"

using namespace std;

Player::Player(float x, float y, float z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;

    velocidad = 0.0f; 
    grados = 0.0f;
    
    calculateSize();

    kartList = 0;
}

void Player::calculateSize(void)
{
    getHitBox(ROUTE_KART_HITBOX, &kartSize);
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
            kartList = loadObj(ROUTE_KART_1);
            break;

        case 2:
            kartList = loadObj(ROUTE_KART_2);
            break;

        case 3:
            kartList = loadObj(ROUTE_KART_3);
            break;
    
        default: break;
    }

    return kartList;
}

Vec3 Player::getPos(void)
{
    return pos;
}

void Player::editPos(Vec3 pos)
{
    this->pos = pos;
}

void Player::move(void)
{
    double rad = grados * M_PI / 180.0;

    if (grados >= 360.0f) grados -= 360.0f;    
    if (grados <= -360.0f) grados += 360.0f;    

    pos.x += velocidad * sin(rad);
    pos.y -= velocidad * cos(rad);
}


Hitbox Player::getHitbox(void)
{
    
    return kartSize;
}