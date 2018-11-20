#version 430

in vert {
    vec2 texCoord;
} IN;

out vec4 outColor;

uniform sampler2D tex;
uniform vec2 size;

void main() {
    outColor = texture(tex, IN.texCoord);
}
