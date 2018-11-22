//
// Created by felix on 05/10/2018.
//

#ifndef GRASSLANDS_CAMERA_H
#define GRASSLANDS_CAMERA_H

#include "glm.hpp"
#include <GLFW/glfw3.h>
#include <gtc/quaternion.hpp>

struct SceneInfo {
    float time;
    glm::vec3 startPos;
    glm::vec3 startLook;
    glm::vec3 vel;
    glm::vec3 lookVel;
};


class Camera {

public:

    enum {STATIC, ANIMATION, FREE};

    int mode = ANIMATION;

    Camera(float ratio, float fov, float near, float far);

    void update(GLFWwindow* window, float dt, bool mouse_active);


    struct {
        bool leftPressed : 1;
        bool rightPressed : 1;
        bool downPressed : 1;
        bool upPressed : 1;
    };

    float oldMouseX;
    float oldMouseY;


    glm::vec3 pos = glm::vec3(0,0,0);
    glm::mat4 look = glm::mat4(1);

    glm::mat4 view;
    glm::mat4 proj;

    bool freeLook = false;

    int currentScene = -1;
    float currentSceneTime = 666;
    int playAnimation = false;

    SceneInfo scenes[3] {
            {6, glm::vec3(-61.9, 3, -11.7), glm::vec3(0,0,0), glm::vec3(-1.2,0,0), glm::vec3(0,0,0)},
            {7, glm::vec3(-35.8, 1.5, 28), glm::vec3(0,3.14,0), glm::vec3(0,0.2,1), glm::vec3(0,0,0)},
            {15, glm::vec3(-9.65, 0.2, 8.27), glm::vec3(-0.2,-0.25,0), glm::vec3(0,1,0.2), glm::vec3(-0.03,0,0)}
    };

    float light;

    float nearPlane, farPlane;
};


#endif //GRASSLANDS_CAMERA_H
