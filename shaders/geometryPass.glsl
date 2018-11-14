#version 430

struct material {
    ivec2 diffuse;
    ivec2 normal;
    ivec2 ram;
};

in Vertex {
    vec3 normal;
    flat uint drawID;
    vec2 texcoord;
    vec3 viewVector;
    noperspective vec3 worldPos;
} IN;

layout (location = 0) out vec3 albedoOut;
layout (location = 1) out vec3 normalOut;
layout (location = 2) out vec3 RAMOut;


layout(std430, binding = 1) buffer MaterialIndexBuffer {
    uint materialIndex[];
};

layout(std430, binding = 2) buffer MaterialDataBuffer {
    material materials[];
};

uniform sampler2DArray tex[8];

vec4 matTexture(ivec2 t, vec2 texcoord) {
    return texture(tex[t.x], vec3(texcoord, t.y));
}

#include "../shaders/normals.glsl"

void main() {

    material mat = materials[materialIndex[IN.drawID]];

    RAMOut= matTexture(mat.ram, IN.texcoord).xyz;
    albedoOut = matTexture(mat.diffuse, IN.texcoord).xyz;

    vec3 N = normalize(IN.normal);
    vec3 normalMap = matTexture(mat.normal, IN.texcoord).xyz * 2.0 - 1.0;
    normalMap *= vec3(1,-1,1);

    normalOut = perturb_normal(N, IN.viewVector, IN.texcoord, normalMap);
    //normalOut = vec3(1, 0.0, 0.0);
}
