#ifndef PLAYER_H
#define PLAYER_H

#include <GL/gl.h>

struct Vec3
{
    float x, y, z;
};

class Player
{
    private:
        int selectedKart;
        Vec3 pos;
        float hitbox[2];  // por ejemplo: ancho y fondo
        GLuint kartList;

    public:
        float velocidad;
        float grados;

        Player(float x, float y, float z);
        void selectKart(int selectedKart);
        GLuint loadVehicle(void);
        Vec3 getPos() const;
        void move();
};

#endif