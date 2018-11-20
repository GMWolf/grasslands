
//
// Created by Felix Bridault on 19/11/2018.
//

#include <iostream>
#include "FrameBuffer.h"
#include <algorithm>

void check_error(const char *stmt) {
    GLenum err;
    do {
        err = glGetError();
        if (err == GL_INVALID_ENUM) std::cerr << "OpenGL invalid enum at " << stmt << "\n";
        else if (err == GL_INVALID_VALUE) std::cerr << "OpenGL invalid value at " << stmt << "\n";
        else if (err == GL_INVALID_OPERATION) std::cerr << "OpenGL invalid operation at " << stmt << "\n";
        else if (err == GL_INVALID_FRAMEBUFFER_OPERATION) std::cerr << "OpenGL invalid framebuffer operation at  " << stmt << "\n";
        else if (err == GL_OUT_OF_MEMORY) std::cerr << "OpenGL out of memory at " << stmt << "\n";

        if (err != 0) {
            // Who needs breakpoints when you can invoke SIGSEGV instead?
            puts(nullptr);
        }
    } while (err);
}

FrameBuffer FrameBuffer::windowBuffer = FrameBuffer();

GLuint FrameBuffer::addTexture(GLenum format, GLenum attachment) {


    GLenum target = samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    GLuint tex;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &tex);

    glBindTexture(target, tex);


    if (samples > 1) {
        glTexStorage2DMultisample(target, samples, format, width, height, false);
    } else {
        glTexStorage2D(target, 1, format, width, height);
   }
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, tex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return tex;
}

FrameBuffer::FrameBuffer(int width, int height, int samples) : width(width), height(height), samples(samples) {
    glGenFramebuffers(1, &fbo);
}

FrameBuffer::~FrameBuffer() {
    release();
}

void FrameBuffer::release() {
    glDeleteFramebuffers(1, &fbo);
    fbo = 0;
}

FrameBuffer::FrameBuffer(FrameBuffer &&other) noexcept : fbo(other.fbo), samples(other.samples), width(other.width), height(other.height) {
    other.fbo = 0;
}

FrameBuffer &FrameBuffer::operator=(FrameBuffer &&other) noexcept {
    if (this != &other) {
        release();
        std::swap(fbo, other.fbo);
    }
    return *this;
}

void FrameBuffer::setDrawTarget() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void FrameBuffer::setReadTarget() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
}

void FrameBuffer::setTarget() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
