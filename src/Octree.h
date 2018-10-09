//
// Created by b5023110 on 08/10/2018.
//

#ifndef GRASSLANDS_OCTREE_H
#define GRASSLANDS_OCTREE_H

#include <glm.hpp>
#include "RenderObject.h"
#include <vector>

struct OctreeNode {

    OctreeNode() = default;;
    OctreeNode(glm::vec3 center, float halfSize, int maxItems);


    void insert(RenderObject* renderObject);

    bool contains(RenderObject* robj);


    OctreeNode* nodes = nullptr;

    std::vector<RenderObject*> renderObjects;

    int maxItems;

    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
    float halfSize;
};

class Octree {

public:
    Octree(float halfSize) : root(glm::vec3(0,0,0), halfSize, 2048){
    }

    OctreeNode root;
};


#endif //GRASSLANDS_OCTREE_H
