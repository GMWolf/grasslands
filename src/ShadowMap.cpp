//
// Created by b5023110 on 06/11/2018.
//

#include "ShadowMap.h"
#include <limits>
#include <ext/matrix_transform.hpp>
#include <iostream>

void ShadowMap::computeProjections(Camera &cam, const glm::vec3 &lightdir) {

    pass.shadowPass = true;

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
    glm::vec3 lightPos = centroid - (lightdir *cam.farPlane);

    pass.view = glm::lookAt(lightPos, centroid, glm::normalize(glm::cross(glm::normalize(glm::cross(lightdir, glm::vec3(0,1,0))), lightdir)));


    //get min max
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(-std::numeric_limits<float>::max());

    for(glm::vec4& c : corners) {
        glm::vec3 p = glm::vec3(pass.view * c);
        min = glm::min(min, p);
        max = glm::max(max, p);
    }

    pass.projection = glm::ortho(min.x, max.x, min.y, max.y, 0.01f, -min.z);

}

ShadowMap::ShadowMap(int resolution, float zNear, float zFar) : zNear(zNear), zFar(zFar), resolution(resolution) {

    pass.viewportW = resolution;
    pass.viewportH = resolution;
    pass.viewportX = 0;
    pass.viewportY = 0;

    postPass.viewportW = resolution;
    postPass.viewportH = resolution;
    postPass.viewportX = 0;
    postPass.viewportY = 0;
    postPass.clearBuffers = true;
    postPass.clearColour = glm::vec4(0,0,0,0);


    /*glCreateFramebuffers(1, &fbo);
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);


    glTextureStorage2D(fbo, 1, GL_DEPTH_COMPONENT, 2048, 2048);

    glTextureParameterf(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTextureParameterf(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);



    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);


    glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, tex, 0);*/

    glGenFramebuffers(1, &pass.fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pass.fbo);
    glGenTextures(1, &dtex);
    glBindTexture(GL_TEXTURE_2D, dtex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dtex, 0);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, resolution, resolution, 0, GL_RG, GL_FLOAT, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);


    glGenFramebuffers(1, &postPass.fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postPass.fbo);

    glGenTextures(1, &btex);
    glBindTexture(GL_TEXTURE_2D, btex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, resolution, resolution, 0, GL_RG, GL_FLOAT, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, btex, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    postPass.tex = tex;
    postPass.dtex = btex;
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
