//
// Created by felix on 02/10/2018.
//

#include "RenderObject.h"

RenderObject::RenderObject(const Mesh &mesh, const Material &mat, const Transform &transform, const bool s) :
mesh(mesh), mat(mat), transform(transform),isStatic(s) {

}
