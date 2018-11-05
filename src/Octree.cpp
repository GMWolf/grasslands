//
// Created by b5023110 on 08/10/2018.
//

#include "Octree.h"
#include <iostream>

OctreeNode::OctreeNode(glm::vec3 center, float halfSize, int maxItems) : center(center), halfSize(halfSize), maxItems(maxItems){
    min = center - glm::vec3(halfSize);
    max = center + glm::vec3(halfSize);
}

bool OctreeNode::contains(RenderObject *robj) {

    //get all 8 corners
    /*const glm::vec3& bboxMin = robj->mesh.bboxMin;
    const glm::vec3& bboxMax = robj->mesh.bboxMax;
    glm::vec3 corners[8];
    corners[0] = robj->transform.apply(glm::vec3(bboxMin[0], bboxMax[1], bboxMin[2]));
    corners[1] = robj->transform.apply(glm::vec3(bboxMin[0], bboxMax[1], bboxMax[2]));
    corners[2] = robj->transform.apply(glm::vec3(bboxMax[0], bboxMax[1], bboxMax[2]));
    corners[3] = robj->transform.apply(glm::vec3(bboxMax[0], bboxMax[1], bboxMin[2]));
    corners[4] = robj->transform.apply(glm::vec3(bboxMax[0], bboxMin[1], bboxMin[2]));
    corners[5] = robj->transform.apply(glm::vec3(bboxMax[0], bboxMin[1], bboxMax[2]));
    corners[6] = robj->transform.apply(glm::vec3(bboxMin[0], bboxMin[1], bboxMax[2]));
    corners[7] = robj->transform.apply(glm::vec3(bboxMin[0], bboxMin[1], bboxMin[2]));

    bool inside = false;
    for(int i = 0; i < 8; i++) {
        glm::bvec3 gt = glm::greaterThan(corners[i], min);
        glm::bvec3 lt = glm::lessThan(corners[i], max);
        bool cornerInside = glm::all(gt) && glm::all(lt);
        inside = inside || cornerInside;
    }

    return inside;*/
    return all(glm::lessThanEqual(robj->transform.pos, max)) && all(glm::greaterThan(robj->transform.pos, min));
}

void OctreeNode::insert(RenderObject *renderObject) {

    if(contains(renderObject)) {

        if(nodes == nullptr) {
            renderObjects.push_back(renderObject);

            if(renderObjects.size() > maxItems) {
                nodes = new OctreeNode[8];
                for(int i = 0; i < 8; i++) {
                    float ox = i & 4 ? 1 : -1;
                    float oy = i & 2 ? 1 : -1;
                    float oz = i & 1 ? 1 : -1;
                    nodes[i] = OctreeNode(center + glm::vec3(ox, oy, oz) * (halfSize / 2), halfSize/2, maxItems);
                    for(auto robj : renderObjects) {
                        nodes[i].insert(robj);
                    }
                }
                renderObjects.clear();
            }

        } else {

            for(int i = 0; i < 8; i++) {
                nodes[i].insert(renderObject);
            }

        }
    }
}
