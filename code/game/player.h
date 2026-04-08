#ifndef PLAYER_H
#define PLAYER_H

#ifndef PI
#define PI 3.14159265358979323846f
#endif

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
    Vec3 pos;
    float hitbox[2];  // por ejemplo: ancho y fondo

public:
    float velocidad;
    float grados;

    Player(float x, float y, float z);
    Vec3 get_pos() const;
    void move();
};

#endif