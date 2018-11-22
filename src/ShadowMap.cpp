//
// Created by b5023110 on 06/11/2018.
//

#include "ShadowMap.h"
#include <limits>
#include <gtc/matrix_transform.hpp>
#include <iostream>

void ShadowMap::computeProjections(const Camera &cam, const glm::vec3 &lightdir) {

    glm::mat4 invViewProj = glm::inverse(cam.proj * cam.view);

    //get view corners
    glm::vec3 centroid(0,0,0);
    glm::vec4 corners[8];
    glm::vec2 a(1, -1);
    for(int i = 0; i < 8; i++) {
        corners[i].x = (i & 1) ? 1 : -1;
        corners[i].y = (i & 2) ? 1 : -1;
        corners[i].z = (i & 4) ? zFar : zNear;
        corners[i].w = 1.0;
        corners[i] = (invViewProj * corners[i]);
        corners[i] /= corners[i].w;
        centroid += glm::vec3(corners[i]);
    }

    centroid /= 8;

    //compute view
    glm::vec3 lightPos = centroid - (lightdir * cam.farPlane);

    view = glm::lookAt(lightPos, centroid, glm::normalize(glm::cross(glm::normalize(glm::cross(lightdir, glm::vec3(0,1,0))), lightdir)));


    //get min max
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(-std::numeric_limits<float>::max());

    for(glm::vec4& c : corners) {
        glm::vec3 p = glm::vec3(view * c);
        min = glm::min(min, p);
        max = glm::max(max, p);
    }

    projection = glm::ortho(min.x, max.x, min.y, max.y, 0.01f, -min.z);

}

ShadowMap::ShadowMap(int resolution, float zNear, float zFar) : zNear(zNear), zFar(zFar), resolution(resolution),
fbo(resolution, resolution, 1), blurFbo(resolution, resolution, 1){



    dtex = fbo.addTexture(GL_DEPTH_COMPONENT32, GL_DEPTH_ATTACHMENT);
    tex = fbo.addTexture(GL_RG16F, GL_COLOR_ATTACHMENT0);
    btex = blurFbo.addTexture(GL_RG16F, GL_COLOR_ATTACHMENT0);
}

ShadowMap::~ShadowMap() {
    glDeleteTextures(1, &tex);
    glDeleteTextures(1, &dtex);
    glDeleteTextures(1, &btex);
}

CSM::CSM(unsigned int levels, unsigned int baseRes) : levels(levels), baseRes(baseRes) {
    unsigned int res = baseRes;
    float z0 = 0;
    for(int i = 0; i < levels; i++) {
        float z1 = z0 + 1 / (float) levels;
//        shadowMaps.emplace_back(res, z0, z1);
        res /= 2;
        z0 = z1;
    }
}

void CSM::computeProjections(Camera &cam, const glm::vec3 &lightDir) {
    for(auto& sm : shadowMaps) {
        sm.computeProjections(cam, lightDir);
    }
}
