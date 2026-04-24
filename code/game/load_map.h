#ifndef LOAD_MAP_H
#define LOAD_MAP_H

#include <GL/gl.h>

#include "../game_render.h"

#define ROUTE_MAP_1 "sources/assets/maps/Track_1.obj"
#define ROUTE_MAP_2 "sources/assets/maps/Track_2.obj"
#define ROUTE_MAP_3 "sources/assets/maps/Track_3.obj"

// Puesto limites primitivos por dificultad de implementacion
#define ROUTE_LIMIT_MAP_1 "sources/assets/maps/Track_Limit_1.obj"
#define ROUTE_LIMIT_MAP_2 "sources/assets/maps/Track_Limit_2.obj"
#define ROUTE_LIMIT_MAP_3 "sources/assets/maps/Track_Limit_3.obj"

#define ROUTE_MAP_HITBOX "sources/assets/maps/Track_Limit.obj"

class Map
{
    private:
        GLuint mapList;
        Coordinates4f mapSize;
        Coordinates4f trackLimit;

        void calculateSize(void);

    public:
        Map();
        void setupLights(void);
        GLuint loadMap(int selectedMap);
        void destroyLights(void);
        Coordinates4f getMapSize(void);
};

#endif