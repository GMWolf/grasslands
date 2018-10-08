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

    shader = new Shader({
        {GL_VERTEX_SHADER, vertexText},
        {GL_FRAGMENT_SHADER, fragText}
    });


    std::ifstream dispatchFile("../dispatchGeom.glsl");
    std::string dispatchText((std::istreambuf_iterator<char>(dispatchFile)), (std::istreambuf_iterator<char>()));

    dispatchCompute = new Shader({
        {GL_COMPUTE_SHADER, dispatchText}
    });

}


void Renderer::submit(const Mesh &mesh, const Texture &texture, const Transform &transform) {

    batch.mesh = &mesh;

    //Check fence
    if (batch.fences[batch.bufferIndex]) {
        GLenum timeoutflag;
        do {
            timeoutflag = glClientWaitSync(batch.fences[batch.bufferIndex], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
        } while (timeoutflag == GL_TIMEOUT_EXPIRED);

        batch.fences[batch.bufferIndex] = nullptr;
    }


    size_t commandId = batch.commandCount++;

    //Set command
    batch.commands[batch.bufferIndex][commandId].meshIndex = mesh.index;


    //Set texture info
    batch.textureIndices[batch.bufferIndex][commandId] = texture.layer;

    //Set transform info
    batch.transforms[batch.bufferIndex][commandId] = transform;

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
        timeoutflag = glClientWaitSync(batch.fences[index], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
    } while(timeoutflag == GL_TIMEOUT_EXPIRED);


}

void Renderer::setProjection(const glm::mat4 &proj) {
        shader->setUniform(shader->getUniformLocation("MVP"), proj);
    dispatchCompute->setUniform(1, proj);
    viewproj = proj;
}

void Renderer::renderbatch(Batch &batch) {

    if (batch.commandCount > 0) {

        glFlushMappedNamedBufferRange(batch.computeCommandsBuffer, batch.bufferIndex * batch.bufferSize * sizeof(DrawElementsIndirectCommand) , batch.commandCount * sizeof(DrawElementsIndirectCommand));
        glFlushMappedNamedBufferRange(batch.textureIndexBuffer, batch.bufferIndex * batch.bufferSize * sizeof(GLuint), batch.commandCount * sizeof(GLuint));
        glFlushMappedNamedBufferRange(batch.transformBuffer, batch.bufferIndex * batch.bufferSize * sizeof(Transform), batch.commandCount * sizeof(Transform));

        dispatchCompute->use();
        dispatchCompute->setUniform(0, batch.bufferIndex * batch.bufferSize); //offset

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.mesh->buffer->meshDataBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.computeCommandsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.indirectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, batch.transformBuffer);
        glDispatchCompute(batch.commandCount, 1, 1);

        glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

        shader->use();
        shader->setUniform(shader->getUniformLocation("tex"), 0);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, batch.indirectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.transformBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.textureIndexBuffer);



        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT,
                                    reinterpret_cast<const void *>(batch.bufferIndex * batch.bufferSize * sizeof(DrawElementsIndirectCommand)),
                                    batch.commandCount, 0);

        batch.fences[batch.bufferIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);


        batch.commandCount = 0;
        batch.bufferIndex = (batch.bufferIndex + 1) % batch.bufferCount;

    }
}

void Renderer::submit(const RenderObject &robj) {
    submit(robj.mesh, robj.texture, robj.transform);
}

void Renderer::submit(const std::vector<RenderObject>& objs) {
    for(const RenderObject& obj : objs) {
        submit(obj);
    }
}

Batch::Batch(){
    bufferIndex= 0;
    commandCount = 0;

    //Create buffers
    glCreateBuffers(4, bufferObjects);

    //Command buffer
    GLbitfield flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT  | GL_MAP_COHERENT_BIT ;
    glNamedBufferStorage(computeCommandsBuffer, bufferCount * bufferSize * sizeof(DrawElementsIndirectCommand), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    commands[0] = static_cast<ComputeDispatchCommand *>(glMapNamedBufferRange(computeCommandsBuffer, 0, bufferCount * bufferSize * sizeof(DrawElementsIndirectCommand), flags));


    //Indirect buffer
    flags = 0;
    glNamedBufferStorage(indirectBuffer, bufferCount * bufferSize * sizeof(DrawElementsIndirectCommand), nullptr, flags);


    //texture index buffer
    flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ;
    glNamedBufferStorage(textureIndexBuffer, bufferCount * bufferSize * sizeof(GLint), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    textureIndices[0] = static_cast<GLuint *>(glMapNamedBufferRange(textureIndexBuffer, 0 , bufferCount * bufferSize * sizeof(GLuint), flags));

    //Transform buffers
    flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ;
    glNamedBufferStorage(transformBuffer, bufferCount * bufferSize * sizeof(Transform), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    transforms[0] = static_cast<Transform*>(glMapNamedBufferRange(transformBuffer, 0, bufferCount *  bufferSize * sizeof(Transform), flags));


    //Get offset mappings
    for(int i = 1; i < bufferCount; i++) {
        commands[i] = commands[0] + (bufferSize * i);
        textureIndices[i] = textureIndices[0] + (bufferSize  * i);
        transforms[i] = transforms[0] + (bufferSize  * i);
    }

    //Set fences to nullptr
    for (auto &fence : fences) {
        fence = nullptr;
    }

}

Batch::~Batch() {
    glUnmapNamedBuffer(indirectBuffer);
    glUnmapNamedBuffer(textureIndexBuffer);
    glUnmapNamedBuffer(transformBuffer);

    glDeleteBuffers(3, bufferObjects);
}
