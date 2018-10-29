//
// Created by b5023110 on 10/10/2018.
//

#ifndef GRASSLANDS_MATERIAL_H
#define GRASSLANDS_MATERIAL_H

#include "Texture.h"

struct MaterialData {

    MaterialData(GLuint diffuse, GLuint normal, GLuint ram, GLuint disp)
    : diffuseLayer(diffuse), normalLayer(normal), RAMLayer(ram), dispLayer(disp){
    }

    GLuint diffuseLayer;
    GLuint normalLayer;
    GLuint RAMLayer;
    GLuint dispLayer;
};

class MaterialArray;
struct Material {
    Material(MaterialArray* array, GLuint index) :array(array), index(index) {
    }
    GLuint index;
    MaterialArray* array;
};

class MaterialArray {
    friend class Renderer;
public:
    MaterialArray() {
        glCreateBuffers(1, &buffer);
        glNamedBufferStorage(buffer, materialCount * sizeof(MaterialData), nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

    Material addMaterial(const Texture& diffuse, const Texture& normal, const Texture& ram, const Texture& disp) {
        MaterialData md(diffuse.layer, normal.layer, ram.layer, disp.layer);
        GLuint id = nextID++;
        glNamedBufferSubData(buffer, id * sizeof(MaterialData), sizeof(MaterialData), &md);

        return Material(this, id);
    }


private:
    GLuint nextID = 0;
    const int materialCount = 10;
    GLuint buffer = 0;

};

#endif //GRASSLANDS_MATERIAL_H
