#ifndef LOAD_OBJ_H
#define LOAD_OBJ_H

#include <string>

#include <GL/gl.h> 

#include "../game_render.h"

GLuint loadObj(std::string path);
void getHitBox(std::string path, Hitbox *hitBox);
void getCoordinates(std::string path, Coordinates4f *coordinates);

#endif
