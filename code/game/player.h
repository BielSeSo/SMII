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
        GLuint kartList;

        Vec3 pos;
        float hitbox[2];

    public:
        float velocidad;
        float grados;
        

        Player(float x, float y, float z);
        void selectKart(int selectedKart);
        GLuint loadVehicle(void);
        Vec3 getPos(void);
        void move(void);
};

#endif