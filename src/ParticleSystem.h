
//
// Created by Felix Bridault on 21/11/2018.
//

#ifndef GRASSLANDS_PARTICLESYSTEM_H
#define GRASSLANDS_PARTICLESYSTEM_H


#include <glad/glad.h>
#include <vec3.hpp>
#include "Shader.h"
#include "Texture.h"

struct particleVertData {
    glm::vec3 pos; float size; glm::vec3 colour; float rot;
};

struct partUData {
    glm::vec3 vel; float age;
};

class ParticleSystem {
public:

    ParticleSystem(size_t partCount);

    void update(float dt);
    void bindVA();
    glm::ivec2 texture;
    GLenum blendSourceFactor = GL_BLEND_SRC_ALPHA;
    GLenum blendDestFactor = GL_ONE_MINUS_SRC_ALPHA;
    GLuint partCount;
    Shader* computeShader = nullptr;

    glm::vec3 position;

private:

    GLuint vertBuffer{};
    GLuint partBuffer{};
    GLuint vertexArray{};
    float time = 0;


};


#endif //GRASSLANDS_PARTICLESYSTEM_H
