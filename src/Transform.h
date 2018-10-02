//
// Created by felix on 02/10/2018.
//

#ifndef GRASSLANDS_TRANSFORM_H
#define GRASSLANDS_TRANSFORM_H

#include <glm.hpp>
#include <gtc/quaternion.hpp>

struct Transform {

    glm::vec3 pos;
    float scale;
    glm::quat rot;

};

#endif //GRASSLANDS_TRANSFORM_H
