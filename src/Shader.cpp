//
// Created by felix on 27/08/2018.
//

#include "Shader.h"
#include <algorithm>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>

Shader::Shader(std::map<GLenum, std::string> sources) {

    program = glCreateProgram();

    for(auto& e : sources) {
        GLuint sh = glCreateShader(e.first);
        const char* c_source = e.second.c_str();
        glShaderSource(sh, 1, &c_source, NULL);
        glCompileShader(sh);
        testShader(sh);
        glAttachShader(program, sh);
        glDeleteShader(sh); //Flag for deletion
    }

    glLinkProgram(program);
    compiled = testLink(program);
}

bool Shader::testShader(GLuint shader) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        std::cout << "Could not compile shader" << std::endl;
        char error[2048];
        glGetShaderInfoLog(shader, sizeof(error), nullptr, error);
        std::cout << error << std::endl;
        return false;
    }
    return true;
}


bool Shader::testLink(GLuint program) {
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        std::cout << "Could not link shader" << std::endl;
        char error[2048];
        glGetProgramInfoLog(program, sizeof(error), nullptr, error);
        std::cout << error << std::endl;
        return false;
    }
    return true;
}

void Shader::use() const{
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
void Shader::setUniform(GLint location, const bool &value) {
    glProgramUniform1i(program, location, value);
}


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
void Shader::setUniform(GLint location,  const glm::vec2& value) {
    glProgramUniform2f(program, location, value.x, value.y);
}

template<>
void Shader::setUniform(GLint location, const glm::mat4 &value) {
    glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, value_ptr(value));
}

template<>
void Shader::setUniform(GLint location, const std::vector<unsigned int> &value) {
    glProgramUniform1uiv(program, location, value.size(), value.data());
}

template<>
void Shader::setUniform(GLint location, const std::vector<int> &value) {
    glProgramUniform1iv(program, location, value.size(), value.data());
}

template<>
void Shader::setUniform(GLint location, const glm::ivec2 &value) {
    glProgramUniform2i(program, location, value.x, value.y);
}

//endregion