#version 430
#define PI 3.1415926535897932384626433832795


in vert {
    vec2 texCoord;
} IN;

uniform sampler2D shadowMap;

uniform mat4 shadowVP;

uniform vec3 lightDir;
uniform vec3 lightColour;

uniform sampler2DMS albedoTex;
uniform sampler2DMS depthTex;
uniform sampler2DMS normalTex;
uniform sampler2DMS RAMTex;
uniform vec2 size;
uniform mat4 invProjMat;

out vec4 outColor;

#include "../shaders/PBR.glsl"
#include "../shaders/shadow.glsl"


vec3 viewPosFromDepth(float depth, vec2 texCoord) {
    float z = depth * 2.0 - 1.0;

    vec4 p = invProjMat * vec4(texCoord * 2.0 - 1.0, z, 1.0);
    p /= p.w;

    return p.xyz;
}

void main() {

    vec3 albedo = texelFetch(albedoTex, ivec2(size * IN.texCoord), 0).xyz;
    float depth = texelFetch(depthTex, ivec2(size * IN.texCoord), 0).r;
    vec3 N = texelFetch(normalTex, ivec2(size * IN.texCoord), 0).xyz;
    vec3 RAM = texelFetch(RAMTex, ivec2(size * IN.texCoord), 0).xyz;

    float roughness = RAM.x;
    float AO = RAM.y;
    float metalic = RAM.z;

    vec3 view_vector = viewPosFromDepth(depth, IN.texCoord);

    vec3 V = normalize(view_vector);

    vec3 L = normalize(lightDir);

    vec3 H = normalize(L + V);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metalic);
    vec3 F = fresnel(max(dot(H, V), 0.0), F0);

    float NDF = D_GGX(N, H, roughness);
    float G = G_smith(N, V, L, roughness);

    vec3 numerator = NDF * G * F;
    float denom = 4.0 * max(dot(N,V),0.0) * max(dot(N,L), 0.0);
    vec3 specular = numerator / max(denom, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalic;

    float NdotL = max(dot(N, L), 0.0);

    vec3 radiance = lightColour;
    vec3 light = (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.25) * albedo * AO;

//    light *= shadowIntensity(pos, normal * 0.001);


    outColor = vec4(light + ambient, 1.0);
}
