#version 450

#define PI 3.1415926535897932384626433832795

struct material {
    ivec2 albedo;
    ivec2 normal;
    ivec2 roughAlpha;
    ivec2 translucency;
};


layout(std430, binding = 1) buffer MaterialIndexBuffer {
    uint materialIndex[];
};

layout(std430, binding = 2) buffer MaterialDataBuffer {
    material materials[];
};

#include "../shaders/ForwardPlus.glsl"

layout(std430, binding = 3) readonly buffer LightDataBuffer {
  uint lightCount, pad0, pad1, pad2;
  Light lights[];
};

layout(std430, binding = 4) readonly buffer VisibleLightBuffer {
    TileLightData tileData[];
};

uniform sampler2DArray tex[8];
uniform sampler2D shadowMap;

uniform mat4 shadowVP;

uniform vec3 lightDir;
uniform vec3 lightColour;

in Vertex {
    vec3 normal;
    flat uint drawID;
    vec2 texcoord;
    vec3 viewVector;
    noperspective vec3 worldPos;
} IN;

out vec4 outColor;


#include "../shaders/normals.glsl"
#include "../shaders/PBR.glsl"
#include "../shaders/shadow.glsl"


vec4 matTexture(ivec2 t, vec2 texcoord) {
    return texture(tex[t.x], vec3(texcoord, t.y));
}

vec3 computeLight(vec3 L, vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, vec3 translucency, vec3 colour, float intensity) {

    vec3 H = normalize(L + V);

    vec3 F = fresnel(max(dot(H, V), 0.0), F0);
    float NDF = D_GGX(N, H, roughness);
    float G = G_smith(N, V, L, roughness);

    vec3 numerator = NDF * G * F;

    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denom, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    float NdotL = max(dot(N, L), 0.0);
    vec3 radiance = colour * intensity;

    //translucency
    float NdotLI = min(max(dot(-N, L), 0), 1);

    float EdotL = min(max(dot(V, -L), 0.0), 1);

    vec3 bounce = (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 transmit = (EdotL * NdotLI) * translucency * radiance * albedo ;

    return bounce + transmit;
}


void main()
{

    material mat = materials[materialIndex[IN.drawID]];
    vec3 RA = matTexture(mat.roughAlpha, IN.texcoord).xyz;

    float roughness = RA.r;
    float Alpha = RA.g;

    if (Alpha < 0.45) {
        discard;
    }

    vec3 albedo = matTexture(mat.albedo, IN.texcoord).xyz;
    float albedoSize = length(albedo);
    albedo.r += mix(-0.05, 0.05, IN.drawID / 8000.f);
    albedo = normalize(albedo) * albedoSize;

    vec3 N = normalize(IN.normal);
    vec3 normalMap = normalize(matTexture(mat.normal, IN.texcoord).xyz * 2.0 - 1.0);
    normalMap *= vec3(1,-1,1);

    //N = perturb_normal(N, IN.viewVector, IN.texcoord, normalMap);

    vec3 V = normalize(IN.viewVector);


    vec3 F0 = vec3(0.04);

    vec3 L = normalize(lightDir);

    vec3 translucency = matTexture(mat.translucency, IN.texcoord).xyz;

     float shadow = shadowIntensity(IN.worldPos + IN.normal * 0.001);
    vec3 lightAccumulation = computeLight(L, N, V, F0, albedo, roughness, translucency, lightColour, shadow);

    ivec2 tilePos = ivec2(gl_FragCoord.xy) / ivec2(16,16);
    uint tileIndex = tilePos.y * (tileCountX) + tilePos.x;

    TileLightData tld = tileData[tileIndex];


    for(uint i = 0; i < tld.lightCount; i++) {
        Light l = lights[tld.visibleLightIndex[i]];
        L = IN.worldPos - l.posRad.xyz;
        L = -L;
        float intensity = l.colorI.w * (max(l.posRad.w - length(L), 0.0f) / l.posRad.w);
        L = normalize(L);
        lightAccumulation += computeLight(L, N, V, F0, albedo, roughness, translucency, l.colorI.xyz, intensity);
    }

    outColor = vec4(lightAccumulation, Alpha);


    if (showLightDebug) {
        outColor.xyz = vec3(tileData[tileIndex].lightCount / 16.0f ) ;
    }

}
