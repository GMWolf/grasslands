#version 430



in Vertex {
 vec3 normal;
 flat uint drawID;
 vec2 texcoord;
 vec3 viewVector;
 noperspective vec3 worldPos;
} IN;


uniform sampler2DArray tex[8];
uniform sampler2D shadowMap;

uniform mat4 shadowVP;

uniform vec3 lightDir;
uniform vec3 lightColour;

out vec4 outColor;

#include "../shaders/normals.glsl"
#include "../shaders/PBR.glsl"
#include "../shaders/shadow.glsl"

void main() {



}
