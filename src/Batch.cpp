//
// Created by b5023110 on 30/10/2018.
//

#include "Batch.h"

Batch::Batch(GLuint batchSize, MeshBuffer &mb, MaterialArray &ma) : batchSize(batchSize), meshBuffer(mb), materialArray(ma) {

}


StaticBatch::StaticBatch(std::vector<RenderObject> &robj) : Batch(robj.size(), *robj[0].mesh.buffer, *robj[0].mat.array){

    auto * commands = new ComputeCullCommand[batchSize];
    auto * materialIndices = new GLuint[batchSize];
    auto * transforms = new Transform[batchSize];

    for(int i = 0; i < batchSize; i++) {
        commands[i].meshIndex = robj[i].mesh.index;
        materialIndices[i] = robj[i].mat.index;
        transforms[i] = robj[i].transform;
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



DynamicBatch::DynamicBatch(std::vector<RenderObject> &robj) :
    Batch(robj.size(), *robj[0].mesh.buffer, *robj[0].mat.array),
    objects(robj)  {

    auto * commands = new ComputeCullCommand[batchSize];
    auto * materialIndices = new GLuint[batchSize];

    for(int i = 0; i < batchSize; i++) {
        commands[i].meshIndex = robj[i].mesh.index;
        materialIndices[i] = robj[i].mat.index;
    }

    glCreateBuffers(4, bufferObjects);
    glNamedBufferStorage(computeCullCommandsBuffer, batchSize * sizeof(ComputeCullCommand), commands, 0);
    glNamedBufferStorage(indirectBuffer, batchSize * sizeof(DrawElementsIndirectCommand), nullptr, 0);
    glNamedBufferStorage(materialIndexBuffer, batchSize * sizeof(GLuint), materialIndices, 0);

    //multi buffered transform buffer
    glNamedBufferStorage(transformBuffer, buffCount * batchSize * sizeof(Transform), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT |GL_DYNAMIC_STORAGE_BIT | GL_MAP_COHERENT_BIT);

    for(int i = 0; i < buffCount; i++) {
        fence[i] = nullptr;
    }

    assert(batchSize > 0);
    transforms = static_cast<Transform*>(glMapNamedBufferRange(transformBuffer, 0, buffCount * batchSize * sizeof(Transform), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));


    for(int i = 0; i < batchSize; i++) {
        transforms[i] = robj[i].transform;
    }


   // glFlushMappedNamedBufferRange(transformBuffer, 0, batchSize * sizeof(Transform));

    delete[] commands;
    delete[] materialIndices;
}
