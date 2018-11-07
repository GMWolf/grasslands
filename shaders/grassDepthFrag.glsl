#version 430

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

in Vertex {
    flat uint drawID;
    vec2 texcoord;
    noperspective float depth;
} IN;

out vec4 outColor;

uniform sampler2DArray tex[8];


vec4 matTexture(ivec2 t, vec2 texcoord) {
    return texture(tex[t.x], vec3(texcoord, t.y));
}

void main() {

    material mat = materials[materialIndex[IN.drawID]];
    vec3 RA = matTexture(mat.roughAlpha, IN.texcoord).xyz;

    float roughness = RA.r;
    float Alpha = RA.g;

    if (Alpha < 0.45) {
        discard;
    }

     float z = IN.depth;
      float z2 = z * z;
      float z3 = z2 * z;
      float z4 = z3 * z;
      outColor.x = z;
      outColor.y = z2;
      outColor.z = z3;
      outColor.w = z4;
}
