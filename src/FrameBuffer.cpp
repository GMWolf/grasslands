
//
// Created by Felix Bridault on 19/11/2018.
//

#include <iostream>
#include "FrameBuffer.h"

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

GLuint FrameBuffer::addTexture(GLenum format, GLenum attachment) {
    check_error("a");

    GLenum target = samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    GLuint tex;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    check_error("b");
    glGenTextures(1, &tex);
    check_error("c");
    glBindTexture(target, tex);
    check_error("d");

    if (samples > 1) {
        glTexStorage2DMultisample(target, samples, format, width, height, false);
        check_error("e");
    } else {
        glTexStorage2D(target, 1, format, width, height);
        check_error("f");
   }


    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, tex, 0);
    check_error("g");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check_error("h");

    return tex;
}

FrameBuffer::FrameBuffer(int width, int height, int samples) : width(width), height(height), samples(samples) {
    glGenFramebuffers(1, &fbo);
    std::cout << "fob is " << fbo << std::endl;
}
