//
// Created by felix on 29/09/2018.
//

#ifndef GRASSLANDS_UTIL_H
#define GRASSLANDS_UTIL_H

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/packing.hpp>

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
		return *this;
    }
};


struct Vec3Half {
    GLhalf values[3];

    Vec3Half& operator=(const glm::vec3& v) {
        values[0] = glm::packHalf1x16(v.x);
        values[1] = glm::packHalf1x16(v.y);
        values[2] = glm::packHalf1x16(v.z);
        return *this;
    }


    operator glm::vec3() const{
        return glm::vec3(
                glm::unpackHalf1x16(values[0]),
                glm::unpackHalf1x16(values[1]),
                glm::unpackHalf1x16(values[2]));
    }
};

#endif //GRASSLANDS_UTIL_H
