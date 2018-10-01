//
// Created by felix on 30/09/2018.
//
#include <gtc/type_ptr.hpp>
#include "Renderer.h"
#include <fstream>

Renderer::Renderer() {

    std::ifstream vertFile("../defaultVertex.glsl");
    std::string vertexText((std::istreambuf_iterator<char>(vertFile)), (std::istreambuf_iterator<char>()));

    std::ifstream fragFile("../defaultFragment.glsl");
    std::string fragText((std::istreambuf_iterator<char>(fragFile)), (std::istreambuf_iterator<char>()));

    shader = new Shader(vertexText, fragText);

}

ModelBatch &Renderer::getBatch(MeshBuffer *const buffer, TextureArray* const array) {
        for(ModelBatch& b : batches) {
                if (b.meshBuffer == buffer && b.textureArray == array) {
                        return b;
                }
        }

        return batches.emplace_back(buffer, array);
}

void Renderer::clearBatches() {
        for(ModelBatch& b : batches) {
                b.commandCount = 0;
        }
}

void Renderer::submit(const Mesh &mesh, const Texture &texture, const glm::mat4 &transform) {
        ModelBatch& batch = getBatch(mesh.buffer, texture.textureArray);

        size_t commandId = batch.commandCount++;

        //Set command
        DrawElementsIndirectCommand& command = batch.commands[commandId];
        command.count = mesh.elementCount;
        command.instanceCount = 1;
        command.firstIndex = mesh.first;
        command.baseVertex = mesh.baseVertex;
        command.baseInstance = static_cast<GLuint>(commandId);

        //Set texture info
        batch.textureIndices[commandId] = texture.layer;

        //Set transform info
        batch.modelMatrices[commandId] = transform;
}

void Renderer::renderBatches() {

        shader->use();
        shader->setUniform(shader->getUniformLocation("tex"), 0);

        for(const ModelBatch& b : batches) {
                b.meshBuffer->bindVa();
                b.textureArray->bind(0);
                glFlushMappedNamedBufferRange(b.indirectBuffer, 0, b.commandCount * sizeof(DrawElementsIndirectCommand));
                glFlushMappedNamedBufferRange(b.textureIndexBuffer, 0, b.commandCount * sizeof(GLuint));
                glFlushMappedNamedBufferRange(b.modelMatrixBuffer, 0, b.commandCount * sizeof(glm::mat4));
                glBindBuffer(GL_DRAW_INDIRECT_BUFFER, b.indirectBuffer);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,  b.modelMatrixBuffer);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, b.textureIndexBuffer);

                glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, b.commandCount, 0);
        }

        clearBatches();
}

void Renderer::setProjection(const glm::mat4 &proj) {
        shader->setUniform(shader->getUniformLocation("MVP"), proj);
}

ModelBatch::ModelBatch(MeshBuffer*const buffer, TextureArray* const array) : meshBuffer(buffer), textureArray(array) {
        commandCount = 0;
        maxCommandCount = 1024;

        //Create buffers
        glCreateBuffers(3, bufferObjects);

        //Indirect buffer
        GLbitfield flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT  ;
        glNamedBufferStorage(indirectBuffer, maxCommandCount * sizeof(DrawElementsIndirectCommand), nullptr, flags);
        flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT ;
        commands = static_cast<DrawElementsIndirectCommand *>(glMapNamedBufferRange(indirectBuffer, 0, maxCommandCount * sizeof(DrawElementsIndirectCommand), flags));

        //texture index buffer
        flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT ;
        glNamedBufferStorage(textureIndexBuffer, maxCommandCount * sizeof(GLint), nullptr, flags);
        flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT ;
        textureIndices = static_cast<GLuint *>(glMapNamedBufferRange(textureIndexBuffer, 0, maxCommandCount * sizeof(GLuint), flags));

        //Transform buffers
        flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT ;
        glNamedBufferStorage(modelMatrixBuffer, maxCommandCount * sizeof(glm::mat4), nullptr, flags);
        flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT ;
        modelMatrices = static_cast<glm::mat4*>(glMapNamedBufferRange(modelMatrixBuffer, 0, maxCommandCount * sizeof(glm::mat4), flags));
}
