//
// Created by felix on 27/08/2018.
//

#ifndef WAGLE2_SHADER_H
#define WAGLE2_SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <map>

class Shader {
public:

    Shader(std::map<GLenum, std::string> sources);

    ~Shader();

    void release();

    Shader(const Shader &) = delete;

    Shader &operator=(const Shader &) = delete;

    Shader(Shader &&other) noexcept;

    Shader &operator=(Shader &&other) noexcept;

    void use();

    GLint getUniformLocation(const std::string &name);

    template<class T>
    void setUniform(GLint location,const T& value);

    operator bool() {
        return compiled;
    }

private:
    static bool testShader(GLuint shader);

    static bool testLink(GLuint program);
    GLuint program;

    bool compiled;

};


#endif //WAGLE2_SHADER_H
