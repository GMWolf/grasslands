//
// Created by felix on 30/09/2018.
//

#ifndef GRASSLANDS_TEXTURE_H
#define GRASSLANDS_TEXTURE_H

#include <glad/glad.h>
#include <map>
#include <tuple>
#include <memory>

class TextureArray;
class Texture {
    friend class TextureArray;

public:
    GLuint layer;
    TextureArray* const textureArray;

    void setData(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, GLenum type, const void * pixels);
    void setCompressedData(GLint level, GLenum format, GLint x, GLint y, GLsizei width, GLsizei height, GLsizei size, const void * data);

private:
    Texture(TextureArray* textureArray, GLuint layer);
};


class TextureArray {
    friend class Texture;
public:
    TextureArray(GLsizei mipmaplevels, GLenum format, GLsizei width, GLsizei height, GLsizei layercount);
    ~TextureArray();

    Texture getTexture();

    void bind(GLint unit);

    const GLsizei width;
    const GLsizei height;

private:

    GLuint nextLayer = 0;

    GLuint texture;
};


class TextureGroup {

public:

    TextureArray& getArray(GLsizei width, GLsizei height, GLsizei miplevels, GLenum format);


private:

    //tuple goes: width, height, mipmaplevel, format
    std::map<std::tuple<GLsizei, GLsizei, GLsizei, GLenum >, std::unique_ptr<TextureArray>> textureArrays;
};



#endif //GRASSLANDS_TEXTURE_H
