#version 430

layout(location = 0) in uint drawID;
layout(location = 1) in vec3 position;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texcoord;


out Vertex {
    vec3 normal;
    flat uint drawID;
    vec2 texcoord;
    vec3 viewVector;
} OUT;


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


uniform vec3 eyePos;

vec3 rotate(vec3 vec, vec4 quat) {
    vec3 t = 2 * cross(quat.xyz, vec);
    return vec + quat.w * t + cross(quat.xyz, t);
}

void main() {
    Transform t = ModelTransform[drawID];

    vec3 pos = (rotate(position, t.rot) * t.scale) + t.pos;

    gl_Position = vec4(pos, 1.0);
    OUT.drawID = drawID;
    OUT.texcoord = texcoord;
    OUT.normal = rotate(normal, t.rot);

    OUT.viewVector = eyePos - pos;
}
