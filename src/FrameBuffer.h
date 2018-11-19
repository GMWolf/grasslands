
//
// Created by Felix Bridault on 19/11/2018.
//

#ifndef GRASSLANDS_FRAMEBUFFER_H
#define GRASSLANDS_FRAMEBUFFER_H


#include <glad/glad.h>

class FrameBuffer {

public:
    FrameBuffer(int width, int height, int samples = 0);


    GLuint addTexture(GLenum format, GLenum attachment);


    GLuint fbo;
    const int samples;
    const int width, height;
};


#endif //GRASSLANDS_FRAMEBUFFER_H
