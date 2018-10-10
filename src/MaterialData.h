//
// Created by b5023110 on 10/10/2018.
//

#ifndef GRASSLANDS_MATERIAL_H
#define GRASSLANDS_MATERIAL_H

#include "Texture.h"

struct MaterialData {

    MaterialData(GLuint m) {
        diffuseLayer = m;
    }

    GLuint diffuseLayer;

};

class MaterialArray {

public:
    MaterialArray() {
        glCreateBuffers(1, &buffer);
        glNamedBufferStorage(buffer, materialCount * sizeof(MaterialData), nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

    GLuint addMaterial(const Texture& diffuse) {
        MaterialData md(diffuse.layer);
        GLuint id = nextID++;
        glNamedBufferSubData(buffer, id * sizeof(MaterialData), sizeof(MaterialData), &md);
    }


private:
    GLuint nextID;
    const int materialCount = 10;
    GLuint buffer;

};

#endif //GRASSLANDS_MATERIAL_H
