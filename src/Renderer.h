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

#include "PassMask.h"

struct Light {
    glm::vec3 pos;
    float radius;
    glm::vec3 color;
    float intensity;
};

struct LightData {
    GLuint count;
private: GLuint a,b,c;
public:
    Light lights[4096];

    void addLight(Light light) {
        lights[count++] = light;
    }

    void clear() {
        count = 0;
    }
};

struct PassInfo {
    glm::mat4 projection;
    glm::mat4 view;

    Shader* shaderOverride = nullptr;
    int mask = PASS_ALL;
};

class Renderer {

public:

    Renderer(int width, int height);

    void setCamera(const Camera& cam);
    void setProjection(const glm::mat4& proj);
    void setView(const glm::mat4& view);
    void setEyePos(const glm::vec3& pos);

    void render(float time);

    void renderBatch(Batch& batch, PassInfo& pass);
    void renderBatch(StaticBatch& batch, PassInfo& pass);
    void renderBatch(DynamicBatch& batch, PassInfo& pass);
    void renderBatches(PassInfo& pass);
    void renderQuad();


    void shadowPass();

    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 eyePos;

    //Batch batch;

    void addObjects(std::vector<RenderObject*>& renderObjects);

    std::vector<StaticBatch> staticBatches;
    std::vector<DynamicBatch> dynamicBatches;

    Shader* dispatchCompute;
    Shader* defaultDepthShader;

    ShadowMap shadowMap;
    //CSM shadowMap;

    PingPong pingPong;
    FrameBuffer OGBuffer;


    bool showLightDebug = false;

    std::vector<Light> lights;

    glm::vec3 sunDir = glm::vec3(1,1,0);
    glm::vec3 sunCol = glm::vec3(4, 4, 3.25);

    Texture* skybox = nullptr;
    Texture* radiance = nullptr;

    LightData* lightData;
private:

    const Camera* cam;

    GLuint lightBuffer;
    GLuint lightIndexBuffer;



    void addOctreeNodes(OctreeNode& node);

    int width, height;
    float time = 0;

    Shader* volumetricShader;
    Shader* quadShader;
    Shader* boxShader;
    Shader* passShader;
    Shader* gradeShader;
    Shader* lightCullShader;
    Shader* lightDebugShader;
    Shader* skyboxShader;

    GLuint LUT;


    void colorGradePass();

    void volumetricPass();

    void pingpongToScreen();

    void renderLightDebug();

    void scenePass();

    void cullLights();

    void depthPrepass();

    void updateLights();
};




#endif //GRASSLANDS_RENDERER_H
