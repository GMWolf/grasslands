
//
// Created by Felix Bridault on 20/11/2018.
//
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "GUI.h"
#include <string>
#include <sstream>

GUI::GUI(GLFWwindow *window, Renderer& r) : r(r) {
    ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
    nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();
}

void GUI::update(float ms) {
    nk_glfw3_new_frame();

    float fps = 1 / ms;

    if(nk_begin(ctx, "fps", nk_rect(0,0,100,40), NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_static(ctx, 0, 50, 1);
        nk_label(ctx,  ("fps: " + std::to_string((int)fps)).c_str(), NK_TEXT_LEFT);
    }
    nk_end(ctx);


    if (nk_begin(ctx, "Debug", nk_rect(5, 50, 220, 150),
                 NK_WINDOW_MINIMIZABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE|NK_WINDOW_MOVABLE)) {
        nk_layout_row_static(ctx, 0, 80, 1);
        nk_checkbox_label(ctx, "View tiles", &r.showLightDebug);

        nk_layout_row_static(ctx, 0, 80, 2);
        //choose lut
        nk_label(ctx, "LUT:", NK_TEXT_LEFT);
        if(nk_combo_begin_label(ctx, LUT.c_str(), nk_vec2(200, 200))) {
            nk_layout_row_static(ctx, 0, 150, 1);
            for(auto& s : luts) {
                if(nk_button_label(ctx, s.c_str())) {
                    LUT = s;
                    loadLut(s);
                }
            }
            nk_combo_end(ctx);
        }

        std::stringstream pposstream;
        pposstream.precision(3);
        pposstream << "playerpos  x:" << r.eyePos.x << " y:" << r.eyePos.z;

        nk_layout_row_static(ctx, 0, 200, 1);
        nk_label(ctx, pposstream.str().c_str(), NK_TEXT_LEFT);

    }
    nk_end(ctx);
}

void GUI::render() {
    nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
}

void GUI::loadLut(std::string &name) {
    glDeleteTextures(1, &r.LUT);
    r.LUT = loadCubeLUT("../LUTs/" + name + ".CUBE");
}
