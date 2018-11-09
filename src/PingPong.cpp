//
// Created by b5023110 on 08/11/2018.
//

#include "PingPong.h"

PingPong::PingPong(int width, int height) {

    const int samples = 4;

    glGenFramebuffers(1, &fboA);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboA);
    glGenTextures(1, &dTexA);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, dTexA);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH_COMPONENT, width, height, false);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, dTexA, 0);

    glGenTextures(1, &cTexA);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, cTexA);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA8, width, height, false);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, cTexA, 0);


    glGenFramebuffers(1, &fboB);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboB);
    glGenTextures(1, &dTexB);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, dTexB);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH_COMPONENT, width, height, false);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, dTexB, 0);

    glGenTextures(1, &cTexB);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, cTexB);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA8, width, height, false);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, cTexB, 0);

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

GLuint PingPong::getBackTexture() {
    return swapped ? cTexB : cTexA;
}

GLuint PingPong::getBackDepth() {
    return swapped ? dTexB : dTexA;
}

GLuint PingPong::getDepth() {
    return swapped ? dTexA : dTexB;
}
