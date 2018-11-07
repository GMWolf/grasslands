//
// Created by b5023110 on 06/11/2018.
//

#ifndef GRASSLANDS_CSM_H
#define GRASSLANDS_CSM_H

#include "glad/glad.h"
#include "glm.hpp"
#include "Camera.h"

class ShadowMap {
public:
    ShadowMap();
    ~ShadowMap();

    void computeProjections(Camera& cam, const glm::vec3& lightdir);

    glm::mat4 projection;
    glm::mat4 view;

    GLuint tex;
    GLuint dtex;

    GLuint btex;

    GLuint fbo;
    GLuint bfbo;
};


#endif //GRASSLANDS_CSM_H
