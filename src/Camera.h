//
// Created by felix on 05/10/2018.
//

#ifndef GRASSLANDS_CAMERA_H
#define GRASSLANDS_CAMERA_H

#include "glm.hpp"
#include <GLFW/glfw3.h>
#include <gtc/quaternion.hpp>

class Camera {


public:
    Camera(float ratio, float fov, float near, float far);


    void update(GLFWwindow* window, float dt);


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

    float nearPlane, farPlane;
};


#endif //GRASSLANDS_CAMERA_H
