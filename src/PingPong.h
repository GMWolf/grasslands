//
// Created by b5023110 on 08/11/2018.
//

#ifndef GRASSLANDS_PINGPONG_H
#define GRASSLANDS_PINGPONG_H

#include "glad/glad.h"
#include "FrameBuffer.h"

class PingPong {
public:
    PingPong(int width, int height, int samples);

    bool swapped = false;

    FrameBuffer& getFBO();

    FrameBuffer& getBackFBO();

    void swap();

    //GLuint fboA, fboB;
    FrameBuffer fboA, fboB;

    GLuint cTexA, cTexB, dTexA, dTexB;

    GLuint getTexture();

    GLuint getBackTexture();

    GLuint getBackDepth();

    GLuint getDepth();

    const int samples;
};


#endif //GRASSLANDS_PINGPONG_H
