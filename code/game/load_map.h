#ifndef LOAD_MAP_H
#define LOAD_MAP_H

#include <GL/gl.h>

#include "../game_render.h"

#define ROUTE_MAP_1 "sources/assets/maps/Track_1.obj"
#define ROUTE_MAP_2 "sources/assets/maps/Track_2.obj"
#define ROUTE_MAP_3 "sources/assets/maps/Track_3.obj"

#define ROUTE_MAP_HITBOX "sources/assets/maps/Track_Limit.obj"

class Map
{
    private:
        int selectedMap;
        GLuint mapList;
        Coordinates4f mapSize;

        void calculateSize(void);

    public:
        Map();
        void selectMap(int selectedMap);
        void setupLights(void);
        GLuint loadMap(void);
        void destroyLights(void);
        Coordinates4f getMapSize(void);
};

#endif