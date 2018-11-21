#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in float size;
layout(location = 2) in vec3 colour;


out Vertex {
    float size;
    vec3 colour;
}OUT;

uniform mat4 VP;

void main() {

    gl_Position = VP * vec4(position, 1.0);

    OUT.size = size;
    OUT.colour = colour;
}
