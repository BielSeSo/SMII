#ifndef PLAYER_H
#define PLAYER_H

#include <GL/gl.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

#define ROUTE_1 "sources/assets/Kart_1.obj"
#define ROUTE_2 "sources/assets/Kart_2.obj"
#define ROUTE_3 "sources/assets/Kart_3.obj"

static inline float rad(float a)
{
    return a * PI / 180.0f;
}

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