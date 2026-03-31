#include <cmath>
#include "player.hpp"

Player::Player(int x, int y, int z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

Vec3 Player::get_pos()
{
    return pos;
}

void Player::move()
{   
    if(grados >= 360 || grados <=-360)
    {
        grados = 0;
    }

    pos.x -= velocidad * sinf(rad(grados));
	pos.y += velocidad * cosf(rad(grados));
}


