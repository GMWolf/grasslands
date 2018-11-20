
//
// Created by Felix Bridault on 20/11/2018.
//

#ifndef GRASSLANDS_GUI_H
#define GRASSLANDS_GUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_KEYSTATE_BASED_INPUT
#include <nuklear/nuklear.h>
#include <nuklear/nuklear_glfw_gl3.h>


class GUI {

public:
    GUI(GLFWwindow* window);

    void update(float ms);

    void render();


    bool viewTiles = false;

private:
    nk_context* ctx;

};


#endif //GRASSLANDS_GUI_H
