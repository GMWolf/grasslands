#version 430



uniform sampler2D shadowMap;
uniform sampler2DMS depthMap;
uniform sampler2DMS inColour;

in vert {
    vec2 texCoord;
} IN;

out vec4 outColor;

uniform mat4 viewProj;
uniform mat4 invMat;
uniform vec3 eyePos;

uniform mat4 shadowVP;
uniform vec2 size;

uniform float time;

#define SAMPLES 512

#include "../shaders/shadow.glsl"
#include "../shaders/ForwardPlus.glsl"

layout(std430, binding = 0) readonly buffer LightDataBuffer {
  uint lightCount, pad0, pad1, pad2;
  Light lights[];
};

layout(std430, binding = 1) readonly buffer VisibleLightBuffer {
    TileLightData tileData[];
};

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

    d /= camDist;

    float stepl = (30.0f / SAMPLES);
    vec3 step = d * stepl;

    float isamples = 1.f / SAMPLES;

    float light =0.0f;

    vec3 pos = eyePos;

    float weight = 1.0;
    float decay = 0.9998;

    ivec2 tilePos = ivec2(gl_FragCoord.xy) / ivec2(16,16);
    uint tileIndex = tilePos.y * (tileCountX) + tilePos.x;

    TileLightData tld = tileData[tileIndex];

    for(int i = 0; i < SAMPLES; i++) {

        float dist = (i * stepl);
        if (dist > camDist){
            break;
        }
        float s = shadowIntensity(pos);
        float n = 1;//clamp((1 + snoise(vec4(pos * 0.3, time))) * 0.5, 0, 1);

        outColor.xyz *= 1.0 - ((1.0 - vec3(0.7, 0.7, 0.8)) * (2.0 / SAMPLES));
        outColor.xyz += vec3(0.7, 0.7, 0.8) *  s * isamples * weight;// * pow(n, 0.5);

        pos += step;
       weight *= decay;
    }

    //light *= light;

    //DO point lights in screen space
    /*for(int i = 0; i < tld.lightCount; i++) {
        Light l = lights[tld.visibleLightIndex[i]];
        vec4 clipPos = (viewProj * vec4(l.posRad.xyz, 1.0));
        float clipRadius = l.posRad.w / clipPos.w;
        clipPos /= clipPos.w;

        if (clipPos.z > depth) {
            continue;
        }

        vec2 L = clipPos.xy - ((IN.texCoord - 0.5) * 2.0);
        float intensity = l.colorI.w * (max(clipRadius - length(L), 0.0f) / clipRadius);
        light += l.colorI.xyz * intensity * 0.25;
     }*/


    //light *= 0.5;

  // outColor  += vec4(vec3(0.7, 0.7, 0.8) * light, 0.0);
}
