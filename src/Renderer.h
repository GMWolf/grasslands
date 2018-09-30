//
// Created by felix on 30/09/2018.
//

#ifndef GRASSLANDS_RENDERER_H
#define GRASSLANDS_RENDERER_H

#include <vector>
#include "Shader.h"
#include "Mesh.h"

struct DrawElementsIndirectCommand{
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

struct ModelBatch {
    explicit ModelBatch(const MeshBuffer* buffer);
    const MeshBuffer* meshBuffer;

    GLuint maxCommandCount;
    GLuint indirectBuffer;
    GLuint commandCount;

    DrawElementsIndirectCommand* commands;
};

class Renderer {

public:

    Renderer();


    void submit(Mesh& mesh);

    void renderBatches();

    void setProjection(const glm::mat4& proj);

private:

    void clearBatches();

    ModelBatch& getBatch(const MeshBuffer* buffer);

    std::vector<ModelBatch> batches;
    Shader* shader;

};


#endif //GRASSLANDS_RENDERER_H
