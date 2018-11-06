//
// Created by felix on 30/09/2018.
//
#include <gtc/type_ptr.hpp>
#include "Renderer.h"
#include <fstream>
#include "Material.h"


Renderer::Renderer() {

    std::ifstream dispatchFile("../shaders/dispatchGeom.glsl");
    std::string dispatchText((std::istreambuf_iterator<char>(dispatchFile)), (std::istreambuf_iterator<char>()));

    dispatchCompute = new Shader({
        {GL_COMPUTE_SHADER, dispatchText}
    });


    std::ifstream depthVertFile("../shaders/depthVert.glsl");
    std::string depthVertText((std::istreambuf_iterator<char>(depthVertFile)), (std::istreambuf_iterator<char>()));
    defaultDepthShader = new Shader({
        {GL_VERTEX_SHADER, depthVertText}
    });

}

void Renderer::setView(const glm::mat4 &v) {
    dispatchCompute->setUniform(0, proj * v);
    view = v;
}

void Renderer::setProjection(const glm::mat4 &p) {
    dispatchCompute->setUniform(0, p * view);
    proj = p;
}

void Renderer::setEyePos(const glm::vec3 &pos) {
    eyePos = pos;
}

void Renderer::renderBatch(Batch &batch) {
    dispatchCompute->use();

    //GPU Culling
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.meshBuffer.meshDataBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.computeCullCommandsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.indirectBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, batch.transformBuffer);

    glDispatchCompute(batch.batchSize, 1, 1);

    glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

    //render indirect
    Shader* shader = batch.matType.shader;
    shader->use();
    static const std::vector<int> tsamplers {
            0, 1, 2, 3, 4, 5, 6, 7
    };
    shader->setUniform(shader->getUniformLocation("tex"), tsamplers);
    shader->setUniform(shader->getUniformLocation("projection"), proj);
    shader->setUniform(shader->getUniformLocation("MV"), proj * view);
    shader->setUniform(shader->getUniformLocation("eyePos"), eyePos);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, batch.indirectBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.transformBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.materialIndexBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.matType.buffer);

    glMultiDrawElementsIndirect(batch.matType.primType, GL_UNSIGNED_SHORT, 0, batch.batchSize, 0);
}

void Renderer::renderBatch(StaticBatch &batch) {

    glm::mat4 viewproj = proj * view;

    glm::vec4 corners[8];
    corners[0] = viewproj * glm::vec4(batch.min[0], batch.max[1], batch.min[2], 1.f);
    corners[1] = viewproj * glm::vec4(batch.min[0], batch.max[1], batch.max[2], 1.f);
    corners[2] = viewproj * glm::vec4(batch.max[0], batch.max[1], batch.max[2], 1.f);
    corners[3] = viewproj * glm::vec4(batch.max[0], batch.max[1], batch.min[2], 1.f);
    corners[4] = viewproj * glm::vec4(batch.max[0], batch.min[1], batch.min[2], 1.f);
    corners[5] = viewproj * glm::vec4(batch.max[0], batch.min[1], batch.max[2], 1.f);
    corners[6] = viewproj * glm::vec4(batch.min[0], batch.min[1], batch.max[2], 1.f);
    corners[7] = viewproj * glm::vec4(batch.min[0], batch.min[1], batch.min[2], 1.f);

    glm::bvec3 allGt(true);
    glm::bvec3 allLt(true);
    for(int i = 0; i < 8; i++) {
        glm::bvec3 gt = glm::greaterThan(glm::vec3(corners[i]), glm::vec3(corners[i].w));
        glm::bvec3 lt = glm::lessThan(glm::vec3(corners[i]), -glm::vec3(corners[i].w));

        allGt &= gt;
        allLt &= lt;
    }

    bool inside = !(any(allGt) || any(allLt));

    bool fullInside = (!all(allGt)) && (!all(allLt));

    dispatchCompute->setUniform(1, !fullInside);


    if(inside) {
        renderBatch(static_cast<Batch&>(batch));
    }

}

void Renderer::renderBatch(DynamicBatch &batch) {

    //wait on fence
    if(batch.fence[batch.bufferIndex]) {
        GLenum timeoutflag;
        int n = 0;
        do {
            timeoutflag = glClientWaitSync(batch.fence[batch.bufferIndex], GL_SYNC_FLUSH_COMMANDS_BIT, 10);
            n++;
        } while (timeoutflag == GL_TIMEOUT_EXPIRED);
        batch.fence[batch.bufferIndex] = nullptr;
    }

    //update the transforms
    for(int i = 0; i < batch.batchSize; i++) {
        batch.transforms[batch.bufferIndex][i] = batch.objects[i]->transform;
    }
    batch.transformBuffer = batch.transformBuffers[batch.bufferIndex];

    //submit
    renderBatch(static_cast<Batch&>(batch));

    //fence
    batch.fence[batch.bufferIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    //change buffer id
    batch.bufferIndex = (batch.bufferIndex+1) % DynamicBatch::buffCount;
}

void Renderer::addObjects(std::vector<RenderObject *> &renderObjects) {

    std::map<baseMaterialType*, std::unique_ptr<Octree>> staticObjects;
    std::map<baseMaterialType*, std::unique_ptr<std::vector<RenderObject*>>> dynamicObjects;

    for(auto o : renderObjects) {
        baseMaterialType* matType = o->mat.type;

        if(o->isStatic) {
            if (staticObjects.find(matType) == staticObjects.end()) {
                staticObjects[matType] = std::make_unique<Octree>(100);
            }
            staticObjects[matType]->root.insert(o);
        } else {
            if (dynamicObjects.find(matType) == dynamicObjects.end()) {
                dynamicObjects[matType] = std::make_unique<std::vector<RenderObject*>>();
            }
            auto& vec = *dynamicObjects[matType];
            vec.emplace_back(o);

            if(vec.size() >= MAX_BATCH_SIZE) {
               dynamicBatches.emplace_back(vec);
               vec.clear();
            }
        }
    }


    for(auto& e : staticObjects) {
        addOctreeNodes(e.second->root);
    }

    for(auto& e : dynamicObjects) {
        dynamicBatches.emplace_back(*e.second);
        e.second->clear();
    }
}

void Renderer::addOctreeNodes(OctreeNode & node) {

    if (node.nodes) {
        for(int i = 0; i < 8; i++) {
            addOctreeNodes(node.nodes[i]);
        }
    } else {
        if(node.renderObjects.size() > 0) {
            staticBatches.emplace_back(node.renderObjects);
        }
    }

}
