//
// Created by felix on 02/10/2018.
//

#include "RenderObject.h"

RenderObject::RenderObject(const Mesh &mesh, const Texture &texture, const Transform &transform) : mesh(mesh), texture(texture), transform(transform),
                                                                                                   isStatic(false) {

}
