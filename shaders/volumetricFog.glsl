#version 430



uniform sampler2D shadowMap;
uniform sampler2D depthMap;

in vert {
    vec2 texCoord;
} IN;

out vec4 outColor;

uniform mat4 invMat;
uniform vec3 eyePos;

uniform mat4 shadowVP;

#include "../shaders/shadow.glsl"

void main() {

    outColor = vec4(0,0,0,1.0);

    float depth = 20;//texture(depthMap, IN.texCoord);

    vec2 tc = IN.texCoord * 2.0 -1.0;

    vec4 dw = (invMat * vec4(tc, -1.0, 1.0));
    vec3 d = dw.xyz / dw.w;
    d -= eyePos;
    d = normalize(d);

    vec3 step = d * (depth / 256);

    vec3 pos = eyePos;
    for(int i = 0; i < 256; i++) {

        float s = shadowIntensity(pos);
        outColor += vec4(vec3((s*s*s)) * 0.001, 1.0);

        pos += step;
    }

   // outColor = vec4(d, 1.0);;

}
