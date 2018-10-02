//
// Created by felix on 30/09/2018.
//

#ifndef GRASSLANDS_RENDERER_H
#define GRASSLANDS_RENDERER_H

#include <vector>
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include <glm.hpp>
#include "Transform.h"

struct DrawElementsIndirectCommand{
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

struct ModelBatch {
    explicit ModelBatch( MeshBuffer*const buffer, TextureArray* const array);
    MeshBuffer* const meshBuffer;
    TextureArray* const textureArray;

    GLuint maxCommandCount;
    GLuint commandCount;

    GLuint instanceCount;

    union {
        GLuint bufferObjects[3];
        struct {
            GLuint indirectBuffer;
            GLuint textureIndexBuffer;
            GLuint modelMatrixBuffer;
        };
    };

    DrawElementsIndirectCommand* commands;
    GLuint *textureIndices;
    Transform *modelMatrices;
};

class Renderer {

public:

    Renderer();


    void submit(const Mesh &mesh, const Texture &texture, const Transform &transform);

    void flushBatches();

    void setProjection(const glm::mat4& proj);

private:
    void renderbatch(ModelBatch& batch);
    void clearBatches();

    ModelBatch& getBatch(MeshBuffer *const buffer, TextureArray* const array);

    std::vector<ModelBatch> batches;
    Shader* shader;

};


#endif //GRASSLANDS_RENDERER_H
