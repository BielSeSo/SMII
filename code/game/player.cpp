#include <cmath>
#include "player.h"

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

Vec3 Player::get_pos() const
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