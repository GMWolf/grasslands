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
#include "Octree.h"
#include "Material.h"
#include "Camera.h"
#include "BVH.h"
#include <deque>
#include "Batch.h"
#include <map>
/*
struct DrawElementsIndirectCommand{
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

struct ComputeCullCommand {
    GLuint meshIndex;
};*/
/*
class BatchBuffer;
struct Batch {
    Batch(BatchBuffer* batchBuffer, GLuint bufferOffset, GLuint bufferSize);
    ~Batch();

    BatchBuffer* batchBuffer;
    GLuint bufferOffset;
    GLuint bufferSize;
    GLuint commandCount = 0;

    Shader* shader;
    MeshBuffer* meshBuffer;
    MaterialArray* materialArray;

    ComputeCullCommand* commands;
    GLuint *materialIndices;
    Transform *transforms;

    void add(const Mesh &mesh, const Material& material, const Transform& transform);

    bool valid = true;
};

//BatchBuffer holds buffers batches use, along with fencing off bits of the buffer
class BatchBuffer {
public:
    BatchBuffer(int batchSize, int batchCount);
    ~BatchBuffer();

    union {
        GLuint bufferObjects[4];
        struct {
            GLuint computeCullCommandsBuffer;
            GLuint indirectBuffer;
            GLuint materialIndexBuffer;
            GLuint transformBuffer;
        };
    };

    ComputeCullCommand* commandsPtr;
    GLuint * materialIndicesPtr;
    Transform* transformsPtr;

    int batchSize;
    int batchCount;

    std::deque<GLuint> waitingBatches; //batches ready to be used
    std::deque<std::pair<GLuint, GLsync>> activeBatches; //Batches submitted
};
*/

class Renderer {

public:

    Renderer();

    //void submit(const Mesh &mesh, const Material& material, const Transform &transform);

   /* void submit(const RenderObject& robj);

    void submit(const std::vector<RenderObject>& objs);

    void submit(BVH& bvh);

    void submit(OctreeNode& octree);*/

    //void flushBatches();

    void setProjection(const glm::mat4& proj);
    void setView(const glm::mat4& view);
    void setEyePos(const glm::vec3& pos);


    void renderBatch(Batch& batch);
    void renderBatch(StaticBatch& batch);
    void renderBatch(DynamicBatch& batch);


    int numObject = 0;

    glm::mat4 view;
    glm::mat4 proj;

    //Batch batch;

    void addObjects(std::vector<RenderObject*>& renderObjects);

    std::vector<StaticBatch> staticBatches;
    std::vector<DynamicBatch> dynamicBatches;

    Shader* dispatchCompute;
    glm::vec3 eyePos;
};




#endif //GRASSLANDS_RENDERER_H
