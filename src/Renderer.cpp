//
// Created by felix on 30/09/2018.
//
#include <gtc/type_ptr.hpp>
#include "Renderer.h"
#include <fstream>
#include "Material.h"


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

void Renderer::setProjection(const glm::mat4 &proj) {
    shader->setUniform(shader->getUniformLocation("MV"), proj);
    dispatchCompute->setUniform(1, proj);
    viewproj = proj;
}

void Renderer::setEyePos(const glm::vec3 &pos) {
    shader->setUniform(shader->getUniformLocation("eyePos"), pos);
}

void Renderer::renderbatch(Batch &batch) {

    if (batch.commandCount > 0) {

        glFlushMappedNamedBufferRange(batch.computeCommandsBuffer, batch.bufferIndex * batch.bufferSize * sizeof(ComputeDispatchCommand) , batch.commandCount * sizeof(ComputeDispatchCommand));
        glFlushMappedNamedBufferRange(batch.materialIndexBuffer, batch.bufferIndex * batch.bufferSize * sizeof(GLuint), batch.commandCount * sizeof(GLuint));
        glFlushMappedNamedBufferRange(batch.transformBuffer, batch.bufferIndex * batch.bufferSize * sizeof(Transform), batch.commandCount * sizeof(Transform));

        dispatchCompute->use();
        dispatchCompute->setUniform(0, batch.bufferIndex * batch.bufferSize); //offset

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.meshBuffer->meshDataBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.computeCommandsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.indirectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, batch.transformBuffer);
        glDispatchCompute(batch.commandCount, 1, 1);

        glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

        shader->use();
        shader->setUniform(shader->getUniformLocation("tex"), 0);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, batch.indirectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.transformBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.materialIndexBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.materialArray->buffer);


        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT,
                                    reinterpret_cast<const void *>(batch.bufferIndex * batch.bufferSize * sizeof(DrawElementsIndirectCommand)),
                                    batch.commandCount, 0);

        batch.fences[batch.bufferIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);


        batch.commandCount = 0;
        batch.bufferIndex = (batch.bufferIndex + 1) % batch.bufferCount;

    }
}

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
    glNamedBufferStorage(materialIndexBuffer, bufferCount * bufferSize * sizeof(GLint), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    materialIndices[0] = static_cast<GLuint *>(glMapNamedBufferRange(materialIndexBuffer, 0 , bufferCount * bufferSize * sizeof(GLuint), flags));

    //Transform buffers
    flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ;
    glNamedBufferStorage(transformBuffer, bufferCount * bufferSize * sizeof(Transform), nullptr, flags);
    flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
    transforms[0] = static_cast<Transform*>(glMapNamedBufferRange(transformBuffer, 0, bufferCount *  bufferSize * sizeof(Transform), flags));


    //Get offset mappings
    for(int i = 1; i < bufferCount; i++) {
        commands[i] = commands[0] + (bufferSize * i);
        materialIndices[i] = materialIndices[0] + (bufferSize  * i);
        transforms[i] = transforms[0] + (bufferSize  * i);
    }

    //Set fences to nullptr
    for (auto &fence : fences) {
        fence = nullptr;
    }

}

Batch::~Batch() {
    glUnmapNamedBuffer(indirectBuffer);
    glUnmapNamedBuffer(materialIndexBuffer);
    glUnmapNamedBuffer(transformBuffer);

    glDeleteBuffers(3, bufferObjects);
}
