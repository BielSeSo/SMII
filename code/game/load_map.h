#ifndef LOAD_MAP_H
#define LOAD_MAP_H

#include <GL/gl.h>

#include "load_obj.h"

class Map
{
    private:
        int selectedMap;
        GLuint mapList;

    public:
        Map();
        void selectMap(int selectedMap);
        void setupLights();
        GLuint load_map(void);
};

#endif