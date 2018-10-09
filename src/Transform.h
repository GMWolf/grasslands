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

    glm::vec3 apply(const glm::vec3& p);
};

inline glm::vec3 Transform::apply(const glm::vec3 &p) {

    //return p + pos;
    return (rot * p) * scale + pos;

}

#endif //GRASSLANDS_TRANSFORM_H
