//
// Created by felix on 29/09/2018.
//

#include "Mesh.h"

vertexData::vertexData(const glm::vec3 &p, const glm::vec3 &n, const glm::vec2 &t) {
    position[0] = p[0];
    position[1] = p[1];
    position[2] = p[2];
    normal = n;
    texcoords[0] = t[0] * 0xFFFF;
    texcoords[1] = t[1] * 0xFFFF;
}

Mesh::Mesh(MeshBuffer* buffer, GLint first, GLint elementCount, GLint baseVertex) :
        buffer(buffer), first(first), elementCount(elementCount), baseVertex(baseVertex) {
}

void Mesh::setVertexData(const std::vector<vertexData>& data) {
    glNamedBufferSubData(buffer->vertexBuffer, baseVertex * sizeof(vertexData), data.size() * sizeof(vertexData), data.data());
}

void Mesh::setElementData(const std::vector<GLushort> &elements) {
    glNamedBufferSubData(buffer->elementBuffer, first * sizeof(GLushort), elements.size() * sizeof(GLushort), elements.data());
}

MeshBuffer::MeshBuffer() : nextFirst(0), nextBaseVertex(0) {
    //Create gl objects
    glCreateBuffers(2, bufferObjects);
    glCreateVertexArrays(1, &vertexArray);

    //Setup storage
    GLint vertexCount =  128 * 1024;
    glNamedBufferStorage(vertexBuffer, sizeof(vertexData) * vertexCount, nullptr, GL_DYNAMIC_STORAGE_BIT);

    GLint elementCount = 128 * 1024;
    glNamedBufferStorage(elementBuffer, elementCount * sizeof(GLushort), nullptr, GL_DYNAMIC_STORAGE_BIT);

    //Format
    glVertexArrayAttribFormat(vertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(vertexData, position));
    glVertexArrayAttribFormat(vertexArray, 1, 4, GL_INT_2_10_10_10_REV, GL_TRUE, offsetof(vertexData, normal));
    glVertexArrayAttribFormat(vertexArray, 2, 2, GL_UNSIGNED_SHORT, GL_TRUE, offsetof(vertexData, texcoords));

    //meshBuffer source
    glVertexArrayVertexBuffer(vertexArray, 0, vertexBuffer, 0, sizeof(vertexData));

    //link attributes
    glVertexArrayAttribBinding(vertexArray, 0, 0);
    glVertexArrayAttribBinding(vertexArray, 1, 0);
    glVertexArrayAttribBinding(vertexArray, 2, 0);

    //enable attributes
    glEnableVertexArrayAttrib(vertexArray, 0);
    glEnableVertexArrayAttrib(vertexArray, 1);
    glEnableVertexArrayAttrib(vertexArray, 2);

    //set element meshBuffer
    glVertexArrayElementBuffer(vertexArray, elementBuffer);
}

Mesh MeshBuffer::getMesh(GLint elementCount, GLint vertexCount) {

    GLint first = nextFirst;
    GLint baseVertex = nextBaseVertex;

    nextFirst += elementCount;
    nextBaseVertex += vertexCount;


    return Mesh(this, first, elementCount, baseVertex);
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
