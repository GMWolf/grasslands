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


GLubyte defaultTexels[16] = {
        0, 0, 0, 255,
        255, 0, 0, 255,
        0, 255, 0, 255,
        0, 0, 255, 255
};

//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/#using-the-compressed-texture
Texture loadDDS(TextureGroup& group, const std::string filename) {

    std::cout << "loading " << filename << "\n";
    std::ifstream file;
    file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Could not open file " << filename << std::endl;
        Texture tex = group.getArray(2,2,1,GL_RGB8).getTexture();
        tex.setData(0, GL_RGBA, 0, 0, 2, 2, GL_UNSIGNED_BYTE, defaultTexels);
        return tex;
    }

    char filecode[4];
    file.read(filecode, 4);
    if(strncmp(filecode, "DDS ", 4) != 0) {
        file.close();
        std::cout << "Filecode does not match for file " << filename << std::endl;
        Texture tex = group.getArray(2,2,1,GL_RGB8).getTexture();
        tex.setData(0, GL_RGBA, 0, 0, 2, 2, GL_UNSIGNED_BYTE, defaultTexels);
        return tex;
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
            Texture tex = group.getArray(2,2,1,GL_RGB8).getTexture();
            tex.setData(0, GL_RGBA, 0, 0, 2, 2, GL_UNSIGNED_BYTE, defaultTexels);
            return tex;
    }


    Texture texture = group.getArray(width, height, mipMapCount, format).getTexture();


    const unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    //Load each mipmap
    for(unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
        unsigned int size  = ((width +3)/4)*((height +3)/4) * blockSize;
        texture.setCompressedData(level, format, 0, 0, width, height, size, buffer + offset);
        offset += size;
        width /= 2;
        height /= 2;
    }

    free(buffer);

    return texture;

}
