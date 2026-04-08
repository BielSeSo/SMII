#include <iostream>
#include <string>
#include <algorithm>

#include <GL/glut.h>
#include <GL/glu.h>

#include "load_obj.h"

using namespace std;

/*
void computeBounds() 
{
    aiVector3D minV, maxV;

    minV = aiVector3D( 1e10f,  1e10f,  1e10f);
    maxV = aiVector3D(-1e10f, -1e10f, -1e10f);
    for (unsigned int m = 0; m < gScene->mNumMeshes; ++m) {
        const aiMesh* mesh = gScene->mMeshes[m];
        for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
            const aiVector3D& p = mesh->mVertices[v];
            minV.x = std::min(minV.x, p.x); minV.y = std::min(minV.y, p.y); minV.z = std::min(minV.z, p.z);
            maxV.x = std::max(maxV.x, p.x); maxV.y = std::max(maxV.y, p.y); maxV.z = std::max(maxV.z, p.z);
        }
    }
}
*/

GLuint load_obj(string path)
{
    static Assimp::Importer gImporter;

    const aiScene* gScene = gImporter.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices |   // aplica transformaciones de nodos -> mallas ya en espacio global
        aiProcess_SortByPType
    );

    if (!gScene) {
        std::cerr << "Error cargando modelo: " << gImporter.GetErrorString() << "\n";
        return 0;
    }
    
    GLuint gListId = glGenLists(1);
    glNewList(gListId, GL_COMPILE);

    glEnable(GL_NORMALIZE); // Por si la escala altera longitudes de normales

    for (unsigned int m = 0; m < gScene->mNumMeshes; ++m) {
        const aiMesh* mesh = gScene->mMeshes[m];

        // Color/material simple (difuso)
        aiColor4D diff(0.8f, 0.8f, 0.8f, 1.0f);
        if (gScene->mMaterials && mesh->mMaterialIndex < gScene->mNumMaterials) {
            const aiMaterial* mat = gScene->mMaterials[mesh->mMaterialIndex];
            aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diff);
        }
        glColor4f(diff.r, diff.g, diff.b, diff.a);

        // Dibujar triángulos
        glBegin(GL_TRIANGLES);
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            const aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices != 3) continue; // por si acaso (hemos triangulado, así que debería ser 3)

            for (unsigned int k = 0; k < 3; ++k) {
                unsigned int idx = face.mIndices[k];
                if (mesh->HasNormals()) {
                    const aiVector3D& n = mesh->mNormals[idx];
                    glNormal3f(n.x, n.y, n.z);
                }
                const aiVector3D& v = mesh->mVertices[idx];
                glVertex3f(v.x, v.y, v.z);
            }
        }
        glEnd();
    }

    glEndList();

    return gListId;
}
