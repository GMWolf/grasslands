//
// Created by felix on 27/08/2018.
//

#include "Shader.h"
#include <algorithm>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

Shader::Shader(const std::string &vertexSource, const std::string &fragmentSource) {
    const char* c_vertexSource = vertexSource.c_str();
    const char* c_fragmentSource = fragmentSource.c_str();

    GLuint vertexShader, fragmentShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &c_vertexSource, NULL);
    glCompileShader(vertexShader);
    testShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &c_fragmentSource, NULL);
    glCompileShader(fragmentShader);
    testShader(fragmentShader);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    testLink(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::testShader(GLuint shader) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        std::cout << "Could not compile shader" << std::endl;
        char error[2048];
        glGetShaderInfoLog(shader, sizeof(error), nullptr, error);
        std::cout << error << std::endl;
    }
}


void Shader::testLink(GLuint program) {
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        std::cout << "Could not link shader" << std::endl;
        char error[2048];
        glGetProgramInfoLog(program, sizeof(error), nullptr, error);
        std::cout << error << std::endl;
    }
}

void Shader::use() {
    glUseProgram(program);
}

GLint Shader::getUniformLocation(const std::string &name) {
    return glGetUniformLocation(program, name.c_str());
}

void Shader::release() {
    glDeleteProgram(program);
    program = 0;
}

Shader::~Shader() {
    release();
}

Shader::Shader(Shader &&other) noexcept : program(other.program){
    other.program = 0;
}

Shader &Shader::operator=(Shader &&other) noexcept {
    if (this != &other) {
        release();
        std::swap(program, other.program);
    }
    return *this;
}



//region uniform specializations

template<>
void Shader::setUniform(GLint location, const int &value) {
    glProgramUniform1i(program, location, value);
}

template<>
void Shader::setUniform(GLint location, const float& value) {
    glProgramUniform1f(program, location, value);
}

template<>
void Shader::setUniform(GLint location, const unsigned int& value) {
    glProgramUniform1ui(program, location, value);
}

template<>
void Shader::setUniform(GLint location,  const glm::vec3& value) {
    glProgramUniform3f(program, location, value.x, value.y, value.z);
}

template<>
void Shader::setUniform(GLint location, const glm::mat4 &value) {
    glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, value_ptr(value));
}

//endregion