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

    Mesh meshes[4] = {
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

    MaterialArray matArray;
    Material mat1 = matArray.addMaterial(RockDiffuse, RockNormal, RockRAM, RockHeight);
    Material mat2 = matArray.addMaterial(BrickDiffuse, BrickNormal, BrickRAM, BrickHeight);
    Material mat3 = matArray.addMaterial(TilesDiffuse, TilesNormal, TilesRAM, TilesHeight);
    Material mat4 = matArray.addMaterial(MetalDiffuse, MetalNormal, MetalRAM, MetalHeight);

    Material materials[] {
        mat1,mat2,mat3,mat4
    };

    std::vector<vertexData> vertices = {
            vertexData(glm::vec3(-1, -1, 0), glm::vec3(1, 1, 1), glm::vec2(0, 1)), //0
            vertexData(glm::vec3(-1, 1, 0), glm::vec3(1, 1, 1), glm::vec2(0, 0)),  //1
            vertexData(glm::vec3(1, -1, 0), glm::vec3(1, 1, 1), glm::vec2(1, 1)),  //2
            vertexData(glm::vec3(1, 1, 0), glm::vec3(1, 1, 1), glm::vec2(1, 0))    //3
    };
    std::vector<GLushort> vertexElements = {0, 1, 2, 1, 2, 3};

    Mesh rect = meshBuffer.getMesh(vertices, vertexElements);
    //suzane.setElementData(vertexElements);


    Renderer renderer;
    Camera camera;

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    double lastTime = glfwGetTime();
    float time = 0;

    std::vector<RenderObject> objects;
    srand(10);


    /*Octree octree(50 * 3);
    std::cout << "built octree" << std::endl;*/
    BVH bvh(128);

    int halfSize = 2;
    //submit a lot of suzanes
    for (int i = -halfSize; i < halfSize; i++) {
        for (int j = -halfSize; j < halfSize; j++) {
            for (int k = -halfSize; k < halfSize; k++) {
                Material &mat = materials[rand() % 4];
                Mesh &m = meshes[rand() % 4];

                Transform transform;
                transform.pos = glm::vec3(i, j, k) * 3.0f;
                transform.scale = 1;
                transform.rot = glm::quat(glm::vec3(0, 0, 0));
                objects.emplace_back(m, mat, transform);
                //octree.root.insert(&objects.back());
            }
        }
    }

    DynamicBatch batch(objects);

    /*
    for(auto& robj : objects) {
        //octree.root.insert(&robj);
        bvh.insert(&robj);
    }*/


    while(!(glfwWindowShouldClose(window) || shouldClose)) {

        double thisTime = glfwGetTime();
        double dt = thisTime - lastTime;
        lastTime = thisTime;
        time += dt;

        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float ratio = width / (float) height;

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 1000.f);

        camera.update(window, dt);
        renderer.setProjection(projection * camera.view);
        renderer.setEyePos(camera.pos);

        suzane.buffer->bindVa();
        RockDiffuse.textureArray->bind(0);

        //Rotate
        for(RenderObject& o : objects) {
            //o.transform.rot = glm::quat(glm::vec3(0, time / 2 ,0));
        }

        renderer.numObject = 0;
        //renderer.submit(bvh);
        renderer.renderBatch(batch);
        //std::cout << renderer.numObject << std::endl;
        //renderer.submit(octree.root);
        //renderer.submit(objects);

        //renderer.flushBatches();

        renderer.numObject = 0;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}