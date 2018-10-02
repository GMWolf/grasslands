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
                if (b.meshBuffer == buffer &&
                b.textureArray == array &&
                b.commandCount < b.maxCommandCount) {
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

void Renderer::submit(const Mesh &mesh, const Texture &texture, const Transform &transform) {

    //TODO tripple buffering of buffers.

    ModelBatch& batch = getBatch(mesh.buffer, texture.textureArray);

    bool addedToPrevious = false;
    //Try to append to previous commands
    if (batch.commandCount > 0) {
        auto &previousCommand = batch.commands[batch.commandCount - 1];
        if (previousCommand.firstIndex == mesh.first) {
              //Add to previous command
              previousCommand.instanceCount++;
              addedToPrevious = true;
        }
    }

    //Create new command
    if (!addedToPrevious) {
        size_t commandId = batch.commandCount++;

        //Set command
        DrawElementsIndirectCommand& command = batch.commands[commandId];
        command.count = mesh.elementCount;
        command.instanceCount = 1;
        command.firstIndex = mesh.first;
        command.baseVertex = mesh.baseVertex;
        command.baseInstance = static_cast<GLuint>(batch.instanceCount);
    }

    //Set texture info
    batch.textureIndices[batch.instanceCount] = texture.layer;

    //Set transform info
    batch.modelMatrices[batch.instanceCount] = transform;

    batch.instanceCount++;
    /*if (batch.commandCount >= batch.maxCommandCount-1) {
    renderbatch(batch);
    }*/
}

void Renderer::flushBatches() {

        shader->use();
        shader->setUniform(shader->getUniformLocation("tex"), 0);

        for(ModelBatch& batch : batches) {
                renderbatch(batch);
        }

}

void Renderer::setProjection(const glm::mat4 &proj) {
        shader->setUniform(shader->getUniformLocation("MVP"), proj);
}

void Renderer::renderbatch(ModelBatch &batch) {

    shader->use();
    shader->setUniform(shader->getUniformLocation("tex"), 0);

    batch.meshBuffer->bindVa();
    batch.textureArray->bind(0);
    /*glFlushMappedNamedBufferRange(batch.indirectBuffer, 0, batch.commandCount * sizeof(DrawElementsIndirectCommand));
    glFlushMappedNamedBufferRange(batch.textureIndexBuffer, 0, batch.commandCount * sizeof(GLuint));
    glFlushMappedNamedBufferRange(batch.modelMatrixBuffer, 0, batch.commandCount * sizeof(glm::mat4));*/
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, batch.indirectBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,  batch.modelMatrixBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.textureIndexBuffer);

    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, batch.commandCount, 0);

    batch.commandCount = 0;
    batch.instanceCount = 0;
}

ModelBatch::ModelBatch(MeshBuffer*const buffer, TextureArray* const array) : meshBuffer(buffer), textureArray(array) {
        instanceCount = 0;
        commandCount = 0;
        maxCommandCount = 1024;

        //Create buffers
        glCreateBuffers(3, bufferObjects);

        //Indirect buffer
        GLbitfield flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT  | GL_MAP_COHERENT_BIT ;
        glNamedBufferStorage(indirectBuffer, maxCommandCount * sizeof(DrawElementsIndirectCommand), nullptr, flags);
        flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | /*GL_MAP_FLUSH_EXPLICIT_BIT |*/ GL_MAP_COHERENT_BIT ;
        commands = static_cast<DrawElementsIndirectCommand *>(glMapNamedBufferRange(indirectBuffer, 0, maxCommandCount * sizeof(DrawElementsIndirectCommand), flags));

        //texture index buffer
        flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ;
        glNamedBufferStorage(textureIndexBuffer, maxCommandCount * sizeof(GLint), nullptr, flags);
        flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | /*GL_MAP_FLUSH_EXPLICIT_BIT |*/ GL_MAP_COHERENT_BIT ;
        textureIndices = static_cast<GLuint *>(glMapNamedBufferRange(textureIndexBuffer, 0, maxCommandCount * sizeof(GLuint), flags));

        //Transform buffers
        flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ;
        glNamedBufferStorage(modelMatrixBuffer, maxCommandCount * sizeof(Transform), nullptr, flags);
        flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;// GL_MAP_FLUSH_EXPLICIT_BIT ;
        modelMatrices = static_cast<Transform*>(glMapNamedBufferRange(modelMatrixBuffer, 0, maxCommandCount * sizeof(Transform), flags));
}
