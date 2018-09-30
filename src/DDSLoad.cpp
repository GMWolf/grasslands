//
// Created by felix on 30/09/2018.
//

#include "DDSLoad.h"


//http://www.roostertailgames.com/TorqueRef/t3d3_8/MakeFourCC.html
#define MakeFourCC(ch0, ch1, ch2, ch3) \
   ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) | \
   ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))

#define FOURCC_DXT1 (MakeFourCC('D','X','T','1'))
#define FOURCC_DXT3 (MakeFourCC('D','X','T','3'))
#define FOURCC_DXT5 (MakeFourCC('D','X','T','5'))


//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/#using-the-compressed-texture
GLuint loadDDS(const std::string filename) {

    std::ifstream file;
    file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Could not open file " << filename << std::endl;
        return 0;
    }

    char filecode[4];
    file.read(filecode, 4);
    if(strncmp(filecode, "DDS ", 4) != 0) {
        file.close();
        std::cout << "Filecode does not match for file " << filename << std::endl;
        return 0;
    }

    //Read header
    unsigned char header[124];
    unsigned int height, width, linearSize, mipMapCount, fourCC;

    file.read(reinterpret_cast<char *>(header), 124);
    height = *(unsigned int*)&(header[8 ]);
    width = *(unsigned int*)&(header[12]);
    linearSize = *(unsigned int*)&(header[16]);
    mipMapCount = *(unsigned int*)&(header[24]);
    fourCC = *(unsigned int*)&(header[80]);


    //Buffer data
    unsigned char* buffer;
    unsigned int bufsize;

    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = reinterpret_cast<unsigned char *>(malloc(bufsize * sizeof(unsigned char)));
    file.read(reinterpret_cast<char *>(buffer), bufsize);

    file.close();

    unsigned int format;
    switch(fourCC) {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;

        default:
            free(buffer);
            std::cout << "Unknown dds format: " << fourCC << std::endl;
            return false;
    }

    GLuint textureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureStorage2D(textureID, mipMapCount, format, width, height);

    const unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    //Load each mipmap
    for(unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
        unsigned int size  = ((width +3)/4)*((height +3)/4) * blockSize;
        glCompressedTextureSubImage2D(textureID, level, 0, 0, width, height, format, size, buffer + offset);
        offset += size;
        width /= 2;
        height /= 2;
    }

    free(buffer);

    return textureID;

}
