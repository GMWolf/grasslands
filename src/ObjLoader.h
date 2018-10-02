//
// Created by felix on 30/09/2018.
//

#ifndef GRASSLANDS_OBJLOADER_H
#define GRASSLANDS_OBJLOADER_H

#include <iostream>
#include <string>
#include <vector>
#include "Mesh.h"

class ObjLoader {

    struct vertex {
        bool operator==(const vertex& rhs) const {
            return (pos == rhs.pos) && (uv == rhs.uv) && (normal == rhs.normal);
        }
        unsigned int pos;
        unsigned int uv;
        unsigned int normal;
    };


    friend std::istream& operator>>(std::istream& is, vertex& rhs);

    static void addVertex(const vertex& v, std::vector<vertex>& data, std::vector<GLushort>& elements);

    static void optimizeElements(const std::vector<GLushort>& elements);

public:



    static Mesh load(MeshBuffer& buffer, const std::string& file);

};


std::istream& operator>>(std::istream& is, ObjLoader::vertex& rhs);




#endif //GRASSLANDS_OBJLOADER_H
