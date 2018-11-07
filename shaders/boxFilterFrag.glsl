#version 430

in vert {
    vec2 texCoord;
} IN;

out vec4 outColor;

uniform sampler2D tex;
uniform vec2 sampleSize;

void main() {
    float i = 1.0f / 9.0f;
    outColor += texture(tex, IN.texCoord + vec2(-sampleSize.x, -sampleSize.y)) * i;
    outColor += texture(tex, IN.texCoord + vec2(-sampleSize.x, 0)) * i;
    outColor += texture(tex, IN.texCoord + vec2(-sampleSize.x, sampleSize.y)) * i;
    outColor += texture(tex, IN.texCoord + vec2(0, -sampleSize.y)) * i;
    outColor += texture(tex, IN.texCoord + vec2(0, 0)) * i;
    outColor += texture(tex, IN.texCoord + vec2(0, sampleSize.y)) * i;
    outColor += texture(tex, IN.texCoord + vec2(sampleSize.x, -sampleSize.y)) * i;
    outColor += texture(tex, IN.texCoord + vec2(sampleSize.x, 0)) * i;
    outColor += texture(tex, IN.texCoord + vec2(sampleSize.x, sampleSize.y)) * i;
}
