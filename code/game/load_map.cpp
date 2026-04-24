#include <cstdio>
#include <GL/glut.h>
#include <GL/glu.h>

#include "load_map.h"
#include "load_obj.h"

Map::Map()
{
    mapList = 0;
    
    calculateSize();
}

void Map::calculateSize(void)
{
    getCoordinates(ROUTE_MAP_HITBOX, &mapSize);
}


void Map::setupLights() 
{
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT1);

    GLfloat pos[]  = { 1.0f, 1.0f, 2.0f, 0.0f }; // direccional  
    GLfloat diff[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat amb[]  = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat spec[] = {0.1f, 0.1f, 0.1f, 1.0f};
    
    glLightfv(GL_LIGHT1, GL_POSITION, pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  diff);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  amb);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

GLuint Map::loadMap(int selectedMap)
{   
    switch(selectedMap)
    {
        case 1:
            mapList = loadObj(ROUTE_MAP_1);
            getCoordinates(ROUTE_LIMIT_MAP_1, &trackLimit);
            break;

        case 2:
            mapList = loadObj(ROUTE_MAP_2);
            getCoordinates(ROUTE_LIMIT_MAP_2, &trackLimit);
            break;

        case 3:
            mapList = loadObj(ROUTE_MAP_3);
            getCoordinates(ROUTE_LIMIT_MAP_3, &trackLimit);
            break;

        default: break;
    }
    
    return mapList;
}

void Map::destroyLights(void)
{
    glDisable(GL_LIGHT1);
}

Coordinates4f Map::getMapSize(void)
{
    return mapSize;
}
