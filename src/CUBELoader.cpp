//
// Created by b5023110 on 09/11/2018.
//

#include <iostream>
#include "CUBELoader.h"

GLuint loadCubeLUT(std::string filename) {

    std::ifstream file(filename);

    if (!file) {
        std::cout << "Could not load CUBE file " << filename << std::endl;
        return 0;
    }


    unsigned int size = 0;

    std::vector<glm::u8vec3> colours;

    glm::vec3 min(0);
    glm::vec3 max(1);

    std::string line;
    while(std::getline(file, line)) {
        std::stringstream ss(line);

        std::string head;
        ss >> head;

        if (head == "LUT_3D_SIZE") {
            ss >> size;
            colours.reserve(size * size * size);
            continue;
        }

        if (head == "DOMAIN_MIN") {
            ss >> min.x >> min.y >> min.z;
            continue;
        }
        if (head == "DOMAIN_MAX") {
            ss >> max.x >> max.y >> max.z;
            continue;
        }
        if (head[0] == '#') {
            continue;
        }

        if (head == "TITLE") {
            continue;
        }

        if (head == "") {
            continue;
        }

        //Must be a values!
        std::stringstream ss2(line);
        glm::vec3 c;
        ss2 >> c.r >> c.g >> c.b;
        c.r = (c.r - min.r) / (max.r - min.r);
        c.g = (c.g - min.g) / (max.g - min.g);
        c.b = (c.b - min.b) / (max.b - min.b);

        colours.emplace_back((uint8_t)(c.r * 255), (uint8_t)(c.g * 255), (uint8_t)(c.b * 255));
        //colours.emplace_back(c);
        /*std::cout << line << std::endl;
        std::cout << c.r << " " << c.g << " " << c.b << std::endl;*/
    }

    if (colours.size() < size * size * size) {
        std::cout << "Error loading LUT " << filename << std::endl;
        return 0;
    }


    GLuint tex;
    glCreateTextures(GL_TEXTURE_3D, 1, &tex);

    glTextureStorage3D(tex, 1, GL_RGB8, size, size, size);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureSubImage3D(tex, 0, 0, 0, 0, size, size, size, GL_RGB, GL_UNSIGNED_BYTE, colours.data());

    return tex;

}
