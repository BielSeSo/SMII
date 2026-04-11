#ifndef LOAD_OBJ_H
#define LOAD_OBJ_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL/gl.h> 

#include <string>

GLuint loadObj(std::string path);
void computeBounds();
void getHitbox();

#endif
