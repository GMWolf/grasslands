//
// Created by felix on 05/10/2018.
//

#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

#define KEY_UPDATE(KEYCODE, VAR) \
switch(glfwGetKey(window, KEYCODE)) { \
case GLFW_PRESS: VAR = true; break; \
case GLFW_RELEASE: VAR = false; break; \
default:break; \
} \


void Camera::update(GLFWwindow* window, float dt, bool mouse_active) {

    KEY_UPDATE(GLFW_KEY_LEFT, leftPressed);
    KEY_UPDATE(GLFW_KEY_A, leftPressed);
    KEY_UPDATE(GLFW_KEY_RIGHT, rightPressed);
    KEY_UPDATE(GLFW_KEY_D, rightPressed);
    KEY_UPDATE(GLFW_KEY_UP, upPressed);
    KEY_UPDATE(GLFW_KEY_W, upPressed);
    KEY_UPDATE(GLFW_KEY_DOWN, downPressed);
    KEY_UPDATE(GLFW_KEY_S, downPressed);

    if (mode==FREE) {
        glm::vec3 translate(0, 0, 0);
        translate += glm::vec3(look[0]) * ((rightPressed - leftPressed) * 5.0f * dt);
        translate += glm::vec3(look[2]) * -((upPressed - downPressed) * 5.0f * dt);


        pos += translate;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float cdx = xpos - oldMouseX;
        float cdy = ypos - oldMouseY;
        oldMouseX = xpos;
        oldMouseY = ypos;

        if (mouse_active) {
            look = glm::rotate(glm::mat4(1), cdy / 500, glm::vec3(-look[0])) * look;
            look = glm::rotate(look, cdx / 500, glm::vec3(0, -1, 0));
        }

        light = 1.0f;
    } else {
        if (mode == ANIMATION) {
            currentSceneTime += dt;
            if (currentSceneTime > scenes[currentScene].time) {
                currentSceneTime = 0;
                currentScene = (currentScene + 1) % 3;
                pos = scenes[currentScene].startPos;
                look = glm::mat4(glm::quat(scenes[currentScene].startLook));
            }

            pos += scenes[currentScene].vel * dt;
            look *= glm::mat4(glm::quat(scenes[currentScene].lookVel * dt));

            //do transition effect
            const float transitionTime = 1.0f;
            if (currentSceneTime < transitionTime) {
                light = currentSceneTime / transitionTime;
            } else if (currentSceneTime > scenes[currentScene].time - transitionTime) {
                light = (scenes[currentScene].time - currentSceneTime) / transitionTime;
            } else {
                light = 1.0f;
            }
        }

    }

    view = glm::inverse(glm::translate(glm::mat4(1.0), pos) * look);

}

Camera::Camera(float ratio, float fov, float near, float far) : nearPlane(near), farPlane(far) {
    proj = glm::perspective(glm::radians(fov), ratio, near, far);
}
