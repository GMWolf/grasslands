#version 430 core

layout(location = 0) in uint drawID;
layout(location = 1) in vec3 position;
layout(location = 3) in vec2 texcoord;

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


uniform mat4 MV;

out Vertex {
    flat uint drawID;
    vec2 texcoord;
    noperspective float depth;
} OUT;


vec3 rotate(vec3 vec, vec4 quat) {
    vec3 t = 2 * cross(quat.xyz, vec);
    return vec + quat.w * t + cross(quat.xyz, t);
    //return vec + 2.0 * cross( cross(quat.xyz, vec) + quat.w * vec, quat.xyz);
}

void main()
{
    Transform t = ModelTransform[drawID];

    vec3 pos = (rotate(position, t.rot) * t.scale) + t.pos;
    gl_Position = MV * vec4(pos, 1.0);
    OUT.drawID = drawID;
    OUT.texcoord = texcoord;
    OUT.texcoord.y *= -1;
    OUT.depth = gl_Position.z;
}