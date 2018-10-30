//
// Created by felix on 30/09/2018.
//
#include <gtc/type_ptr.hpp>
#include "Renderer.h"
#include <fstream>
#include "Material.h"


Renderer::Renderer() {


    std::ifstream vertFile("../shaders/BasicVertex.glsl");
    std::string vertexText((std::istreambuf_iterator<char>(vertFile)), (std::istreambuf_iterator<char>()));

    //std::ifstream vertFile("../defaultVertex.glsl");
    //std::string vertexText((std::istreambuf_iterator<char>(vertFile)), (std::istreambuf_iterator<char>()));

    std::ifstream fragFile("../shaders/defaultFragment.glsl");
    std::string fragText((std::istreambuf_iterator<char>(fragFile)), (std::istreambuf_iterator<char>()));

    std::ifstream contFile("../shaders/defaultTessellateControl.glsl");
    std::string contText((std::istreambuf_iterator<char>(contFile)), (std::istreambuf_iterator<char>()));

    std::ifstream evalFile("../shaders/defaultTessellateEvaluation.glsl");
    std::string evalText((std::istreambuf_iterator<char>(evalFile)), (std::istreambuf_iterator<char>()));

    shader = new Shader({
        {GL_VERTEX_SHADER, vertexText},
        {GL_TESS_CONTROL_SHADER, contText},
        {GL_TESS_EVALUATION_SHADER, evalText},
        {GL_FRAGMENT_SHADER, fragText}
    });


    std::ifstream dispatchFile("../shaders/dispatchGeom.glsl");
    std::string dispatchText((std::istreambuf_iterator<char>(dispatchFile)), (std::istreambuf_iterator<char>()));

    dispatchCompute = new Shader({
        {GL_COMPUTE_SHADER, dispatchText}
    });

}

/*
void Renderer::submit(const Mesh &mesh, const Material& material, const Transform &transform) {

    batch.meshBuffer = mesh.buffer;
    batch.materialArray = material.array;

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
    batch.materialIndices[batch.bufferIndex][commandId] = material.index;

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
*/
void Renderer::setProjection(const glm::mat4 &proj) {
    shader->setUniform(shader->getUniformLocation("MV"), proj);
    dispatchCompute->setUniform(1, proj);
    viewproj = proj;
}

void Renderer::setEyePos(const glm::vec3 &pos) {
    shader->setUniform(shader->getUniformLocation("eyePos"), pos);
}
/*
void Renderer::renderbatch(Batch &batch) {

    if (batch.commandCount > 0) {

        glFlushMappedNamedBufferRange(batch.batchBuffer->computeCullCommandsBuffer, batch.bufferOffset * sizeof(ComputeCullCommand), batch.commandCount * sizeof(ComputeCullCommand));
        glFlushMappedNamedBufferRange(batch.batchBuffer->materialIndexBuffer, batch.bufferOffset * sizeof(GLuint), batch.commandCount * sizeof(GLuint));
        glFlushMappedNamedBufferRange(batch.batchBuffer->transformBuffer, batch.bufferOffset * sizeof(Transform), batch.commandCount * sizeof(Transform));

        dispatchCompute->use();
        dispatchCompute->setUniform(0, batch.bufferOffset); //offset

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.meshBuffer->meshDataBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.batchBuffer->computeCullCommandsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.batchBuffer->indirectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, batch.batchBuffer->transformBuffer);

        glDispatchCompute(batch.commandCount, 1, 1);

        glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

        shader->use();
        shader->setUniform(shader->getUniformLocation("tex"), 0);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, batch.batchBuffer->indirectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.batchBuffer->transformBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.batchBuffer->materialIndexBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.materialArray->buffer);

        //TODO set texture arrays

        glMultiDrawElementsIndirect(GL_PATCHES, GL_UNSIGNED_SHORT,
                                    reinterpret_cast<const void *>(batch.bufferOffset * sizeof(DrawElementsIndirectCommand)),
                                    batch.commandCount, 0);

        //set fence
        batch.batchBuffer->activeBatches.emplace_back(std::make_pair(batch.bufferOffset, glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0)));

        batch.valid = false;
    }
}
*/
/*
void Renderer::submit(const RenderObject &robj) {
    numObject++;
    submit(robj.mesh, robj.mat, robj.transform);
}

void Renderer::submit(const std::vector<RenderObject>& objs) {
    for(const RenderObject& obj : objs) {
        submit(obj);
    }
}

void Renderer::submit(OctreeNode &octree) {

    glm::vec4 corners[8];
    for(int i = 0; i < 8; i++) {
        float ox = i & 4 ? 1 : -1;
        float oy = i & 2 ? 1 : -1;
        float oz = i & 1 ? 1 : -1;
        corners[i] = viewproj * glm::vec4(octree.center + glm::vec3(ox,oy,oz) * octree.halfSize, 1.0f);
    }

    glm::bvec3 allGt(true);
    glm::bvec3 allLt(true);
    for(int i = 0; i < 8; i++) {
        glm::bvec3 gt = glm::greaterThan(glm::vec3(corners[i]), glm::vec3(corners[i].w));
        glm::bvec3 lt = glm::lessThan(glm::vec3(corners[i]), -glm::vec3(corners[i].w));

        allGt &= gt;
        allLt &= lt;
    }

    bool inside = !(any(allGt) || any(allLt));

    if(inside) {

        if(octree.nodes) {
            for(int i = 0; i < 8; i++) {
                submit(octree.nodes[i]);
            }
        } else {
            for(auto r : octree.renderObjects) {
                submit(*r);
            }
        }

    }

}

void Renderer::submit(BVH &bvh) {

    glm::vec4 corners[8];
    corners[0] = viewproj * glm::vec4(bvh.min[0], bvh.max[1], bvh.min[2], 1.f);
    corners[1] = viewproj * glm::vec4(bvh.min[0], bvh.max[1], bvh.max[2], 1.f);
    corners[2] = viewproj * glm::vec4(bvh.max[0], bvh.max[1], bvh.max[2], 1.f);
    corners[3] = viewproj * glm::vec4(bvh.max[0], bvh.max[1], bvh.min[2], 1.f);
    corners[4] = viewproj * glm::vec4(bvh.max[0], bvh.min[1], bvh.min[2], 1.f);
    corners[5] = viewproj * glm::vec4(bvh.max[0], bvh.min[1], bvh.max[2], 1.f);
    corners[6] = viewproj * glm::vec4(bvh.min[0], bvh.min[1], bvh.max[2], 1.f);
    corners[7] = viewproj * glm::vec4(bvh.min[0], bvh.min[1], bvh.min[2], 1.f);


    glm::bvec3 allGt(true);
    glm::bvec3 allLt(true);
    for(int i = 0; i < 8; i++) {
        glm::bvec3 gt = glm::greaterThan(glm::vec3(corners[i]), glm::vec3(corners[i].w));
        glm::bvec3 lt = glm::lessThan(glm::vec3(corners[i]), -glm::vec3(corners[i].w));

        allGt &= gt;
        allLt &= lt;
    }

    bool inside = !(any(allGt) || any(allLt));

    if (inside) {
        if(bvh.children) {
            submit(bvh.children[0]);
            submit(bvh.children[1]);
        } else {
            for(auto r : bvh.objects) {
                submit(*r);
            }
        }
    }
}

*/

