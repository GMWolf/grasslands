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
#include "ShadowMap.h"
#include "PingPong.h"

class Renderer {

public:

    Renderer(int width, int height);

    void setProjection(const glm::mat4& proj);
    void setView(const glm::mat4& view);
    void setEyePos(const glm::vec3& pos);

    void render(float time);

    void renderPass(Pass* pass);
    void renderPass(ScenePass* pass);
    void renderPass(PostPass* postPass);

    void renderBatch(Batch& batch, ScenePass* pass);
    void renderBatch(StaticBatch& batch, ScenePass* pass);
    void renderBatch(DynamicBatch& batch, ScenePass* pass);


    int numObject = 0;

    glm::mat4 view;
    glm::mat4 proj;

    //Batch batch;

    void addObjects(std::vector<RenderObject*>& renderObjects);

    std::vector<StaticBatch> staticBatches;
    std::vector<DynamicBatch> dynamicBatches;

    Shader* dispatchCompute;
    Shader* defaultDepthShader;

    glm::vec3 eyePos;

    ShadowMap shadowMap;
    //CSM shadowMap;

    ScenePass defaultPass;

    std::vector<Pass*> passes;

    PingPong pingPong;

private:
    void addOctreeNodes(OctreeNode& node);

    int width, height;

    Shader* volumetricShader;
    Shader* quadShader;
    Shader* boxShader;
    Shader* passShader;
    Shader* gradeShader;

    GLuint LUT;
};




#endif //GRASSLANDS_RENDERER_H
