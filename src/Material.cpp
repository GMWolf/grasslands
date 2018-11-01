//
// Created by b5023110 on 01/11/2018.
//
#include "Material.h"

baseMaterialType::baseMaterialType(Shader *shader, GLenum primitiveType, uint32_t count, size_t dataSize)
    : shader(shader), count(count), dataSize(dataSize), primType(primitiveType){
    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(buffer, count * dataSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
}

Material baseMaterialType::addMaterial(void *data) {
    GLuint id = nextID++;
    glNamedBufferSubData(buffer, id * dataSize, dataSize, data);
    return Material(this, id);
}