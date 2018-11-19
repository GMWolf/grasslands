//
// Created by felix on 30/09/2018.
//

#include "DDSLoad.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gli/gli.hpp>



GLubyte defaultTexels[16] = {
        0, 0, 0, 255,
        255, 0, 0, 255,
        0, 255, 0, 255,
        0, 0, 255, 255
};

Texture loadDDS(TextureGroup& group, const std::string filename) {
    gli::texture texSource(gli::load(filename));
    if (texSource.empty()) {
        std::cout << "could not load file" << filename << std::endl;
        return brokenTexture(group);
    }

    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format format = GL.translate(texSource.format(), texSource.swizzles());

    GLenum target;
    switch(texSource.target()) {
        case gli::TARGET_2D: target = GL_TEXTURE_2D_ARRAY; break;
        case gli::TARGET_CUBE: target = GL_TEXTURE_CUBE_MAP_ARRAY; break;
        default: {
            std::cout << "Unsuported target " << filename << std::endl;
            return brokenTexture(group);
        }
    }

    Texture texture = group.getArray(target, texSource.extent(0).x, texSource.extent(0).y, texSource.levels(), format.Internal).getTexture();
    //Load each face and mipmap
    for(unsigned int face = 0; face < texSource.faces(); face++) {
        for (unsigned int level = 0; level < texSource.levels(); ++level) {
            if (gli::is_compressed(texSource.format())) {
                texture.setCompressedData(level, format.Internal, 0, 0, texSource.extent(level).x,
                                          texSource.extent(level).y, texSource.size(level),
                                          texSource.data(0, face, level), face);
            } else {
                texture.setData(level, format.External, 0, 0, texSource.extent(level).x, texSource.extent(level).y,
                                format.Type, texSource.data(0, face, level), face);
            }
        }
    }


    return texture;

}

Texture brokenTexture(TextureGroup& group) {
    Texture tex = group.getArray(GL_TEXTURE_2D, 2,2,1,GL_RGB8).getTexture();
    tex.setData(0, GL_RGBA, 0, 0, 2, 2, GL_UNSIGNED_BYTE, defaultTexels);
    return tex;
}