void Renderer::renderBatch(Batch &batch) {
    dispatchCompute->use();
    dispatchCompute->setUniform(0, 0); //offset

    //GPU Culling
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.meshBuffer.meshDataBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.computeCullCommandsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.indirectBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, batch.transformBuffer);

    glDispatchCompute(batch.batchSize, 1, 1);

    glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

    //render indirect
    shader->use();
    shader->setUniform(shader->getUniformLocation("tex"), 0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, batch.indirectBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.transformBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.materialIndexBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.materialArray.buffer);

    //TODO set texture arrays

    glMultiDrawElementsIndirect(GL_PATCHES, GL_UNSIGNED_SHORT, 0, batch.batchSize, 0);
}

void Renderer::renderBatch(StaticBatch &batch) {
    renderBatch(static_cast<Batch&>(batch));
}

void Renderer::renderBatch(DynamicBatch &batch) {

    if(batch.fence) {
        GLenum timeoutflag;
        do {
            timeoutflag = glClientWaitSync(batch.fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
        } while (timeoutflag == GL_TIMEOUT_EXPIRED);

        batch.fence = nullptr;
    }

    //update the transforms
    for(int i = 0; i < batch.batchSize; i++) {
        batch.transforms[i] = batch.objects[i].transform;
    }
    //submit
    renderBatch(static_cast<Batch&>(batch));

    //fence
    batch.fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}


/*

Batch::Batch(BatchBuffer* batchBuffer, GLuint bufferOffset, GLuint bufferSize) :
    batchBuffer(batchBuffer), bufferSize(bufferSize), bufferOffset(bufferOffset){

    commands = batchBuffer->commandsPtr + bufferOffset;
    materialIndices = batchBuffer->materialIndicesPtr + bufferOffset;
    transforms = batchBuffer->transformsPtr + bufferOffset;
}

Batch::~Batch() {

}

void Batch::add(const Mesh &mesh, const Material &material, const Transform &transform) {
    assert(meshBuffer == mesh.buffer);
    assert(materialArray == material.array);
    assert(valid);

    GLuint commandId = commandCount++;

    commands[commandId].meshIndex = mesh.index;
    materialIndices[commandId] = material.index;
    transforms[commandId] = transform;

    if(commandCount >= bufferSize) {
        //TODO: submit batch
    }

}

BatchBuffer::BatchBuffer(int batchSize, int batchCount) : batchSize(batchSize), batchCount(batchCount) {
    int bufferSize = batchSize * batchCount;

    //create Buffers
    glCreateBuffers(4, bufferObjects);

    //Command buffer
    GLbitfield flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT  | GL_MAP_COHERENT_BIT ;
    glNamedBufferStorage(computeCullCommandsBuffer, bufferSize * sizeof(ComputeCullCommand), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    commandsPtr = static_cast<ComputeCullCommand *>(glMapNamedBufferRange(computeCullCommandsBuffer, 0, bufferSize * sizeof(ComputeCullCommand), flags));

    //Indirect buffer
    flags = 0;
    glNamedBufferStorage(indirectBuffer, bufferSize * sizeof(DrawElementsIndirectCommand), nullptr, flags);

    //texture index buffer
    flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ;
    glNamedBufferStorage(materialIndexBuffer, bufferSize * sizeof(GLint), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    materialIndicesPtr = static_cast<GLuint *>(glMapNamedBufferRange(materialIndexBuffer, 0 , bufferSize * sizeof(GLuint), flags));

    //Transform buffers
    flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ;
    glNamedBufferStorage(transformBuffer, bufferSize * sizeof(Transform), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    transformsPtr = static_cast<Transform*>(glMapNamedBufferRange(transformBuffer, 0, bufferSize * sizeof(Transform), flags));

    //batch indices
    for(int i = 0; i < batchCount; i++) {
        waitingBatches.emplace_back(i);
    }

}

BatchBuffer::~BatchBuffer() {
    glUnmapNamedBuffer(indirectBuffer);
    glUnmapNamedBuffer(materialIndexBuffer);
    glUnmapNamedBuffer(transformBuffer);

    glDeleteBuffers(4, bufferObjects);
}
*/