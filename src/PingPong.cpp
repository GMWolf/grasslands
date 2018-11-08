//
// Created by b5023110 on 08/11/2018.
//

#include "PingPong.h"

PingPong::PingPong(int width, int height) {

    glGenFramebuffers(1, &fboA);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboA);
    glGenTextures(1, &dTexA);
    glBindTexture(GL_TEXTURE_2D, dTexA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dTexA, 0);

    glGenTextures(1, &cTexA);
    glBindTexture(GL_TEXTURE_2D, cTexA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cTexA, 0);


    glGenFramebuffers(1, &fboB);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboB);
    glGenTextures(1, &dTexB);
    glBindTexture(GL_TEXTURE_2D, dTexB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dTexB, 0);

    glGenTextures(1, &cTexB);
    glBindTexture(GL_TEXTURE_2D, cTexB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cTexB, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint PingPong::getFBO() {
    return swapped ? fboA : fboB;
}

void PingPong::swap() {
    swapped = !swapped;
}

GLuint PingPong::getTexture() {
    return swapped ? cTexA : cTexB;
}
