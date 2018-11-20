
//
// Created by Felix Bridault on 20/11/2018.
//
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "GUI.h"

GUI::GUI(GLFWwindow *window) {
    ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
    nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();
}

void GUI::update(float ms) {
    nk_glfw3_new_frame();
    if (nk_begin(ctx, "yo", nk_rect(50, 50, 230, 250),
                 NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE)) {
    }
    nk_end(ctx);
}

void GUI::render() {
    nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
}
