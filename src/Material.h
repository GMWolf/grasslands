//
// Created by b5023110 on 10/10/2018.
//

#ifndef GRASSLANDS_MATERIAL_H
#define GRASSLANDS_MATERIAL_H

#include "Texture.h"
#include "Shader.h"

struct MaterialData {



    MaterialData(GLuint diffuse, GLuint normal, GLuint ram, GLuint disp)
    : diffuseLayer(diffuse), normalLayer(normal), RAMLayer(ram), dispLayer(disp){
    }
    MaterialData(const Texture& diffuse, const Texture& normal, const Texture& ram, const Texture& disp)
    : MaterialData(diffuse.layer, normal.layer, ram.layer, disp.layer) {
    }

    GLuint diffuseLayer;
    GLuint normalLayer;
    GLuint RAMLayer;
    GLuint dispLayer;
};


class baseMaterialType;
struct Material {
    Material(baseMaterialType* type, GLuint index) : type(type), index(index) {
    }
    GLuint index;
    baseMaterialType* type;
};


class baseMaterialType {
    friend class Renderer;
public:
    baseMaterialType(Shader* shader, GLenum primitiveType, uint32_t count, size_t dataSize);

    Material addMaterial(void* data);

private:
    GLenum primType;
    GLuint nextID = 0;
    Shader* shader;
    GLuint buffer = 0;
    const size_t dataSize;
    const uint32_t  count;
};

template<class T, int PRIMITIVE_TYPE = GL_TRIANGLES>
class MaterialType : baseMaterialType{

public:
    MaterialType(Shader* shader, uint32_t count);

    Material addMaterial(const T& data);
};

template<class T, int P>
MaterialType<T, P>::MaterialType(Shader *shader, uint32_t count) :  baseMaterialType(shader, P, count, sizeof(T)) {
}

template<class T, int P>
Material MaterialType<T, P>::addMaterial(const T &data) {
    return baseMaterialType::addMaterial((void*)&data);
}

#endif //GRASSLANDS_MATERIAL_H
