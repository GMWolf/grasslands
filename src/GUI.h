
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
#include "Renderer.h"
#include "CUBELoader.h"

class GUI {

public:
    GUI(GLFWwindow* window, Renderer& renderer);

    void update(float ms);

    void render();
private:
    nk_context* ctx;
    Renderer& r;


    std::string LUT = "Bourbon 64";

    void loadLut(std::string& name);

    std::string luts[12] {
            "Bourbon 64",
            "Django 25",
            "Faded 47",
            "Neon 770",
            "Arabica 12",
            "Chemical 168",
            "Clouseau 54",
            "Cubicle 99",
            "Fusion 88",
            "Paladin 1875",
            "Sprocket 231",
            "Tweed 71"
    };

};


#endif //GRASSLANDS_GUI_H
