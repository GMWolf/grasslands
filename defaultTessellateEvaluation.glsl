#version 430

layout(triangles, equal_spacing, ccw) in;

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

    //TODO : move vertices

    gl_Position = MV * vec4(pos, 1.0);

    OUT.normal = normal;
    OUT.drawID = IN[0].drawID;
    OUT.texcoord = /* vec2(1.0, 0.3);//*/texcoord;
    OUT.viewVector = eyePos - pos;//viewVector;
}
