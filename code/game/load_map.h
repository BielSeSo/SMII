#ifndef LOAD_MAP_H
#define LOAD_MAP_H

#include <GL/gl.h>

#define ROUTE_1 "sources/assets/Track_1.obj"
#define ROUTE_2 "sources/assets/Track_2.obj"
#define ROUTE_3 "sources/assets/Track_3.obj"

class Map
{
    private:
        int selectedMap;
        GLuint mapList;

    public:
        Map();
        void selectMap(int selectedMap);
        void setupLights();
        GLuint loadMap(void);
};

#endif