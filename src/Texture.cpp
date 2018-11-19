//
// Created by felix on 30/09/2018.
//

#include <iostream>
#include "Texture.h"


Texture::Texture(TextureArray *textureArray, GLuint layer) : textureArray(textureArray), layer(layer) {
}

void Texture::setData(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, GLenum type, const void * pixels, int face) {

    int facelayer = (textureArray->target == GL_TEXTURE_CUBE_MAP_ARRAY) ? (layer * 6 + face) : layer;
    glTextureSubImage3D(textureArray->texture, level, x, y, facelayer, width, height, 1, format, type, pixels);
}

void Texture::setCompressedData(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, GLsizei size, const void *data, int face) {
    int facelayer = (textureArray->target == GL_TEXTURE_CUBE_MAP_ARRAY) ? (layer * 6 + face) : layer;
    glCompressedTextureSubImage3D(textureArray->texture, level, x, y, facelayer, width, height, 1, format, size, data);
}

Texture::operator glm::ivec2() const{
    return {textureArray->unit, layer};
}

TextureArray::TextureArray(GLenum target, GLuint unit, GLsizei mipmaplevels, GLenum format, GLsizei width, GLsizei height, GLsizei layercount)
    : target(target), unit(unit), width(width), height(height), nextLayer(0) {
    glCreateTextures(target, 1, &texture);

    glTextureStorage3D(texture, mipmaplevels, format, width, height, layercount);

    //glTextureParameter
    glTextureParameterf(texture, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
    glTextureParameteri(texture,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texture,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTextureParameteri(texture,GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture,GL_TEXTURE_WRAP_T, GL_REPEAT);
}

TextureArray::~TextureArray() {
    glDeleteTextures(1, &texture);
}

Texture TextureArray::getTexture() {
    GLuint layer = nextLayer++;
    return Texture(this, layer);
}

void TextureArray::bind() {
    //std::cout << unit << std::endl;
    glBindTextureUnit(unit, texture);
}

TextureArray &TextureGroup::getArray(GLenum target, GLsizei width, GLsizei height, GLsizei miplevels, GLenum format) {
    auto dim = std::make_tuple(target, width, height, miplevels, format);
    if(textureArrays.find(dim) == textureArrays.end()) {
        std::cout << "New Texture Array!" << std::endl;
        textureArrays.emplace(dim, std::make_unique<TextureArray>(target, nextUnit++, miplevels, format, width, height, 60));
    }

    return *textureArrays[dim];
}

void TextureGroup::bind() {
    for(auto& e : textureArrays) {
        e.second->bind();
    }
}
