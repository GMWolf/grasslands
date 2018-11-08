//
// Created by b5023110 on 08/11/2018.
//

#ifndef GRASSLANDS_PINGPONG_H
#define GRASSLANDS_PINGPONG_H

#include "glad/glad.h"

class PingPong {
public:
    PingPong(int width, int height);

    bool swapped = false;

    GLuint getFBO();

    void swap();

    GLuint fboA, fboB;

    GLuint cTexA, cTexB, dTexA, dTexB;

    GLuint getTexture();

};


#endif //GRASSLANDS_PINGPONG_H
