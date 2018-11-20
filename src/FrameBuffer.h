
//
// Created by Felix Bridault on 19/11/2018.
//

#ifndef GRASSLANDS_FRAMEBUFFER_H
#define GRASSLANDS_FRAMEBUFFER_H


#include <glad/glad.h>


class FrameBuffer {

public:

    static FrameBuffer windowBuffer;

    FrameBuffer(int width, int height, int samples = 0);
    ~FrameBuffer();

    void release();

    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    FrameBuffer(FrameBuffer&& other) noexcept;
    FrameBuffer& operator=(FrameBuffer&& other) noexcept;


    GLuint addTexture(GLenum format, GLenum attachment);

    void setDrawTarget();
    void setReadTarget();
    void setTarget();

    GLuint fbo;
    const int samples;
    const int width, height;

private:
    FrameBuffer() :fbo(0), samples(1), width(0), height(0) {
    }
};


#endif //GRASSLANDS_FRAMEBUFFER_H
