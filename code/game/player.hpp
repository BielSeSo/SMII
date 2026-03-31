#ifndef PLAYER_HPP
#define PLAYER_HPP

#define PI 3.14
#define rad(a) (a*PI/180)

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

struct Vec3{
    float x, y, z;
};

class Player
{
    private:
        Vec3 pos;
        float hitbox[2];

    public:
        float velocidad;
        float grados;

        Player(int x, int y, int z);
        Vec3 get_pos();
        void move();
};

#endif