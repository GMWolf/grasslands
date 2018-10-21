//
// Created by felix on 30/09/2018.
//

#include <iostream>
#include "Texture.h"


Texture::Texture(TextureArray *textureArray, GLuint layer) : textureArray(textureArray), layer(layer) {
}

void Texture::setData(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, GLenum type, const void * pixels) {
    glTextureSubImage3D(textureArray->texture, level, x, y, layer, width, height, 1, format, type, pixels);
}

void Texture::setCompressedData(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, GLsizei size, const void *data) {
    glCompressedTextureSubImage3D(textureArray->texture, level, x, y, layer,width, height, 1, format, size, data);
}

TextureArray::TextureArray(GLsizei mipmaplevels, GLenum format, GLsizei width, GLsizei height, GLsizei layercount)
    : width(width), height(height), nextLayer(0) {
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture);

    glTextureStorage3D(texture, mipmaplevels, format, width, height, layercount);

    glTextureParameteri(texture,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR );
    glTextureParameteri(texture,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTextureParameteri(texture,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
}

TextureArray::~TextureArray() {
    glDeleteTextures(1, &texture);
}

Texture TextureArray::getTexture() {
    GLuint layer = nextLayer++;
    return Texture(this, layer);
}

void TextureArray::bind(GLint unit) {
    glBindTextureUnit(unit, texture);
}


TextureArray &TextureGroup::getArray(GLsizei width, GLsizei height, GLsizei miplevels, GLenum format) {
    auto dim = std::make_tuple(width, height, miplevels, format);
    if(textureArrays.find(dim) == textureArrays.end()) {
        std::cout << "New Texture Array!" << std::endl;
        textureArrays.emplace(dim, std::make_unique<TextureArray>(miplevels, format, width, height, 32));
    }

    return *textureArrays[dim];
}
