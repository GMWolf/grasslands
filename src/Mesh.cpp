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

Mesh::Mesh(MeshBuffer* buffer, GLuint index, GLint first, GLint elementCount, GLint baseVertex, GLint vertexCount) :
        buffer(buffer), index(index), first(first), elementCount(elementCount), baseVertex(baseVertex), vertexCount(vertexCount) {
}

void Mesh::setVertexData(const std::vector<vertexData>& data) {
    glNamedBufferSubData(buffer->vertexBuffer, baseVertex * sizeof(vertexData), data.size() * sizeof(vertexData), data.data());

    const float inf = std::numeric_limits<float>::infinity();
    const float ninf = -inf;
    bboxMax = glm::vec3(ninf, ninf, ninf);
    bboxMin = glm::vec3(inf, inf, inf);
    //Calculate bboxN
    for(const vertexData& v : data) {
        glm::vec3 pos = v.position;
        bboxMax = glm::max(bboxMax, pos);
        bboxMin = glm::min(bboxMin, pos);
    }
}

void Mesh::setElementData(const std::vector<GLushort> &elements) {
    glNamedBufferSubData(buffer->elementBuffer, first * sizeof(GLushort), elements.size() * sizeof(GLushort), elements.data());
}

MeshBuffer::MeshBuffer() : nextFirst(0), nextBaseVertex(0) {
    //Create gl objects
    glCreateBuffers(4, bufferObjects);
    glCreateVertexArrays(1, &vertexArray);

    //Setup storage
    GLint batchSize = 2048*3;
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

Mesh MeshBuffer::getMesh(GLint elementCount, GLint vertexCount) {
    GLuint index = nextMeshIndex++;

    GLint first = nextFirst;
    GLint baseVertex = nextBaseVertex;

    nextFirst += elementCount;
    nextBaseVertex += vertexCount;

    MeshData md(first, elementCount, baseVertex, glm::vec3(0,0,0), glm::vec3(0,0,0));

    glNamedBufferSubData(meshDataBuffer, index * sizeof(MeshData), sizeof(MeshData), &md);

    return Mesh(this, index, first, elementCount, baseVertex, vertexCount);
}

Mesh MeshBuffer::getMesh(const std::vector<vertexData> &vertexData, const std::vector<GLushort> &elements) {

    Mesh m = getMesh(static_cast<GLint>(elements.size()), static_cast<GLint>(vertexData.size()));
    m.setVertexData(vertexData);
    m.setElementData(elements);

    return m;
}

void MeshBuffer::bindVa() const {
    glBindVertexArray(vertexArray);
}

MeshBuffer::~MeshBuffer() {
    glDeleteBuffers(4, bufferObjects);
    glDeleteVertexArrays(1, &vertexArray);
}
