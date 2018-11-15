//
// Created by b5023110 on 06/11/2018.
//

#ifndef GRASSLANDS_CSM_H
#define GRASSLANDS_CSM_H

#include "glad/glad.h"
#include "glm.hpp"
#include "Camera.h"
#include <vector>
#include "Pass.h"

class ShadowMap {
public:
    ShadowMap(ShadowMap&&) = delete;
    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator=(ShadowMap&&) = delete;
    ShadowMap& operator=(const ShadowMap&)= delete;

    explicit ShadowMap(int resolution, float zNear = -1, float zFar = 1);
    ~ShadowMap();

    void computeProjections(const Camera& cam, const glm::vec3& lightdir);

    GLuint tex;
    GLuint dtex;

    GLuint btex;

    float zNear;
    float zFar;

    int resolution;

    GLuint fbo;
    GLuint bfbo;

    glm::mat4 view, projection;
};

class CSM {
public:
    CSM(unsigned int levels, unsigned int baseRes);


    void computeProjections(Camera& cam, const glm::vec3& lightDir);

    unsigned int levels;
    unsigned int baseRes;
    std::vector<ShadowMap> shadowMaps;
};


#endif //GRASSLANDS_CSM_H
