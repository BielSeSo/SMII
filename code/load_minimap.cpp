#include <cstdio>
#include <GL/glut.h>
#include <GL/glu.h>

#include "load_minimap.hpp"
#include "load_obj.hpp"
//#include "codebase.h"

Loader loader_map;

void Map::selectMap(int selectedMap)
{
    this->selectedMap = selectedMap;
}

void Map::setupLights() 
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat pos[]  = {1.0f, 1.0f, 2.0f, 0.0f}; // direccional
    GLfloat diff[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat amb[]  = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat spec[] = {0.1f, 0.1f, 0.1f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diff);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void Map::load_map()
{   
    switch(selectedMap)
    {
        case 1:
            loader_map.load_model("assets/Track_1.obj");
            break;

        default: break;
    }
    
}