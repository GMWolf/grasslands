#version 430

#define E 2.71827

layout(triangles, fractional_even_spacing, ccw) in;

in VertexIn {
       vec3 normal;
       flat uint drawID;
       vec2 texcoord;
       vec3 viewVector;
} IN[];

out Vertex {
        vec3 normal;
        flat uint drawID;
        vec2 texcoord;
        vec3 viewVector;
} OUT;

struct material {
    uint diffuse;
    uint normal;
    uint ram;
    uint disp;
};

layout(std430, binding = 1) buffer MaterialIndexBuffer {
    uint materialIndex[];
};

layout(std430, binding = 2) buffer MaterialDataBuffer {
    material materials[];
};

uniform sampler2DArray tex;

uniform mat4 MV;

uniform vec3 eyePos;

void main() {

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

    vec3 vv0 = gl_TessCoord.x * IN[0].viewVector;
    vec3 vv1 = gl_TessCoord.y * IN[1].viewVector;
    vec3 vv2 = gl_TessCoord.z * IN[2].viewVector;
    vec3 viewVector = vv0 + vv1 + vv2;

    float pinchEdge = gl_TessCoord.x * gl_TessCoord.y * gl_TessCoord.z;
    pinchEdge = smoothstep(0, 1, pinchEdge * 500);

    material mat = materials[materialIndex[IN[0].drawID]];
    float d = textureLod(tex, vec3(texcoord, mat.disp),1).x;
    d = (d * 2.0) - 1;
    d *= 0.02 * pinchEdge;

    pos += normal * d;

    gl_Position = MV * vec4(pos, 1.0);

    OUT.normal = normal;
    OUT.drawID = IN[0].drawID;
    OUT.texcoord = /* vec2(1.0, 0.3);//*/texcoord;
    OUT.viewVector = eyePos - pos;//viewVector;
}
