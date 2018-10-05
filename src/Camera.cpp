//
// Created by felix on 05/10/2018.
//

#include <ext/matrix_transform.hpp>
#include "Camera.h"

#define KEY_UPDATE(KEYCODE, VAR) \
switch(glfwGetKey(window, KEYCODE)) { \
case GLFW_PRESS: VAR = true; break; \
case GLFW_RELEASE: VAR = false; break; \
default:break; \
} \


void Camera::update(GLFWwindow* window, float dt) {

    KEY_UPDATE(GLFW_KEY_LEFT, leftPressed);
    KEY_UPDATE(GLFW_KEY_RIGHT, rightPressed);
    KEY_UPDATE(GLFW_KEY_UP, upPressed);
    KEY_UPDATE(GLFW_KEY_DOWN, downPressed);

    glm::vec3 translate;
    translate.x = (rightPressed - leftPressed) * 5.0f * dt;
    translate.y = 0;
    translate.z = (upPressed - downPressed) * 5.0f * dt;

    pos += translate;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    float cdx = xpos - oldMouseX;
    float cdy = ypos - oldMouseY;
    oldMouseX = xpos;
    oldMouseY = ypos;

    look = glm::rotate(glm::mat4(1), cdy / 500, glm::vec3(look[0])) * look;
    look = glm::rotate(look, cdx / 500, glm::vec3(0,0,1));


    view = glm::inverse(glm::translate(look, pos));
}
