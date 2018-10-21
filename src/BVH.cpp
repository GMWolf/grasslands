//
// Created by felix on 20/10/2018.
//

#include "BVH.h"

BVH::BVH(int maxItems) : maxItems(maxItems) {
}

void BVH::insert(RenderObject *obj) {

    min = glm::min(min, obj->mesh.bboxMin + obj->transform.pos);
    max = glm::max(max, obj->mesh.bboxMax + obj->transform.pos);

    if(children == nullptr){
        objects.emplace_back(obj);

        if (objects.size() > maxItems) {
            split();
        }
    } else {

        glm::vec3 leftMin = glm::min(children[0].min, obj->mesh.bboxMin + obj->transform.pos);
        glm::vec3 leftMax = glm::max(children[0].max, obj->mesh.bboxMax + obj->transform.pos);
        //float leftPerim = glm::compAdd(children[0].max - children[0].min);
        float leftPerimF = glm::compAdd(leftMax - leftMin);
       // float leftPerimDelta = leftPerimF - leftPerim;
        glm::vec3 rightMin = glm::min(children[1].min, obj->mesh.bboxMin + obj->transform.pos);
        glm::vec3 rightMax = glm::max(children[1].max, obj->mesh.bboxMax + obj->transform.pos);
        //float rightPerim = glm::compAdd(children[1].max - children[1].min);
        float rightPerimF = glm::compAdd(rightMax - rightMin);
        //float rightPerimDelta = rightPerimF - rightPerim;


        int into = (leftPerimF < rightPerimF) ? 0 : 1;

        children[into].insert(obj);
    }
}

void BVH::split() {

    children = new BVH[2]{
            BVH(maxItems),
            BVH(maxItems)
    };

    for(RenderObject* o : objects) {
        insert(o);
    }

    //objects.clear();
    std::vector<RenderObject*>().swap(objects);
}
