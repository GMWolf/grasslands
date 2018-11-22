
//
// Created by Felix Bridault on 21/11/2018.
//

#include <gtc/random.inl>
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(size_t partCount) : partCount(partCount){
    glCreateBuffers(1, &vertBuffer);
    glCreateBuffers(1, &partBuffer);

    if (!vertBuffer || !partBuffer) {
        std::cout << "Arg! no buffers?" << std::endl;
    }

    glCreateVertexArrays(1, &vertexArray);


    std::vector<particleVertData> vertData;
    vertData.resize(partCount);
    for(int i = 0; i < partCount; i++) {
        vertData[i].pos = glm::ballRand(0.2f);
        vertData[i].size = 400.f;
    }

    glNamedBufferStorage(vertBuffer, partCount * sizeof(particleVertData), vertData.data(), 0);

    std::vector<partUData> partu;
    partu.resize(partCount);
    for(int i = 0; i < partCount; i++) {
        partu[i].vel = glm::ballRand(0.001);
        partu[i].age = (rand() / (float) RAND_MAX) *1.5f;
    }

    glNamedBufferStorage(partBuffer, partCount * sizeof(partUData), partu.data(), 0);

    glVertexArrayAttribFormat(vertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(particleVertData, pos));
    glVertexArrayAttribFormat(vertexArray, 1, 1, GL_FLOAT, GL_FALSE, offsetof(particleVertData, size));
    glVertexArrayAttribFormat(vertexArray, 2, 3, GL_FLOAT, GL_FALSE, offsetof(particleVertData, colour));
    glVertexArrayAttribFormat(vertexArray, 3, 1, GL_FLOAT, GL_FALSE, offsetof(particleVertData, rot));
    glVertexArrayVertexBuffer(vertexArray, 0, vertBuffer, 0, sizeof(particleVertData));
    glVertexArrayAttribBinding(vertexArray, 0, 0);
    glVertexArrayAttribBinding(vertexArray, 1, 0);
    glVertexArrayAttribBinding(vertexArray, 2, 0);
    glVertexArrayAttribBinding(vertexArray, 3, 0);
    glEnableVertexArrayAttrib(vertexArray, 0);
    glEnableVertexArrayAttrib(vertexArray, 1);
    glEnableVertexArrayAttrib(vertexArray, 2);
    glEnableVertexArrayAttrib(vertexArray, 3);
}

void ParticleSystem::update(float dt) {
    //update positions
    computeShader->use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, partBuffer);

    computeShader->setUniform("spawnPoint", position);
    computeShader->setUniform("spawnRadius", 0.25f);
    computeShader->setUniform("life", 1.5f);
    computeShader->setUniform("dt", dt);
    computeShader->setUniform("time", time);
    time += dt;

    glDispatchCompute(partCount, 1, 1);
}

void ParticleSystem::bindVA() {
    glBindVertexArray(vertexArray);
}
