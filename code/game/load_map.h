#ifndef LOAD_MAP_H
#define LOAD_MAP_H

#include <GL/gl.h>

#include "../game_render.h"

#define ROUTE_MAP_1 "sources/assets/maps/Track_1.obj"
#define ROUTE_MAP_2 "sources/assets/maps/Track_2.obj"
#define ROUTE_MAP_3 "sources/assets/maps/Track_3.obj"

// Puesto limites primitivos por dificultad de implementacion
#define ROUTE_LIMIT_TRACK_1 "sources/assets/maps/Track_Limit_1.obj"
#define ROUTE_LIMIT_TRACK_2 "sources/assets/maps/Track_Limit_2.obj"
#define ROUTE_LIMIT_TRACK_3 "sources/assets/maps/Track_Limit_3.obj"

// Definicion de los limites del mapa
#define ROUTE_LIMIT_MAP_1 "sources/assets/maps/Map_Limit_1.obj"
#define ROUTE_LIMIT_MAP_2 "sources/assets/maps/Map_Limit_2.obj"
#define ROUTE_LIMIT_MAP_3 "sources/assets/maps/Map_Limit_3.obj"

class Map
{
    private:
        GLuint mapList;
        Coordinates4f mapSize;
        Coordinates4f trackLimit;

    public:
        Map();
        void setupLights(void);
        GLuint loadMap(int selectedMap);
        void destroyLights(void);
        Coordinates4f getSize(bool isMapLimit);
};

#endif