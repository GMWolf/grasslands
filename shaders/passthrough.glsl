#version 430

in vert {
    vec2 texCoord;
} IN;

out vec4 outColor;

uniform sampler2DMS tex;
uniform vec2 size;

void main() {
    outColor = texelFetch(tex, ivec2(IN.texCoord * size), 0);
}
