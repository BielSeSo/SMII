#ifndef LOAD_OBJ_HPP
#define LOAD_OBJ_HPP

#include <string>
using namespace std;

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Loader
{   
    private:
        aiVector3D minV, maxV;
        const aiScene* gScene;
        void computeBounds();

    public:
        Loader();
        void load_model(string path);
        void get_hitbox(aiVector3D& minV, aiVector3D& maxV);
};

#endif
