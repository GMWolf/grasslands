#version 430

layout(location = 0) in uint drawID;
layout(location = 1) in vec3 position;
layout(location = 2) in vec3 normal;
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
uniform vec3 eyePos;
uniform float time;


out Vertex {
    vec3 normal;
    flat uint drawID;
    vec2 texcoord;
    vec3 viewVector;
    noperspective vec3 worldPos;
} OUT;


vec3 rotate(vec3 vec, vec4 quat) {
    vec3 t = 2 * cross(quat.xyz, vec);
    return vec + quat.w * t + cross(quat.xyz, t);
    //return vec + 2.0 * cross( cross(quat.xyz, vec) + quat.w * vec, quat.xyz);
}

void main()
{
    Transform t = ModelTransform[drawID];

    vec3 pos = (rotate(position, t.rot) * t.scale) + t.pos + vec3(0.025,0,0) * (sin(time + position.x) * position.y);
    gl_Position = MV * vec4(pos, 1.0);
    OUT.drawID = drawID;
    OUT.normal = rotate(normal, t.rot);
    OUT.texcoord = texcoord;
    OUT.texcoord.y *= -1;
    OUT.viewVector = eyePos - pos;
    OUT.worldPos = pos;
}