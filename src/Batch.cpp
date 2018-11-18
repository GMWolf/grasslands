//
// Created by b5023110 on 30/10/2018.
//

#include "Batch.h"
#include <limits>

Batch::Batch(GLuint batchSize, MeshBuffer &mb, baseMaterialType& ma) : batchSize(batchSize), meshBuffer(mb), matType(ma) {

}


StaticBatch::StaticBatch(std::vector<RenderObject*> &robj) : Batch(robj.size(), *robj[0]->mesh.buffer, *robj[0]->mat.type) {

    auto * commands = new ComputeCullCommand[batchSize];
    auto * materialIndices = new GLuint[batchSize];
    auto * transforms = new Transform[batchSize];

    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(-std::numeric_limits<float>::max());

    for(int i = 0; i < batchSize; i++) {
        commands[i].meshIndex = robj[i]->mesh.index;
        materialIndices[i] = robj[i]->mat.index;
        transforms[i] = robj[i]->transform;

        const Mesh& mesh = robj[i]->mesh;
        glm::vec3 corners[8];
        corners[0] = robj[i]->transform.apply( glm::vec3(mesh.bboxMin[0], mesh.bboxMax[1], mesh.bboxMin[2]));
        corners[1] = robj[i]->transform.apply( glm::vec3(mesh.bboxMin[0], mesh.bboxMax[1], mesh.bboxMax[2]));
        corners[2] = robj[i]->transform.apply( glm::vec3(mesh.bboxMax[0], mesh.bboxMax[1], mesh.bboxMax[2]));
        corners[3] = robj[i]->transform.apply( glm::vec3(mesh.bboxMax[0], mesh.bboxMax[1], mesh.bboxMin[2]));
        corners[4] = robj[i]->transform.apply( glm::vec3(mesh.bboxMax[0], mesh.bboxMin[1], mesh.bboxMin[2]));
        corners[5] = robj[i]->transform.apply( glm::vec3(mesh.bboxMax[0], mesh.bboxMin[1], mesh.bboxMax[2]));
        corners[6] = robj[i]->transform.apply( glm::vec3(mesh.bboxMin[0], mesh.bboxMin[1], mesh.bboxMax[2]));
        corners[7] = robj[i]->transform.apply( glm::vec3(mesh.bboxMin[0], mesh.bboxMin[1], mesh.bboxMin[2]));

        for(int i = 0; i < 8; i++) {
            min = glm::min(min, corners[i]);
            max = glm::max(max, corners[i]);
        }

    }

    glCreateBuffers(4, bufferObjects);

    glNamedBufferStorage(computeCullCommandsBuffer, batchSize * sizeof(ComputeCullCommand), commands, 0);
    glNamedBufferStorage(indirectBuffer, batchSize * sizeof(DrawElementsIndirectCommand), nullptr, 0);
    glNamedBufferStorage(materialIndexBuffer, batchSize * sizeof(GLuint), materialIndices, 0);
    glNamedBufferStorage(transformBuffer, batchSize * sizeof(Transform), transforms, 0);

    delete[] commands;
    delete[] materialIndices;
    delete[] transforms;
}



DynamicBatch::DynamicBatch(std::vector<RenderObject*>& robj) :
    Batch(robj.size(), *robj[0]->mesh.buffer, *robj[0]->mat.type), objects(robj) {


    auto * commands = new ComputeCullCommand[batchSize];
    auto * materialIndices = new GLuint[batchSize];

    for(int i = 0; i < batchSize; i++) {
        commands[i].meshIndex = objects[i]->mesh.index;
        materialIndices[i] = objects[i]->mat.index;
    }

    glCreateBuffers(3, bufferObjects);
    glNamedBufferStorage(computeCullCommandsBuffer, batchSize * sizeof(ComputeCullCommand), commands, 0);
    glNamedBufferStorage(indirectBuffer, batchSize * sizeof(DrawElementsIndirectCommand), nullptr, 0);
    glNamedBufferStorage(materialIndexBuffer, batchSize * sizeof(GLuint), materialIndices, 0);

    //multi buffered transform buffer
    glCreateBuffers(buffCount, transformBuffers);
    for(int i = 0; i < buffCount; i++) {
        glNamedBufferStorage(transformBuffers[i], batchSize * sizeof(Transform), nullptr,
                             GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT  | GL_MAP_COHERENT_BIT);
        transforms[i] = static_cast<Transform*>(glMapNamedBufferRange(transformBuffers[i], 0, batchSize * sizeof(Transform), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
        fence[i] = nullptr;
    }
    transformBuffer = transformBuffers[0];

    for(int i = 0; i < batchSize; i++) {
        transforms[0][i] = robj[i]->transform;
    }

    delete[] commands;
    delete[] materialIndices;
}
