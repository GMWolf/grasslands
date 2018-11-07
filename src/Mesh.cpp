//
// Created by felix on 29/09/2018.
//

#include "Mesh.h"
#include <limits>

vertexData::vertexData(const glm::vec3 &p, const glm::vec3 &n, const glm::vec2 &t) {
    position = p;
    normal = n;
    texcoords[0] = t[0] * 0xFFFF;
    texcoords[1] = t[1] * 0xFFFF;
}

Mesh::Mesh(MeshBuffer* buffer, GLuint index, glm::vec3 bboxMin, glm::vec3 bboxMax) :
        buffer(buffer), index(index),bboxMin(bboxMin), bboxMax(bboxMax) {
}


MeshBuffer::MeshBuffer() : nextFirst(0), nextBaseVertex(0) {
    //Create gl objects
    glCreateBuffers(4, bufferObjects);
    glCreateVertexArrays(1, &vertexArray);

    //Setup storage
    GLint batchSize = MAX_BATCH_SIZE;
    GLuint* ids = new GLuint[batchSize];
    for(GLuint i = 0; i < batchSize; i++) {
        ids[i] = i;
    }
    glNamedBufferStorage(idBuffer, batchSize * sizeof(GLuint), ids, 0);
    delete[] ids;

    GLint vertexCount =  128 * 1024;
    glNamedBufferStorage(vertexBuffer, sizeof(vertexData) * vertexCount, nullptr, GL_DYNAMIC_STORAGE_BIT);

    GLint elementCount = 128 * 1024;
    glNamedBufferStorage(elementBuffer, elementCount * sizeof(GLushort), nullptr, GL_DYNAMIC_STORAGE_BIT);

    //Format
    glVertexArrayAttribIFormat(vertexArray, 0, 1, GL_UNSIGNED_INT, 0); //DRAW ID
    glVertexArrayAttribFormat(vertexArray, 1, 3, GL_HALF_FLOAT, GL_FALSE, offsetof(vertexData, position)); //Position
    glVertexArrayAttribFormat(vertexArray, 2, 4, GL_INT_2_10_10_10_REV, GL_TRUE, offsetof(vertexData, normal)); //normal
    glVertexArrayAttribFormat(vertexArray, 3, 2, GL_UNSIGNED_SHORT, GL_TRUE, offsetof(vertexData, texcoords)); //uv

    //divisors
    glVertexArrayBindingDivisor(vertexArray, 0, 1);

    //meshBuffer source
    glVertexArrayVertexBuffer(vertexArray, 0, idBuffer, 0, sizeof(GLuint));
    glVertexArrayVertexBuffer(vertexArray, 1, vertexBuffer, 0, sizeof(vertexData));

    //link attributes
    glVertexArrayAttribBinding(vertexArray, 0, 0);
    glVertexArrayAttribBinding(vertexArray, 1, 1);
    glVertexArrayAttribBinding(vertexArray, 2, 1);
    glVertexArrayAttribBinding(vertexArray, 3, 1);

    //enable attributes
    glEnableVertexArrayAttrib(vertexArray, 0);
    glEnableVertexArrayAttrib(vertexArray, 1);
    glEnableVertexArrayAttrib(vertexArray, 2);
    glEnableVertexArrayAttrib(vertexArray, 3);

    //set element meshBuffer
    glVertexArrayElementBuffer(vertexArray, elementBuffer);

    //setup mesh data
    GLuint meshCount = 10;
    glNamedBufferStorage(meshDataBuffer, meshCount * sizeof(MeshData), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

Mesh MeshBuffer::getMesh(const std::vector<vertexData> &vertices, const std::vector<GLushort> &elements) {


    GLuint index = nextMeshIndex++;
    GLuint first = nextFirst;
    GLuint baseVertex = nextBaseVertex;

    nextFirst += elements.size();
    nextBaseVertex += vertices.size();

    //Calculate bounding box
    auto bboxMax = glm::vec3(-std::numeric_limits<float>::max());
    auto bboxMin = glm::vec3(std::numeric_limits<float>::max());
    //Calculate bboxN
    for(const vertexData& v : vertices) {
        glm::vec3 pos = v.position;
        bboxMax = glm::max(bboxMax, pos);
        bboxMin = glm::min(bboxMin, pos);
    }

    //Write mesh data
    MeshData md(first, elements.size(), baseVertex, bboxMin, bboxMax);
    glNamedBufferSubData(meshDataBuffer, index * sizeof(MeshData), sizeof(MeshData), &md);
    //Write vertex and element data
    glNamedBufferSubData(vertexBuffer, baseVertex * sizeof(vertexData), vertices.size() * sizeof(vertexData), vertices.data());
    glNamedBufferSubData(elementBuffer, first * sizeof(GLushort), elements.size() * sizeof(GLushort), elements.data());

    return Mesh(this, index, bboxMin, bboxMax);
}

void MeshBuffer::bindVa() const {
    glBindVertexArray(vertexArray);
}

MeshBuffer::~MeshBuffer() {
    glDeleteBuffers(4, bufferObjects);
    glDeleteVertexArrays(1, &vertexArray);
}
