#ifndef PLAYER_H
#define PLAYER_H

#include <GL/gl.h>

#include "../game_render.h"

#define ROUTE_KART_1 "sources/assets/models/Kart_1.obj"
#define ROUTE_KART_2 "sources/assets/models/Kart_2.obj"
#define ROUTE_KART_3 "sources/assets/models/Kart_3.obj"

#define ROUTE_KART_HITBOX "sources/assets/models/Kart_Limit.obj"

class Player
{
    private:
        int selectedKart;
        GLuint kartList;
        Vec3 pos;
        Hitbox kartSize;  

        void calculateSize(void);

    public:
        float velocidad;
        float grados;
        
        Player(float x, float y, float z);
        void selectKart(int selectedKart);
        GLuint loadVehicle(void);
        Vec3 getPos(void);
        void editPos(Vec3 pos);
        void move(void);
        Hitbox getHitbox(void);
};

#endif