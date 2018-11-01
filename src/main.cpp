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



int main() {


    if (!glfwInit()) {
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    GLFWwindow *window;


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

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
    glDebugMessageCallback(MessageCallback, 0);

    MeshBuffer meshBuffer;
    TextureGroup group;

    Mesh suzane = ObjLoader::load(meshBuffer, "../suzane.obj");
    Mesh knot = ObjLoader::load(meshBuffer, "../knot.obj");
    Mesh box = ObjLoader::load(meshBuffer, "../Box.obj");
    Mesh gear = ObjLoader::load(meshBuffer, "../gear.obj");
    Mesh grass = ObjLoader::load(meshBuffer, "../grass.obj");
    Mesh cube = ObjLoader::load(meshBuffer, "../cube.obj");

    Mesh meshes[] = {
            suzane, knot, box, gear, cube
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
    Material mat2 = pbrType.addMaterial(MaterialData(BrickDiffuse, BrickNormal, BrickRAM, BrickHeight));
    Material mat3 = pbrType.addMaterial(MaterialData(TilesDiffuse, TilesNormal, TilesRAM, TilesHeight));
    Material mat4 = pbrType.addMaterial(MaterialData(MetalDiffuse, MetalNormal, MetalRAM, MetalHeight));

    Material materials[] {
        mat1,mat2,mat3,mat4
    };

    Renderer renderer;
    Camera camera;

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    double lastTime = glfwGetTime();
    float time = 0;

    std::vector<RenderObject> objects;
    std::vector<RenderObject*> objptr;
    srand(10);


    /*Octree octree(50 * 3);
    std::cout << "built octree" << std::endl;*/
    BVH bvh(128);

    int halfSize = 32;
    //submit a lot of suzanes
    for (int i = -halfSize; i < halfSize; i++) {
        for (int j = -halfSize; j < halfSize; j++) {
            for (int k = -halfSize; k < halfSize; k++) {
                Material &mat = materials[rand() % 4];
                Mesh &m = meshes[rand() % 5];

                Transform transform{};
                transform.pos = glm::vec3(i, j, k) * 3.0f;
                transform.scale = 1;
                transform.rot = glm::quat(glm::vec3(0, 0, 0));
                objects.emplace_back(m, mat, transform);
                //octree.root.insert(&objects.back());
            }
        }
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

        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float ratio = width / (float) height;

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 10000.f);

        camera.update(window, dt);
        renderer.setProjection(projection);
        renderer.setView(camera.view);
        renderer.setEyePos(camera.pos);

        suzane.buffer->bindVa();
        RockDiffuse.textureArray->bind(0);

        //Rotate
        for(RenderObject& o : objects) {
            o.transform.rot = glm::quat(glm::vec3(0, time / 2 ,0));
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