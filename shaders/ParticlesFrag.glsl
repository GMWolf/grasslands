#version 430

in Vertex {
    vec2 texCoord;
    vec3 colour;
} IN;

uniform sampler2DArray tex[8];

uniform ivec2 t;

out vec4 outColor;

void main() {
    outColor = texture(tex[t.x], vec3(IN.texCoord, t.y)) * vec4(IN.colour, 1.0);
}
