#version 430 core

uniform vec2 pixelSize;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in Vertex {
    float size;
    vec3 colour;
}IN[];

out Vertex {
    vec2 texCoord;
    vec3 colour;
} OUT;

void main() {

    for(int i = 0; i < gl_in.length(); i++) {



        gl_Position = gl_in[i].gl_Position + vec4(vec2(-1, -1) * IN[i].size * pixelSize, 0, 0);
        OUT.texCoord = vec2(0, 0);
        OUT.colour = IN[i].colour;
        EmitVertex();

        gl_Position = gl_in[i].gl_Position + vec4(vec2(-1, 1) * IN[i].size * pixelSize, 0, 0);
        OUT.texCoord = vec2(0, 1);
        OUT.colour = IN[i].colour;
        EmitVertex();

        gl_Position = gl_in[i].gl_Position + vec4(vec2(1, -1) * IN[i].size * pixelSize, 0, 0);
        OUT.texCoord = vec2(1, 0);
        OUT.colour = IN[i].colour;
        EmitVertex();

        gl_Position  = gl_in[i].gl_Position + vec4(vec2(1, 1) * IN[i].size * pixelSize, 0, 0);
        OUT.texCoord = vec2(1, 1);
        OUT.colour = IN[i].colour;
        EmitVertex();

        EndPrimitive();
    }
}
