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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        shouldClose = true;
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
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_SRGB_CAPABLE, true);

    //glfwGetPrimaryMonitor()
    window = glfwCreateWindow(1280, 720, "Grasslands", NULL, NULL);


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
    glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(MessageCallback, 0);

    MeshBuffer meshBuffer;
    TextureGroup group;

    Mesh suzane = ObjLoader::load(meshBuffer, "../suzane.obj");
    Mesh knot = ObjLoader::load(meshBuffer, "../knot.obj");
    Mesh box = ObjLoader::load(meshBuffer, "../Box.obj");
    Mesh gear = ObjLoader::load(meshBuffer, "../gear.obj");
    Mesh grass = ObjLoader::load(meshBuffer, "../grass.obj");
    Mesh grassA = ObjLoader::load(meshBuffer, "../models/grassA.obj");
    Mesh thistle = ObjLoader::load(meshBuffer, "../models/thistle.obj");
    Mesh cube = ObjLoader::load(meshBuffer, "../cube.obj");
    Mesh quad = ObjLoader::load(meshBuffer, "../quad.obj");

    Mesh meshes[] = {
            suzane, knot, box, gear
    };

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

    Texture GroundAlbedo = loadDDS(group, "../textures/groundGrass/grass1-albedo.DDS");
    Texture GroundNormal = loadDDS(group, "../textures/groundGrass/grass1-normal.DDS");
    Texture GroundRAM    = loadDDS(group, "../textures/groundGrass/grass1-RAM.DDS");

    std::ifstream pbr_vertFile("../shaders/PBRVertex.glsl");
    std::string pbr_vertexText((std::istreambuf_iterator<char>(pbr_vertFile)), (std::istreambuf_iterator<char>()));
    std::ifstream pbrtess_vertFile("../shaders/PBRTessVertex.glsl");
    std::string pbrtess_vertexText((std::istreambuf_iterator<char>(pbrtess_vertFile)), (std::istreambuf_iterator<char>()));
    std::ifstream pbr_contFile("../shaders/PBRTessellateControl.glsl");
    std::string pbr_contText((std::istreambuf_iterator<char>(pbr_contFile)), (std::istreambuf_iterator<char>()));
    std::ifstream pbr_evalFile("../shaders/PBRTessellateEvaluation.glsl");
    std::string pbr_evalText((std::istreambuf_iterator<char>(pbr_evalFile)), (std::istreambuf_iterator<char>()));
    std::ifstream pbr_fragFile("../shaders/PBRFragment.glsl");
    std::string pbr_fragText((std::istreambuf_iterator<char>(pbr_fragFile)), (std::istreambuf_iterator<char>()));

    Shader* pbrTesselateShader = new Shader({
        {GL_VERTEX_SHADER, pbrtess_vertexText},
        {GL_TESS_CONTROL_SHADER, pbr_contText},
        {GL_TESS_EVALUATION_SHADER, pbr_evalText},
        {GL_FRAGMENT_SHADER, pbr_fragText}
    });
    Shader* pbrShader = new Shader({
        {GL_VERTEX_SHADER, pbr_vertexText},
        {GL_FRAGMENT_SHADER, pbr_fragText}
    });

    MaterialType<MaterialData> pbrType(pbrShader,10);
    MaterialType<MaterialData, GL_PATCHES> pbrTessType(pbrTesselateShader, 10);
    Material mat1 = pbrType.addMaterial(MaterialData(RockDiffuse, RockNormal, RockRAM, RockHeight));
    Material mat2 = pbrTessType.addMaterial(MaterialData(BrickDiffuse, BrickNormal, BrickRAM, BrickHeight));
    Material mat3 = pbrType.addMaterial(MaterialData(TilesDiffuse, TilesNormal, TilesRAM, TilesHeight));
    Material mat4 = pbrType.addMaterial(MaterialData(MetalDiffuse, MetalNormal, MetalRAM, MetalHeight));
    Material matGround = pbrType.addMaterial(MaterialData(GroundAlbedo, GroundNormal, GroundRAM, GroundAlbedo));

    Material materials[] {
        mat1,mat2,mat3,mat4
    };


    std::ifstream grass_vertFile("../shaders/grassVertex.glsl");
    std::string grass_vertText((std::istreambuf_iterator<char>(grass_vertFile)), (std::istreambuf_iterator<char>()));
    std::ifstream grass_fragFile("../shaders/grassFragment.glsl");
    std::string grass_fragText((std::istreambuf_iterator<char>(grass_fragFile)), (std::istreambuf_iterator<char>()));

    Shader* grassShader = new Shader({
        {GL_VERTEX_SHADER, grass_vertText},
        {GL_FRAGMENT_SHADER, grass_fragText}
    });

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
    Material matGrass12 = grassType.addMaterial({grass12Albedo, grass12Normal, grass12RA, grass12Tr});
    Material matWeed11  = grassType.addMaterial({weed11Albedo, weed11Normal, weed11RA, weed11Tr});
    Material matThistle = grassType.addMaterial({thistle17Albedo, thistle17Normal, thistle17RA, thistle17Tr});

    Renderer renderer;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float ratio = width / (float) height;
    glViewport(0, 0, width, height);
    Camera camera(ratio, 60, 0.1, 500.f);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    //glEnable(GL_FRAMEBUFFER_SRGB);

    double lastTime = glfwGetTime();
    float time = 0;

    std::vector<RenderObject> objects;
    std::vector<RenderObject*> objptr;
    srand(10);


    /*Octree octree(50 * 3);
    std::cout << "built octree" << std::endl;*/

    for(int i = -50; i < 50; i++) {
        for(int j = -50; j < 50; j++) {
            Transform t{};
            t.pos = glm::vec3(i * 4, 0, j * 4);
            t.scale = 2;
            t.rot = glm::quat();
            objects.emplace_back(quad, matGround, t, true);
        }
    }

    for(int i = 0; i < 30000; i++) {
        Transform t{};
        glm::vec2 pos2D = glm::diskRand(100.f);
        float clumpScale = (rand() / (float)RAND_MAX) * 0.3f + 0.5f;

        for(int j = 0; j < 3; j++) {
            glm::vec2 subpos2D = pos2D + glm::diskRand(1.f);
            t.scale = clumpScale + (rand() / (float)RAND_MAX) * 0.1f - 0.05f;
            t.pos = glm::vec3(subpos2D.x, 0, subpos2D.y);
            t.rot = glm::quat(glm::vec3(0, (rand() / (float) RAND_MAX) * 2 * 3.14159, 0));
            objects.emplace_back(grassA, matGrass12, t, true);
        }
    }

    for(int i = 0; i < 1000; i++) {
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

    for(int i = 0; i < 100000; i++) {
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



    for(int i = 0; i < 150; i++) {

        Transform t{};

        glm::vec2 pos2D = glm::diskRand(100.f);
        t.pos = glm::vec3(pos2D.x, 5, pos2D.y);
        t.rot = glm::quat(glm::vec3(0, (rand() / RAND_MAX) * 2 * 3.14159, 0));
        t.scale = 5;

        objects.emplace_back(meshes[rand() % 4], materials[rand() % 4], t, true);
    }

    for(auto& o : objects) {
        objptr.emplace_back(&o);
    }

    renderer.addObjects(objptr);


    while(!(glfwWindowShouldClose(window) || shouldClose)) {

        float thisTime = glfwGetTime();
        float dt = thisTime - lastTime;
        lastTime = thisTime;
        time += dt;

        glClearColor(0.7,0.7,0.8,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.update(window, dt);
        renderer.setProjection(camera.proj);
        renderer.setView(camera.view);
        renderer.setEyePos(camera.pos);

        meshBuffer.bindVa();
        group.bind();
        //Rotate
        for(RenderObject& o : objects) {
          //  o.transform.rot = glm::quat(glm::vec3(0, time / 2 ,0));
        }

        renderer.numObject = 0;
        //renderer.submit(bvh);
        for(auto &batch : renderer.dynamicBatches) {
            renderer.renderBatch(batch);
        }
        for(auto &batch : renderer.staticBatches) {
            renderer.renderBatch(batch);
        }

        renderer.numObject = 0;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}