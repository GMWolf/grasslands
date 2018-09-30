//
// Created by felix on 29/09/2018.
//

#ifndef GRASSLANDS_UTIL_H
#define GRASSLANDS_UTIL_H

#include <glad/glad.h>
#include <glm.hpp>

union Vec3Int{
    GLint i32;
    struct {
        int x : 10;
        int y : 10;
        int z : 10;
        int w : 2;
    };

    Vec3Int& operator=(const glm::vec3& v) {
        x = v.x * 0x1FF;
        y = v.y * 0x1FF;
        z = v.z * 0x1FF;
    }
};

#endif //GRASSLANDS_UTIL_H
