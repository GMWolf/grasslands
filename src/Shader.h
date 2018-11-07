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
#include <fstream>

class Shader {
public:

    Shader(std::map<GLenum, std::string> sources);

    ~Shader();

    void release();

    Shader(const Shader &) = delete;

    Shader &operator=(const Shader &) = delete;

    Shader(Shader &&other) noexcept;

    Shader &operator=(Shader &&other) noexcept;

    void use() const;

    GLint getUniformLocation(const std::string &name);

    template<class T>
    void setUniform(GLint location, const T& value);

    operator bool() {
        return compiled;
    }

private:
    static bool testShader(GLuint shader);

    static bool testLink(GLuint program);
    GLuint program;

    bool compiled;

};


inline std::string operator "" _read(const char* p, size_t l) {
    std::ifstream file(std::string(p, l));
    std::string text((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    return text;
}


#endif //WAGLE2_SHADER_H
