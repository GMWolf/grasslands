//
// Created by felix on 30/09/2018.
//

#ifndef GRASSLANDS_DDSLOAD_H
#define GRASSLANDS_DDSLOAD_H


#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <ios>
#include "Texture.h"



Texture loadDDS(TextureGroup& group, const std::string filename);

Texture brokenTexture(TextureGroup& group);

#endif //GRASSLANDS_DDSLOAD_H
