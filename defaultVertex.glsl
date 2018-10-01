#version 430 core

layout(location = 0) in uint drawID;
layout(location = 1) in vec3 position;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texcoord;

layout(std430, binding = 0) buffer transformData
{
   mat4 transform[];
};

uniform mat4 MVP;

out Vertex {
    vec3 normal;
    flat uint drawID;
    vec2 texcoord;
} OUT;

void main()
{
    gl_Position = MVP * transform[drawID] * vec4(position, 1.0);
    OUT.drawID = drawID;
    OUT.normal = normal;
    OUT.texcoord = texcoord;
}