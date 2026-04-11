#include <cmath>

#include "player.h"
#include "load_obj.h"

Player::Player(float x, float y, float z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;

    velocidad = 0.0f; 
    grados = 0.0f;

    hitbox[0] = 0.5f;
    hitbox[1] = 0.5f;
}

Vec3 Player::getPos() const
{
    return pos;
}

void Player::move()
{
    if (grados >= 360.0f || grados <= -360.0f)
    {
        grados = 0.0f;
    }

    pos.x -= velocidad * sinf(rad(grados));
    pos.y += velocidad * cosf(rad(grados));
}

void Player::selectKart(int selectedKart)
{
    this->selectedKart = selectedKart;
}

GLuint Player::loadVehicle(void)
{
    GLuint kartList = 0;

    switch (selectedKart)
    {
        case 1:
            loadObj(ROUTE_1);
            break;

        case 2:
            loadObj(ROUTE_2);
            break;

        case 3:
            loadObj(ROUTE_3);
            break;
    
        default: break;
    }

    return kartList;
}
