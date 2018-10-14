//
// Created by felix on 02/10/2018.
//

#ifndef GRASSLANDS_RENDEROBJECT_H
#define GRASSLANDS_RENDEROBJECT_H

#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"
#include "Material.h"


struct RenderObject {

    RenderObject(const Mesh& mesh, const Material& mat, const Transform& transform);

    const Mesh& mesh;
    const Material& mat;

    Transform transform;

    const bool isStatic;
};


#endif //GRASSLANDS_RENDEROBJECT_H
