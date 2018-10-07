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


    if(!glfwInit()) {
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    GLFWwindow* window;


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    //glfwGetPrimaryMonitor()
    window = glfwCreateWindow(1280, 720, "Grasslands",  NULL, NULL);



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
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );


    MeshBuffer meshBuffer;
    TextureGroup group;

    Mesh suzane = ObjLoader::load(meshBuffer, "../suzane.obj");
    Mesh knot = ObjLoader::load(meshBuffer, "../knot.obj");
    Mesh box = ObjLoader::load(meshBuffer, "../Box.obj");
    Mesh gear = ObjLoader::load(meshBuffer, "../gear.obj");

    Mesh meshes[4] ={
            suzane, knot, box, gear
    };



    Texture tex = loadDDS(group, "../texture.dds");
    Texture tex2 = loadDDS(group, "../diffuse_1.DDS");



    std::vector<vertexData> vertices = {
            vertexData(glm::vec3(-1, -1, 0), glm::vec3(1,1,1), glm::vec2(0,1)), //0
            vertexData(glm::vec3(-1, 1, 0), glm::vec3(1,1,1), glm::vec2(0,0)),  //1
            vertexData(glm::vec3(1, -1, 0), glm::vec3(1,1,1), glm::vec2(1,1)),  //2
            vertexData(glm::vec3(1, 1, 0), glm::vec3(1,1,1), glm::vec2(1,0))    //3
    };
    std::vector<GLushort> vertexElements = {0,1,2,1,2,3};

    Mesh rect = meshBuffer.getMesh(vertices, vertexElements);
    //suzane.setElementData(vertexElements);


    Renderer renderer;
    Camera camera;

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    double lastTime = glfwGetTime();
    float time = 0;

    std::vector<RenderObject> objects;
    srand(10);

    int halfSize = 15;
    //submit a lot of suzanes
    for(int i = -halfSize; i < halfSize; i++)
        for(int j = -halfSize; j < halfSize; j++)
            for(int k = -halfSize; k < halfSize; k++) {
                Texture& t = rand() % 2 == 0 ? tex : tex2;
                Mesh& m = meshes[rand() % 4];

                Transform transform;
                transform.pos = glm::vec3(i, j, k) * 3.0f;
                transform.scale = 1;
                transform.rot = glm::quat(glm::vec3(0, 0, 0));
                objects.emplace_back(m, t, transform);
            }

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
        projection = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 100.f);

        camera.update(window, dt);
        renderer.setProjection(projection * camera.view);

        suzane.buffer->bindVa();
        tex.textureArray->bind(0);

        //Rotate
        for(RenderObject& o : objects) {
            o.transform.rot = glm::quat(glm::vec3(0, time / 2 ,0));
        }

        renderer.submit(objects);

        renderer.flushBatches();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}