//
// Created by b5023110 on 08/11/2018.
//

#include <iostream>
#include "PingPong.h"

PingPong::PingPong(int width, int height, int samples) : samples(samples), fboA(width,height, samples), fboB(width,height, samples) {

    cTexA = fboA.addTexture(GL_RGBA8, GL_COLOR_ATTACHMENT0);
    cTexB = fboB.addTexture(GL_RGBA8, GL_COLOR_ATTACHMENT0);

    dTexA = fboA.addTexture(GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT);
    dTexB = fboB.addTexture(GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT);

}

FrameBuffer& PingPong::getFBO() {
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

FrameBuffer& PingPong::getBackFBO() {
    return swapped? fboB : fboA;
}
