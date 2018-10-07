//
// Created by felix on 29/09/2018.
//

#ifndef GRASSLANDS_MESHBUFFER_H
#define GRASSLANDS_MESHBUFFER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include <vector>
#include "glTypes.h"

struct vertexData {
    vertexData(const glm::vec3& p, const glm::vec3& n, const glm::vec2& t);


    Vec3Half position;
    GLhalf pad; //16 * 4 -> 64 //padding for efficiency
    Vec3Int normal; //32
    GLushort texcoords[2]; //16 * 2 -> 32

};

struct MeshData {

    MeshData(GLuint first, GLuint elementCount, GLuint baseVertex, glm::vec3 bboxMin, glm::vec3 bboxMax) :
    first(first), elementCount(elementCount), baseVertex(baseVertex)/*, bboxMin(bboxMin), bboxMax(bboxMax)*/ {
    }

    GLuint first;
    GLuint elementCount;
    GLuint baseVertex;
    /*glm::vec3 bboxMin;
    glm::vec3 bboxMax;*/
};

class MeshBuffer;
class Mesh {
    friend class MeshBuffer;
public:

    GLuint index;

    GLuint first;
    GLuint elementCount;
    GLuint baseVertex;
    GLuint vertexCount;

    glm::vec3 bboxMin, bboxMax;

    MeshBuffer* const buffer;

    void setVertexData(const std::vector<vertexData>& data);
    void setElementData(const std::vector<GLushort>& elements);

private:
    Mesh(MeshBuffer* buffer, GLuint index, GLint first, GLint elementCount, GLint baseVertex, GLint vertexCount);
};


class MeshBuffer {
    friend class Mesh;
    friend class Renderer;
public:

    MeshBuffer();
    ~MeshBuffer();

    Mesh getMesh(GLint elementCount, GLint vertexCount);
    Mesh getMesh(const std::vector<vertexData>& vertexData, const std::vector<GLushort>& elements);

    void bindVa() const;

private:

    GLint nextFirst = 0;
    GLint nextBaseVertex = 0;
    GLuint nextMeshIndex = 0;


    GLuint vertexArray;

    union {
        GLuint bufferObjects[4];
        struct {
            GLuint idBuffer;
            GLuint vertexBuffer;
            GLuint elementBuffer;
            GLuint meshDataBuffer;
        };
    };

};




#endif //GRASSLANDS_MESHBUFFER_H
