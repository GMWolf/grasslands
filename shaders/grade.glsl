#version 430

uniform sampler3D LUT;
uniform sampler2D tex;
uniform vec2 size;

uniform float lutSize;

in vert {
    vec2 texCoord;
} IN;

out vec4 outColor;

void main() {

    vec3 inColour = texture(tex, IN.texCoord).xyz;
    float scale = (lutSize - 1.0) / lutSize;
    float offset = 1.0 / (2.0 * lutSize);

    outColor = vec4(texture(LUT, offset + inColour * scale).rgb, 1.0);
}
