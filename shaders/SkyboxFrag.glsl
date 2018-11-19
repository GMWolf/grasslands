#version 430

uniform samplerCubeArray cubemaps[8];

uniform ivec2 skybox;
uniform mat4 invMat;
uniform vec3 eyePos;

in vert {
    vec2 texCoord;
} IN;

out vec4 outColor;

void main() {

     vec2 tc = IN.texCoord * 2.0 - 1.0;
     vec4 clipSpace = vec4(tc, 1.0, 1.0);

     vec4 worldSpacePos = invMat * clipSpace;
     worldSpacePos /= worldSpacePos.w;

     vec3 eyeDir = worldSpacePos.xyz - eyePos;


     outColor = vec4(texture(cubemaps[skybox.x], vec4(eyeDir, skybox.y)));
}
