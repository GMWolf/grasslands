//
// Created by felix on 20/10/2018.
//

#ifndef GRASSLANDS_BVH_H
#define GRASSLANDS_BVH_H


#include "glm.hpp"
#include "RenderObject.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/component_wise.hpp"
#undef GLM_ENABLE_EXPERIMENTAL

class BVH {
public:
    explicit BVH(int maxItems);

    void insert(RenderObject* obj);

    void split();

    std::vector<RenderObject*> objects;

    glm::vec3 min = glm::vec3(INFINITY), max = glm::vec3(-INFINITY);

    int maxItems;

    BVH* children = nullptr;
};


#endif //GRASSLANDS_BVH_H
