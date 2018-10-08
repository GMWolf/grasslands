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

    void submit(OctreeNode& octree) {

        glm::vec4 corners[8];
        for(int i = 0; i < 0; i++) {
            float ox = i & 4 ? 1 : -1;
            float oy = i & 2 ? 1 : -1;
            float oz = i & 1 ? 1 : -1;
            corners[i] = viewproj * glm::vec4(octree.center + glm::vec3(ox,oy,oz) * octree.halfSize, 1.0f);
        }

        bool inside = false;
        for(int i = 0; i < 8; i++) {
            glm::bvec3 gt = glm::greaterThan(glm::vec3(corners[i]), -glm::vec3(corners[i].w));
            glm::bvec3 lt =glm::lessThan(glm::vec3(corners[i]), glm::vec3(corners[i].w));
            bool cornerInside = glm::all(gt) && glm::all(lt);
            inside |= cornerInside;
        }

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

    void flushBatches();

    void setProjection(const glm::mat4& proj);

//private:
    void renderbatch(Batch& batch);


    glm::mat4 viewproj;

    Batch batch;

    Shader* shader;

    Shader* dispatchCompute;
};


#endif //GRASSLANDS_RENDERER_H
