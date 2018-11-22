#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Mesh.h"
#include "ObjLoader.h"
#include "DDSLoad.h"
#include "Shader.h"
#include "Renderer.h"
#include <gtc/matrix_transform.hpp>
#include "Texture.h"
#include "Transform.h"
#include "Camera.h"
#include "Octree.h"
#include "Material.h"
#include "gtc/random.hpp"
#include "SETTINGS.h"
#include "GUI.h"


void error_callback(int error, const char *description) {
    perror(description);
}

void GLAPIENTRY MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
             ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
             type, severity, message );
}

bool shouldClose = false;
bool mouse_active = true;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        shouldClose = true;
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS){
       mouse_active ^= 1;
       if (mouse_active) {
           glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
       } else {
           glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
       }
    }
}

struct GrassMatData {
    glm::ivec2 albedoTex;
    glm::ivec2 normalTex;
    glm::ivec2 roughAlphaTex;
    glm::ivec2 translucencyTex;
};

int main() {
    if (!glfwInit()) {
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    GLFWwindow *window;


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_SRGB_CAPABLE, true);

    //glfwGetPrimaryMonitor()
    window = glfwCreateWindow(1280, 720, "Grasslands demo", NULL/*glfwGetPrimaryMonitor()*/, NULL);
    //window = glfwCreateWindow(1920, 1200, "Grasslands", glfwGetPrimaryMonitor(), NULL);


    if (!window) {
        glfwTerminate();
        return -1;
    }


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSwapInterval(0);

    // During init, enable debug output
   // glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    glEnable(GL_MULTISAMPLE);

    MeshBuffer meshBuffer;
    TextureGroup group;

    Mesh suzane = ObjLoader::load(meshBuffer, "../models/suzane.obj");
    Mesh peakSuzane = ObjLoader::load(meshBuffer, "../models/peak suzane.obj");
    Mesh knot = ObjLoader::load(meshBuffer, "../knot.obj");
    Mesh box = ObjLoader::load(meshBuffer, "../Box.obj");
    Mesh gear = ObjLoader::load(meshBuffer, "../gear.obj");
    Mesh grass = ObjLoader::load(meshBuffer, "../grass.obj");
    Mesh grassA = ObjLoader::load(meshBuffer, "../models/grassA.obj");
    //Mesh grassClump = ObjLoader::load(meshBuffer, "../models/grassClump.obj");
    Mesh thistle = ObjLoader::load(meshBuffer, "../models/thistle.obj");
    Mesh cube = ObjLoader::load(meshBuffer, "../cube.obj");
    Mesh quad = ObjLoader::load(meshBuffer, "../quad.obj");
   // Mesh subdiv = ObjLoader::load(meshBuffer, "../models/subdivquad.obj");
    Mesh platform = ObjLoader::load(meshBuffer, "../models/platform.obj");


    Texture RockDiffuse = loadDDS(group, "../textures/RockJungle/Rock_CliffJungle3_albedo.DDS");
    Texture RockNormal = loadDDS(group, "../textures/RockJungle/Rock_CliffJungle3_normal.DDS");
    Texture RockRAM = loadDDS(group, "../textures/RockJungle/Rock_CliffJungle3_roughAOMetalic.DDS");
    Texture RockHeight = loadDDS(group, "../textures/RockJungle/Rock_CliffJungle3_height.DDS");

    Texture BrickDiffuse = loadDDS(group, "../textures/MedievalBrick/Brick_Medieval_albedo.DDS");
    Texture BrickNormal = loadDDS(group, "../textures/MedievalBrick/Brick_Medieval_normal.DDS");
    Texture BrickRAM = loadDDS(group, "../textures/MedievalBrick/Brick_Medieval_roughAOMetalic.DDS");
    Texture BrickHeight = loadDDS(group, "../textures/MedievalBrick/Brick_Medieval_height.DDS");

    Texture TilesDiffuse = loadDDS(group, "../textures/BrokenTiles/Tiles_Broken_albedo.DDS");
    Texture TilesNormal = loadDDS(group, "../textures/BrokenTiles/Tiles_Broken_normal.DDS");
    Texture TilesRAM = loadDDS(group, "../textures/BrokenTiles/Tiles_Broken_roughAOMetalic.DDS");
    Texture TilesHeight = loadDDS(group, "../textures/BrokenTiles/Tiles_Broken_height.DDS");

    Texture MetalDiffuse = loadDDS(group, "../textures/MetalThreadplate/Metal_ThreadplateBare_albedo.DDS");
    Texture MetalNormal = loadDDS(group,  "../textures/MetalThreadplate/Metal_ThreadplateBare_normal.DDS");
    Texture MetalRAM = loadDDS(group,     "../textures/MetalThreadplate/Metal_ThreadplateBare_roughAOMetalic.DDS");
    Texture MetalHeight = loadDDS(group,     "../textures/MetalThreadplate/Metal_ThreadplateBare_height.DDS");

    Texture GroundAlbedo = loadDDS(group, "../textures/GroundForest3/GroundForest003_albedo_1.DDS");
    Texture GroundNormal = loadDDS(group, "../textures/GroundForest3/GroundForest003_normal.DDS");
    Texture GroundRAM    = loadDDS(group, "../textures/GroundForest3/GroundForest003_RAM.DDS");

    Texture copperAlbedo = loadDDS(group, "../textures/copper/CopperPolished_albedo.DDS");
    Texture copperNormal = loadDDS(group, "../textures/copper/CopperPolished_normal.DDS");
    Texture copperRAM = loadDDS(group, "../textures/copper/CopperPolished_RAM.DDS");

    Texture goreAlbedo = loadDDS(group, "../textures/gore/Gore_albedo.DDS");
    Texture goreNormal = loadDDS(group, "../textures/gore/Gore_normal.DDS");
    Texture goreRAM    = loadDDS(group, "../textures/gore/Gore_ram.DDS");

    Texture cobbleAlbedo = loadDDS(group, "../textures/forest cobble/ForestCobblestone_albedo.DDS");
    Texture cobbleNormal = loadDDS(group, "../textures/forest cobble/ForestCobblestone_normal.DDS");
    Texture cobbleRAM = loadDDS(group, "../textures/forest cobble/ForestCobblestone_RAM.DDS");

    std::ifstream pbr_vertFile("../shaders/PBRVertex.glsl");
    std::string pbr_vertexText((std::istreambuf_iterator<char>(pbr_vertFile)), (std::istreambuf_iterator<char>()));
    std::ifstream pbrtess_vertFile("../shaders/PBRTessVertex.glsl");
    std::string pbrtess_vertexText((std::istreambuf_iterator<char>(pbrtess_vertFile)), (std::istreambuf_iterator<char>()));
    std::ifstream pbr_contFile("../shaders/PBRTessellateControl.glsl");
    std::string pbr_contText((std::istreambuf_iterator<char>(pbr_contFile)), (std::istreambuf_iterator<char>()));
    std::ifstream pbr_evalFile("../shaders/PBRTessellateEvaluation.glsl");
    std::string pbr_evalText((std::istreambuf_iterator<char>(pbr_evalFile)), (std::istreambuf_iterator<char>()));
    std::ifstream pbr_fragFile("../shaders/PBRFragment.glsl");
    std::string pbr_fragText("../shaders/PBRFragment.glsl"_preprocess);

    Shader* pbrTesselateShader = new Shader({
        {GL_VERTEX_SHADER, pbrtess_vertexText},
        {GL_TESS_CONTROL_SHADER, pbr_contText},
        {GL_TESS_EVALUATION_SHADER, pbr_evalText},
        {GL_FRAGMENT_SHADER, "../shaders/PBRTessFragment.glsl"_preprocess}
    });
    Shader* pbrTesselateDepthShader = new Shader({
        {GL_VERTEX_SHADER, pbrtess_vertexText},
        {GL_TESS_CONTROL_SHADER, pbr_contText},
        {GL_TESS_EVALUATION_SHADER, "../shaders/PBRTessEvaluationDepth.glsl"_preprocess},
        {GL_FRAGMENT_SHADER, "../shaders/depthFragment.glsl"_preprocess}
    });
    Shader* pbrShader = new Shader({
        {GL_VERTEX_SHADER, pbr_vertexText},
        {GL_FRAGMENT_SHADER, pbr_fragText}
    });

    MaterialType<MaterialData> pbrType(pbrShader,10);
    MaterialType<DispMaterialData, GL_PATCHES> pbrTessType(pbrTesselateShader, 10);
    pbrTessType.depthShaderOverride = pbrTesselateDepthShader;
    Material mat1 = pbrType.addMaterial(MaterialData(RockDiffuse, RockNormal, RockRAM));
    Material mat2 = pbrType.addMaterial(MaterialData(BrickDiffuse, BrickNormal, BrickRAM));
    Material mat3 = pbrType.addMaterial(MaterialData(TilesDiffuse, TilesNormal, TilesRAM));
    Material mat4 = pbrType.addMaterial(MaterialData(MetalDiffuse, MetalNormal, MetalRAM));
    Material copperMat = pbrType.addMaterial(MaterialData(copperAlbedo, copperNormal, copperRAM));
    Material goreMat = pbrType.addMaterial(MaterialData(goreAlbedo, goreNormal, goreRAM));
    Material cobbleMat = pbrType.addMaterial(MaterialData(cobbleAlbedo, cobbleNormal, cobbleRAM));


    //Texture heightMap = loadDDS(group, "../textures/yosemite_hm.DDS");


   // Material matGround = pbrTessType.addMaterial(DispMaterialData(GroundAlbedo, GroundNormal, GroundRAM, heightMap, 1.f, 10.0f, 1.f, true));
    //Material matGround = pbrTessType.addMaterial(DispMaterialData(BrickDiffuse, BrickNormal, BrickRAM, BrickHeight, 0.02f));
    Material matGround = pbrType.addMaterial(MaterialData(GroundAlbedo, GroundNormal, GroundRAM));
    Material materials[] {
        mat1,mat2,mat3,mat4
    };


    Shader* grassShader = new Shader({
        {GL_VERTEX_SHADER,   "../shaders/grassVertex.glsl"_preprocess},
        {GL_FRAGMENT_SHADER, "../shaders/grassFragment.glsl"_preprocess}
    });

    Shader* grassShadowShader = new Shader({
        {GL_VERTEX_SHADER,   "../shaders/grassDepthVert.glsl"_preprocess},
        {GL_FRAGMENT_SHADER, "../shaders/grassDepthFrag.glsl"_preprocess}
    });


    /*Shader* heightmapPositionCompute = new Shader ({
        {GL_COMPUTE_SHADER, "../shaders/ComputeTransformHeightmap.glsl"_preprocess}
    });
    heightMap.textureArray->bind();
    heightmapPositionCompute->setUniform("tex", (int)heightMap.textureArray->unit);
    heightmapPositionCompute->setUniform("layer", (GLuint)heightMap.layer);
    heightmapPositionCompute->setUniform("texScale", 0.001f);
    heightmapPositionCompute->setUniform("heightScale", 10.0f);*/

    Texture cubeMap = loadDDS(group, "../textures/skybox.dds");
    Texture radiance = loadDDS(group, "../textures/skybox_radiance.dds");

    Texture grass12Albedo = loadDDS(group, "../textures/Grass12/Grass12_albedo.DDS");
    Texture grass12Normal = loadDDS(group, "../textures/Grass12/Grass12_normal.DDS");
    Texture grass12RA     = loadDDS(group, "../textures/Grass12/Grass12_roughAlpha.DDS");
    Texture grass12Tr     = loadDDS(group, "../textures/Grass12/Grass12_translucency.DDS");

    Texture weed11Albedo = loadDDS(group, "../textures/Weed11/Weed_Various11_albedo.DDS");
    Texture weed11Normal = loadDDS(group, "../textures/Weed11/Weed_Various11_normal.DDS");
    Texture weed11RA     = loadDDS(group, "../textures/Weed11/Weed_Various11_RA.DDS");
    Texture weed11Tr     = loadDDS(group, "../textures/Weed11/Weed_Various11_translucency.DDS");

    Texture thistle17Albedo = loadDDS(group, "../textures/SnowThistle/Weed_Thistle17_albedo.DDS");
    Texture thistle17Normal = loadDDS(group, "../textures/SnowThistle/Weed_Thistle17_normal.DDS");
    Texture thistle17RA     = loadDDS(group, "../textures/SnowThistle/Weed_Thistle17_RA.DDS");
    Texture thistle17Tr     = loadDDS(group, "../textures/SnowThistle/Weed_Thistle17_translucency.DDS");

    MaterialType<GrassMatData> grassType(grassShader, 10);
    grassType.depthShaderOverride = grassShadowShader;
    grassType.alphaToCoverage = true;
    //grassType.cullBackfaces = false;
    //grassType.computeTransformShader = heightmapPositionCompute;
    grassType.mask = PASS_DEFAULT | PASS_DEPTH_TRANSMISIVE;
    Material matGrass12 = grassType.addMaterial({grass12Albedo, grass12Normal, grass12RA, grass12Tr});
    Material matWeed11  = grassType.addMaterial({weed11Albedo, weed11Normal, weed11RA, weed11Tr});
    Material matThistle = grassType.addMaterial({thistle17Albedo, thistle17Normal, thistle17RA, thistle17Tr});

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float ratio = width / (float) height;
    glViewport(0, 0, width, height);

    Renderer renderer(width, height);
    renderer.skybox = &cubeMap;
    renderer.radiance = &radiance;



    //setup lights
    //std::vector<glm::vec3> lightVel;
    //std::vector<float> lightAge;
    //const int lightCount = 512;
    //for(int i = 0; i < lightCount; i++) {
    //    lightVel.emplace_back(glm::ballRand(1.f));
    //    lightVel[i].y = abs(lightVel[i].y);
    //    lightAge.emplace_back(5 * rand() / (float) RAND_MAX);
    //    glm::vec2 pos = glm::diskRand(100.f);
    //    renderer.lightData->addLight({
    //        glm::vec3(pos.x, 0.5, pos.y),
    //        2,
    //        glm::vec3(1, 0.5, 0.05),
    //        5
    //    });
    //   /*renderer.lights.push_back({
    //       glm::vec3(pos.x, 0.5, pos.y),
    //       2,
    //       glm::vec3(1, 0.5, 0.05),
    //       5
    //   });*/
    //}



    Camera camera(ratio, 60, 0.1, 75.f);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //glEnable(GL_FRAMEBUFFER_SRGB);


    //CREATE bunch of objects
    std::vector<RenderObject> objects;
    std::vector<RenderObject*> objptr;
    srand(10);


    for(int i = -50; i < 50; i++) {
        for(int j = -50; j < 50; j++) {
            Transform t{};
            t.pos = glm::vec3(i * 4, 0, j * 4);
            t.scale = 2;
            t.rot = glm::quat();
            objects.emplace_back(quad, matGround, t);
        }
    }



    int grassCount = 30000;
    int weedCount = 1000;
    int thistleCount = 10000;

    for(int i = 0; i < grassCount; i++) {
        Transform t{};
        glm::vec2 pos2D = glm::diskRand(100.f);
        float clumpScale = (rand() / (float)RAND_MAX) * 0.3f + 1.0f;
        t.scale = clumpScale;
        t.pos = glm::vec3(pos2D.x, 0, pos2D.y);
        t.rot = glm::quat(glm::vec3(0, (rand() / (float) RAND_MAX) * 2 * 3.14159, 0));
        objects.emplace_back(grassA, matGrass12, t, true);
    }

    for(int i = 0; i < weedCount; i++) {
        Transform t{};
        glm::vec2 pos2D = glm::diskRand(100.f);
        float clumpScale = (rand() / (float)RAND_MAX) * 0.4f + 0.6f;

        for(int j = 0; j < 5; j++) {
            glm::vec2 subpos2D = pos2D + glm::diskRand(1.f);
            t.scale = clumpScale + (rand() / (float)RAND_MAX) * 0.1f - 0.05f;
            t.pos = glm::vec3(subpos2D.x, 0, subpos2D.y);
            t.rot = glm::quat(glm::vec3(0, (rand() / (float) RAND_MAX) * 2 * 3.14159, 0));
            objects.emplace_back(grass, matWeed11, t, true);
        }
    }

    for(int i = 0; i < thistleCount; i++) {
        Transform t{};
        glm::vec2 pos2D = glm::diskRand(100.f);
        float clumpScale = (rand() / (float)RAND_MAX) * 0.2f + 0.3f;

        for(int j = 0; j < 3; j++) {
            glm::vec2 subpos2D = pos2D + glm::diskRand(1.f);
            t.scale = clumpScale + (rand() / (float)RAND_MAX) * 0.1f - 0.05f;
            t.pos = glm::vec3(subpos2D.x, 0, subpos2D.y);
            t.rot = glm::quat(glm::vec3(0, (rand() / (float) RAND_MAX) * 2 * 3.14159, 0));
            objects.emplace_back(thistle, matThistle, t, true);
        }
    }

    std::vector<RenderObject*> rotateObjects;

    for(int i = 0; i < 200; i++) {

        Transform t{};

        glm::vec2 pos2D = glm::diskRand(100.f);
        t.pos = glm::vec3(pos2D.x, 5, pos2D.y);
        t.rot = glm::quat(glm::vec3(0, (rand() / (float)RAND_MAX) * 2 * 3.14159, 0));
        t.scale = 5 + (rand() / (float) RAND_MAX);

        objects.emplace_back(box, materials[rand() % 4], t, true);
    }

    for(int i = 0; i < 150; i++) {

        Transform t{};

        glm::vec2 pos2D = glm::diskRand(100.f);
        t.pos = glm::vec3(pos2D.x, 15, pos2D.y);
        t.rot = glm::quat(glm::vec3(0, (rand() / (float)RAND_MAX) * 2 * 3.14159, 0));
        t.scale = 5;

        objects.emplace_back(gear, materials[rand() % 4], t);
        rotateObjects.push_back(&objects.back());
    }

    Shader* fireCompute = new Shader ({
                                              {GL_COMPUTE_SHADER, "../shaders/fireCompute.glsl"_preprocess}
                                      });
    Texture fireTexture = loadDDS(group, "../textures/fire.DDS");

    //add peak suzane
    {
        Transform t{};
        t.pos = glm::vec3(-6,0, -6);
        t.rot = glm::quat();
        t.scale = 1.5;
        objects.emplace_back(platform, cobbleMat, t);

        t.pos.y = 2;
        t.scale = 2;

        objects.emplace_back(peakSuzane, copperMat, t);
        rotateObjects.push_back(&objects.back());
        //add minions
        int n = 5;
        for (int i = 0; i <n; i++) {
            Transform st{};
            st.pos = t.pos;
            st.pos.x += sin(i * (3.14159 * 2 / n)) * 8;
            st.pos.z += cos(i * (3.14159 * 2 / n)) * 8;
            st.scale = 1;
            st.rot = glm::quat(glm::vec3(0, 3.14159 + i * (3.14159 * 2 / n), 0));
            objects.emplace_back(suzane, goreMat, st);

            renderer.particleSystems.emplace_back(1000);
            renderer.particleSystems.back().computeShader = fireCompute;
            renderer.particleSystems.back().texture = fireTexture;
            renderer.particleSystems.back().blendSourceFactor = GL_ONE;
            renderer.particleSystems.back().blendDestFactor = GL_ONE;
            renderer.particleSystems.back().position = glm::vec3(st.pos.x, 0, st.pos.z);
            renderer.lightData->addLight({
                glm::vec3(st.pos.x, 0.5, st.pos.z),
                10,
                glm::vec3(1, 0.6, 0.3),
                10
            });
        }


    }






    for(auto& o : objects) {
        objptr.emplace_back(&o);
    }


    renderer.addObjects(objptr);



    // Fire thing
    /*for (int i = 0; i < 50; i++) {
        renderer.particleSystems.emplace_back(1000);
        renderer.particleSystems.back().computeShader = fireCompute;
        renderer.particleSystems.back().texture = fireTexture;
        renderer.particleSystems.back().blendSourceFactor = GL_ONE;
        renderer.particleSystems.back().blendDestFactor = GL_ONE;
        glm::vec2 pos= glm::diskRand(100.0f);
        renderer.particleSystems.back().position = glm::vec3(pos.x, 0, pos.y);
        renderer.lightData->addLight({
            glm::vec3(pos.x, 0.5, pos.y),
            10,
            glm::vec3(1, 0.6, 0.3),
            10
        });
    }*/






    GUI gui(window, renderer);
    //Main loop
    float lastTime = glfwGetTime();
    float time = 0;
    while(!(glfwWindowShouldClose(window) || shouldClose)) {
        glfwPollEvents();



        float thisTime = glfwGetTime();
        float dt = thisTime - lastTime;
        lastTime = thisTime;
        time += dt;


        gui.update(dt);

        //Rotate
        for(auto o : rotateObjects) {
            o->transform.rot = glm::quat(glm::vec3(0, time / 2 ,0));
        }

        //move lights
        /*for(int i = 0; i < lightCount; i++) {
            lightAge[i] -= dt;
            if (lightAge[i] <= 0) {
                glm::vec2 pos = glm::diskRand(100.f);
                renderer.lightData->lights[i].pos = glm::vec3(pos.x, 0, pos.y);
                lightVel[i] = glm::ballRand(1.f);
                lightVel[i].y = abs(lightVel[i].y);
                lightAge[i] = 5;
            }

            renderer.lightData->lights[i].pos += lightVel[i] * dt;
            if (lightAge[i] > 4.5) {
                renderer.lightData->lights[i].intensity = 5 * (5.f - lightAge[i]) / 0.5f;
            } else if (lightAge[i] < 0.5) {
                renderer.lightData->lights[i].intensity = 5 * (lightAge[i]) / 0.5f;
            } else {
                renderer.lightData->lights[i].intensity = 5;
            }

            lightVel[i] += glm::ballRand(1.f) * dt;
        }*/

        camera.update(window, dt, mouse_active);

        renderer.setCamera(camera);

        meshBuffer.bindVa();
        group.bind();

        renderer.render(dt);

        gui.render();
        glfwSwapBuffers(window);

    }
    nk_glfw3_shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}