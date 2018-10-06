#version 430 core

layout(std430, binding = 1) buffer textureData {
    uint textureIndex[];
};

uniform sampler2DArray tex;
in Vertex {
    vec3 normal;
    flat uint drawID;
    vec2 texcoord;
} IN;

out vec4 outColor;
void main()
{
    float light = dot(IN.normal, -normalize(vec3(0, 1, 1))) * 0.8 + 0.2;
    outColor = texture(tex, vec3(IN.texcoord, textureIndex[IN.drawID])) * light;
}