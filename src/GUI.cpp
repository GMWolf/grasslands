
//
// Created by Felix Bridault on 20/11/2018.
//
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "GUI.h"
#include <string>

GUI::GUI(GLFWwindow *window) {
    ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
    nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();
}

void GUI::update(float ms) {
    nk_glfw3_new_frame();

    float fps = 1 / ms;


    if (nk_begin(ctx, ("fps: " + std::to_string((int)fps)).c_str(), nk_rect(50, 50, 150, 100),
                 NK_WINDOW_MINIMIZABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE)) {
        nk_layout_row_static(ctx, 30, 80, 1);
        nk_checkbox_label(ctx, "View tiles", (int*)&viewTiles);

    }
    nk_end(ctx);
}

void GUI::render() {
    nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
}
