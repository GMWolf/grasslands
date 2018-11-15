//
// Created by b5023110 on 08/11/2018.
//

#ifndef GRASSLANDS_PASS_H
#define GRASSLANDS_PASS_H

#include <glad/glad.h>
#include <glm.hpp>
#include "Shader.h"
#include <functional>

struct Pass {
    Pass() {
    }

    Pass(const Pass&) = delete;
    Pass(Pass &&) = delete;
    Pass&operator=(const Pass&) = delete;
    Pass&operator=(Pass&&) = delete;

    virtual ~Pass() = default;

    GLuint fbo;

    bool clearBuffers = true;
    glm::vec4 clearColour = glm::vec4(42,42,42,42);
    int viewportX, viewportY, viewportW, viewportH;

    std::function<void(void)> setup;
};

struct ScenePass : public Pass {
    ~ScenePass() override = default;

    glm::mat4 view;
    glm::mat4 projection;
    bool shadowPass;
};

struct PostPass : public Pass {
    ~PostPass() override = default;

    Shader* shader;
    GLuint tex;
    GLuint dtex;
    bool generateMipMaps;


};

struct ComputePass : public Pass {
    ~ComputePass() override = default;

    Shader* shader;

    GLuint x, y, z;

    GLenum barrier;
};



#endif //GRASSLANDS_PASS_H
