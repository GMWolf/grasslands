//
// Created by felix on 30/09/2018.
//

#ifndef GRASSLANDS_TEXTURE_H
#define GRASSLANDS_TEXTURE_H

#include <glad/glad.h>
#include <map>
#include <tuple>
#include <memory>
#include <vector>
#include "glm.hpp"

class TextureArray;
class Texture {
    friend class TextureArray;

public:
    GLuint layer;
    TextureArray* const textureArray;

    void setData(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, GLenum type, const void * pixels, int face = 0);
    void setCompressedData(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, GLsizei size, const void * data, int face = 0);

    operator glm::ivec2() const;

private:
    Texture(TextureArray* textureArray, GLuint layer);
};


class TextureArray {
    friend class Texture;
public:
    TextureArray(GLenum target, GLuint unit, GLsizei mipmaplevels, GLenum format, GLsizei width, GLsizei height, GLsizei layercount);
    ~TextureArray();

    Texture getTexture();

    void bind();

    const GLsizei width;
    const GLsizei height;

    const GLuint unit;
    const GLenum target;
private:

    GLuint nextLayer = 0;

    GLuint texture;
};


class TextureGroup {

public:

    TextureArray& getArray(GLenum target,GLsizei width, GLsizei height, GLsizei miplevels, GLenum format);
    void bind();
private:
    GLuint nextUnit = 0;
    //tuple goes: target, width, height, mipmaplevel, format
    std::map<std::tuple<GLenum, GLsizei, GLsizei, GLsizei, GLenum >, std::unique_ptr<TextureArray>> textureArrays;
};



#endif //GRASSLANDS_TEXTURE_H
