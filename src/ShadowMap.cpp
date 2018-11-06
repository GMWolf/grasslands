//
// Created by b5023110 on 06/11/2018.
//

#include "ShadowMap.h"
#include <limits>

void ShadowMap::computeProjections(Camera &cam, const glm::vec3 &lightdir) {

    glm::mat4 invViewProj = glm::inverse(cam.proj * cam.view);

    //get view corners
    glm::vec3 centroid(0,0,0);
    glm::vec3 corners[8];
    glm::vec2 a(1, -1);
    for(int i = 0; i < 8; i++) {
        corners[i].x = (i & 1) ? 1 : -1;
        corners[i].y = (i & 2) ? 1 : -1;
        corners[i].z = (i & 4) ? 1 : -1;
        corners[i] = glm::vec3((invViewProj * glm::vec4(corners[i], 1.0)));
        centroid += corners[i];
    }

    centroid /= 8;


    //compute view
    glm::vec3 lightPos = centroid - (lightdir * cam.farPlane);

    view = glm::lookAt(lightPos, centroid, glm::cross(glm::cross(lightdir, glm::vec3(0,1,0)), lightdir));


    //get min max
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::min());

    for(glm::vec3& c : corners) {
        glm::vec3 p = glm::vec3(view * glm::vec4(c, 1.0f));
        min = glm::min(min, p);
        max = glm::max(max, p);
    }

    //extend ortho
    projection = glm::ortho(min.x, max.x, min.y, max.y, -max.z, -min.z);
}

ShadowMap::ShadowMap() {
    glCreateFramebuffers(1, &fbo);
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);

    glTextureStorage2D(fbo, 1, GL_DEPTH_COMPONENT32F, 2048, 2048);

    glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, tex, 0);
}

ShadowMap::~ShadowMap() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &tex);
}
