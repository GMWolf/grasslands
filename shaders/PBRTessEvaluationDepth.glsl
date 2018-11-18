#version 430

#define E 2.71827

layout(triangles, fractional_even_spacing, ccw) in;

in Vertex {
       vec3 normal;
       flat uint drawID;
       vec2 texcoord;
       vec3 viewVector;
       float texSize;
} IN[];

noperspective out float depth;


struct material {
     ivec2 diffuse;
     ivec2 normal;
     ivec2 ram;
     ivec2 disp;
     float dispFactor;
     float matScale;
     float heightScale;
     bool normalsFromHeight;
 };


struct Transform
 {
       vec3 pos;
       float scale;
       vec4 rot;
};

layout(std430, binding = 0) buffer transformBuffer
{
    Transform ModelTransform[];
};

layout(std430, binding = 1) buffer MaterialIndexBuffer {
    uint materialIndex[];
};

layout(std430, binding = 2) buffer MaterialDataBuffer {
    material materials[];
};

uniform sampler2DArray tex[8];

uniform mat4 MV;

uniform vec3 eyePos;


vec4 matTexture(ivec2 t, vec2 texcoord) {
    return texture(tex[t.x], vec3(texcoord, t.y));
}

vec4 matTextureLod(ivec2 t, vec2 texcoord, float lod) {
    return textureLod(tex[t.x], vec3(texcoord, t.y), lod);
}

void main() {

    Transform t = ModelTransform[IN[0].drawID];

    vec3 p0 = gl_TessCoord.x * gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_TessCoord.y * gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_TessCoord.z * gl_in[2].gl_Position.xyz;
    vec3 pos = p0 + p1 + p2;

    vec3 n0 = gl_TessCoord.x * IN[0].normal;
    vec3 n1 = gl_TessCoord.y * IN[1].normal;
    vec3 n2 = gl_TessCoord.z * IN[2].normal;
    vec3 normal = normalize(n0 + n1 + n2);

    vec2 tc0 = gl_TessCoord.x * IN[0].texcoord;
    vec2 tc1 = gl_TessCoord.y * IN[1].texcoord;
    vec2 tc2 = gl_TessCoord.z * IN[2].texcoord;
    vec2 texcoord = tc0 + tc1 + tc2;

    float ts0 = gl_TessCoord.x * IN[0].texSize;
    float ts1 = gl_TessCoord.y * IN[1].texSize;
    float ts2 = gl_TessCoord.z * IN[2].texSize;
    float texSize = ts0 + ts1 + ts2;


    //float pinchEdge = 1.0;
    float pinchEdge = gl_TessCoord.x * gl_TessCoord.y * gl_TessCoord.z;
    pinchEdge = smoothstep(0, 1, pinchEdge * 500);

    material mat = materials[materialIndex[IN[0].drawID]];

    float levels = textureQueryLevels(tex[mat.disp.x]);
    float lodLevel = levels - log2(texSize);

    float d = matTextureLod(mat.disp, texcoord * mat.heightScale, 0/* lodLevel*/).x;
    d = (d * 2.0) - 1;
    d *= mat.dispFactor ;

    pos += normal * d * t.scale;

    gl_Position = MV * vec4(pos, 1.0);
    depth = gl_Position.z;
}
