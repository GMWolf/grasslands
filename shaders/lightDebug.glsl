#version 430


#include "../shaders/ForwardPlus.glsl"



layout(std430, binding = 0) readonly buffer VisibleLightBuffer {
    TileLightData tileData[];
};

out vec4 outColor;

void main() {

    ivec2 tilePos = ivec2(gl_FragCoord.xy) / ivec2(16,16);
    uint tileIndex = tilePos.y * (tileCountX) + tilePos.x;

    TileLightData tld = tileData[tileIndex];

    outColor.xyz = vec3(tileData[tileIndex].lightCount / 16.0f ) ;

}
