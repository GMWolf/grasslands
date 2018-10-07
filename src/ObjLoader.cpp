//
// Created by felix on 30/09/2018.
//

#include "ObjLoader.h"

#include <ios>
#include <fstream>
#include <sstream>
#include <glm.hpp>
#include <algorithm>


Mesh ObjLoader::load(MeshBuffer &buffer, const std::string &filename) {

    std::ifstream file;
    file.open(filename, std::ios::in);

    if (!file.is_open()) {
        std::cout << "Could not open file " << filename << std::endl;
        return buffer.getMesh(std::vector<vertexData>(),std::vector<GLushort >());
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;

    std::vector<vertex> data;
    std::vector<GLushort> elements;

    std::string lineHeader;
    for(std::string line; std::getline(file, line);) {
        std::istringstream iss(line);
        iss >> lineHeader;

        if (lineHeader == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        } else if (lineHeader == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            texcoords.push_back(uv);
        } else if (lineHeader == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (lineHeader == "f") {
            vertex v[3];
            iss >> v[0] >> v[1] >> v[2];
            addVertex(v[0], data, elements);
            addVertex(v[1], data, elements);
            addVertex(v[2], data, elements);
        }
    }

    std::vector<vertexData> vertexData;
    for(vertex v : data) {
        vertexData.emplace_back(positions[v.pos-1], normals[v.normal-1], texcoords[v.uv-1]);
    }

    return buffer.getMesh(vertexData, elements);
}

void ObjLoader::addVertex(const ObjLoader::vertex &v, std::vector<ObjLoader::vertex> &data,
                          std::vector<GLushort> &elements) {

    auto s = std::find(data.begin(), data.end(), v);
    if(s != data.end()) {
        elements.emplace_back(std::distance(data.begin(), s));
    } else {
        elements.emplace_back(data.size());
        data.emplace_back(v);
    }


}

std::istream &operator>>(std::istream &is, ObjLoader::vertex &rhs) {
    char bin;
    is >> rhs.pos >> bin >> rhs.uv >> bin >> rhs.normal;
    return is;
}

void ObjLoader::optimizeElements(const std::vector<GLushort> &elements) {



}
