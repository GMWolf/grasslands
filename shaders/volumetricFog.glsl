#version 430



uniform sampler2D shadowMap;
uniform sampler2DMS depthMap;
uniform sampler2DMS inColour;

in vert {
    vec2 texCoord;
} IN;

out vec4 outColor;

uniform mat4 invMat;
uniform vec3 eyePos;

uniform mat4 shadowVP;
uniform vec2 size;

#define SAMPLES 128

#include "../shaders/shadow.glsl"

void main() {

    outColor = texelFetch(inColour, ivec2(size * IN.texCoord), 0);

    float depth = texelFetch(depthMap, ivec2(size * IN.texCoord), 0).r;
    depth = depth * 2.0 - 1.0;
    vec2 tc = IN.texCoord * 2.0 - 1.0;
    vec4 clipSpace = vec4(tc, depth, 1.0);

    vec4 worldSpacePos = invMat * clipSpace;

    worldSpacePos /= worldSpacePos.w;

    vec3 d = worldSpacePos.xyz - eyePos;

    float camDist = length(d);

    d = normalize(d);

    float stepl = (20.0f / SAMPLES);
    vec3 step = d * stepl;

    float isamples = 1.f / SAMPLES;

    float light = 0;

    vec3 pos = eyePos;

    float weight = 1.0;
    float decay = 0.998;

    for(int i = 0; i < SAMPLES; i++) {

        float dist = (i * stepl);
        if (dist > camDist){
            break;
        }
        float s = shadowIntensity(pos);
        light += s * isamples * weight;

        pos += step;
        weight *= decay;
    }

    light *= light;

    light *= 0.5;



    outColor += vec4(light, light, light, 0.0);

    //outColor = vec4(d, 1.0);

}
