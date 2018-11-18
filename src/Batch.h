//
// Created by b5023110 on 30/10/2018.
//

#ifndef GRASSLANDS_BATCH_H
#define GRASSLANDS_BATCH_H
#include <glad/glad.h>
#include <vector>
#include "RenderObject.h"
#include "Transform.h"

struct DrawElementsIndirectCommand{
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

struct ComputeCullCommand {
    GLuint meshIndex;
};

struct Batch {
    Batch(GLuint batchSize, MeshBuffer& mb, baseMaterialType& ma);

    MeshBuffer& meshBuffer;
    baseMaterialType& matType;

    GLuint batchSize;



    union {
        GLuint bufferObjects[4];
        struct {
            GLuint computeCullCommandsBuffer;
            GLuint indirectBuffer;
            GLuint materialIndexBuffer;
            GLuint transformBuffer;
        };
    };
};

struct StaticBatch : public Batch {
    explicit StaticBatch(std::vector<RenderObject*>& robj);
    glm::vec3 min, max;
};

struct DynamicBatch : public Batch {

    static const int buffCount = 3;

    explicit DynamicBatch(std::vector<RenderObject*>& robjs);

    std::vector<RenderObject*> objects;
    Transform* transforms[buffCount];

    GLuint transformBuffers[buffCount];

    int bufferIndex = 0;

    GLsync fence[buffCount];
};


#endif //GRASSLANDS_BATCH_H
