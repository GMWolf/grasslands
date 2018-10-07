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
#include "RenderObject.h"

struct DrawElementsIndirectCommand{
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

struct ComputeDispatchCommand {
    GLuint meshIndex;
};

struct Batch {
    Batch();
    ~Batch();

    static const GLuint bufferCount = 3;
    static const GLuint bufferSize = 2048;

    const Mesh* mesh;

    GLuint bufferIndex = 0;

    GLuint commandCount = 0;

    union {
        GLuint bufferObjects[4];
        struct {
            GLuint computeCommandsBuffer;
            GLuint indirectBuffer;
            GLuint textureIndexBuffer;
            GLuint transformBuffer;
        };
    };

    ComputeDispatchCommand* commands[bufferCount];
    GLuint *textureIndices[bufferCount];
    Transform *transforms[bufferCount];

    GLsync fences[bufferCount];
};


class Renderer {

public:

    Renderer();

    void submit(const Mesh &mesh, const Texture &texture, const Transform &transform);

    void submit(const RenderObject& robj);

    void submit(const std::vector<RenderObject>& objs);

    void flushBatches();

    void setProjection(const glm::mat4& proj);

//private:
    void renderbatch(Batch& batch);

    Batch batch;

    Shader* shader;

    Shader* dispatchCompute;
};


#endif //GRASSLANDS_RENDERER_H
