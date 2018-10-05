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

    std::cout << "New Shader" << std::endl;
    shader = new Shader(vertexText, fragText);

}


void Renderer::submit(const Mesh &mesh, const Texture &texture, const Transform &transform) {

    size_t commandId = batch.commandCount++;

    //Set command
    DrawElementsIndirectCommand& command = batch.commands[batch.bufferIndex][commandId];
    command.count = mesh.elementCount;
    command.instanceCount = 1;
    command.firstIndex = mesh.first;
    command.baseVertex = mesh.baseVertex;
    command.baseInstance = static_cast<GLuint>(batch.bufferIndex * batch.bufferSize + batch.commandCount);


    //Set texture info
    batch.textureIndices[batch.bufferIndex][batch.commandCount] = texture.layer;

    //Set transform info
    batch.modelMatrices[batch.bufferIndex][batch.commandCount] = transform;

    if (batch.commandCount >= batch.bufferSize) {
        renderbatch(batch);
    }
}

void Renderer::flushBatches() {


    GLuint index = batch.bufferIndex;
    renderbatch(batch);
    //Wait for last batch to end
    GLenum timeoutflag;
    int n = 0;
    do {
        //std::cout << "Waiting for last fence " << ++n << std::endl;
        timeoutflag = glClientWaitSync(batch.fences[index], GL_SYNC_FLUSH_COMMANDS_BIT, 1000);
    } while(timeoutflag == GL_TIMEOUT_EXPIRED);


}

void Renderer::setProjection(const glm::mat4 &proj) {
        shader->setUniform(shader->getUniformLocation("MVP"), proj);
}

void Renderer::renderbatch(Batch &batch) {

    if (batch.commandCount > 0) {
       // std::cout << "buffer index: " << batch.bufferIndex <<"\n";
        shader->use();
        shader->setUniform(shader->getUniformLocation("tex"), 0);

        glFlushMappedNamedBufferRange(batch.indirectBuffer, batch.bufferIndex * batch.bufferSize * sizeof(DrawElementsIndirectCommand) , batch.commandCount * sizeof(DrawElementsIndirectCommand));
        glFlushMappedNamedBufferRange(batch.textureIndexBuffer, batch.bufferIndex * batch.bufferSize * sizeof(GLuint), batch.commandCount * sizeof(GLuint));
        glFlushMappedNamedBufferRange(batch.modelMatrixBuffer, batch.bufferIndex * batch.bufferSize * sizeof(Transform), batch.commandCount * sizeof(Transform));
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, batch.indirectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.modelMatrixBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.textureIndexBuffer);

        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT,
                                    reinterpret_cast<const void *>(batch.bufferIndex * batch.bufferSize * sizeof(DrawElementsIndirectCommand)),
                                    batch.commandCount, 0);

        batch.fences[batch.bufferIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);


        batch.commandCount = 0;
        batch.bufferIndex = (batch.bufferIndex + 1) % batch.bufferCount;

        //Wait on next batch to finish
        int n = 0;
        if (batch.fences[batch.bufferIndex]) {
            GLenum timeoutflag;
            do {
                timeoutflag = glClientWaitSync(batch.fences[batch.bufferIndex], GL_SYNC_FLUSH_COMMANDS_BIT, 1000);
                if(timeoutflag != GL_ALREADY_SIGNALED) {
                    //std::cout << "Waiting on fence " << batch.bufferIndex << " " << (++n) << "ms\n";
                }
            } while (timeoutflag == GL_TIMEOUT_EXPIRED);

        }

    }
}

void Renderer::submit(const RenderObject &robj) {
    submit(robj.mesh, robj.texture, robj.transform);
}

Batch::Batch(){
    bufferIndex= 0;
    commandCount = 0;

    //Create buffers
    glCreateBuffers(3, bufferObjects);

    //Indirect buffer
    GLbitfield flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT  | GL_MAP_COHERENT_BIT ;
    glNamedBufferStorage(indirectBuffer, bufferCount * bufferSize * sizeof(DrawElementsIndirectCommand), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    commands[0] = static_cast<DrawElementsIndirectCommand *>(glMapNamedBufferRange(indirectBuffer, 0, bufferCount * bufferSize * sizeof(DrawElementsIndirectCommand), flags));

    //texture index buffer
    flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ;
    glNamedBufferStorage(textureIndexBuffer, bufferCount * bufferSize * sizeof(GLint), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    textureIndices[0] = static_cast<GLuint *>(glMapNamedBufferRange(textureIndexBuffer, 0 , bufferCount * bufferSize * sizeof(GLuint), flags));

    //Transform buffers
    flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ;
    glNamedBufferStorage(modelMatrixBuffer, bufferCount * bufferSize * sizeof(Transform), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    modelMatrices[0] = static_cast<Transform*>(glMapNamedBufferRange(modelMatrixBuffer, 0, bufferCount *  bufferSize * sizeof(Transform), flags));


    //Get offset mappings
    for(int i = 1; i < bufferCount; i++) {
        commands[i] = commands[0] + (bufferSize * i);
        textureIndices[i] = textureIndices[0] + (bufferSize  * i);
        modelMatrices[i] = modelMatrices[0] + (bufferSize  * i);
    }

    //Set fences to nullptr
    for (auto &fence : fences) {
        fence = nullptr;
    }

}

Batch::~Batch() {
    glUnmapNamedBuffer(indirectBuffer);
    glUnmapNamedBuffer(textureIndexBuffer);
    glUnmapNamedBuffer(modelMatrixBuffer);

    glDeleteBuffers(3, bufferObjects);
}
