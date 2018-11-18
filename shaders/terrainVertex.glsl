#version 430 core

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
uniform sampler2DArray tex[8];


struct material {
    ivec2 diffuse;
    ivec2 normal;
    ivec2 ram;
    ivec2 disp;
};

layout(std430, binding = 1) buffer MaterialIndexBuffer {
    uint materialIndex[];
};

layout(std430, binding = 2) buffer MaterialDataBuffer {
    material materials[];
};

out Vertex {
    vec3 normal;
    flat uint drawID;
    vec2 texcoord;
    vec3 viewVector;
    vec3 worldPos;
} OUT;

vec3 rotate(vec3 vec, vec4 quat) {
    vec3 t = 2 * cross(quat.xyz, vec);
    return vec + quat.w * t + cross(quat.xyz, t);
}


vec4 matTexture(ivec2 t, vec2 texcoord) {
    return texture(tex[t.x], vec3(texcoord, t.y));
}

void main()
{
    material mat = materials[materialIndex[drawID]];

    Transform t = ModelTransform[drawID];

    vec3 pos = (rotate(position, t.rot) * t.scale) + t.pos;
    pos.y += matTexture(mat.disp, texcoord).r;

    gl_Position = MV * vec4(pos, 1.0);
    OUT.drawID = drawID;
    OUT.normal = rotate(normal, t.rot);
    OUT.texcoord = texcoord;
    OUT.viewVector = eyePos - pos;
    OUT.worldPos = pos;
}