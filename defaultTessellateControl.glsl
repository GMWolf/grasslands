#version 430 core

layout(vertices = 3) out;



in Vertex{
     vec3 normal;
     flat uint drawID;
     vec2 texcoord;
     vec3 viewVector;
} IN[];

out VertexIn {
        vec3 normal;
        flat uint drawID;
        vec2 texcoord;
        vec3 viewVector;
} OUT[];

/*
uniform int minTessLevel;
uniform int maxTessLevel;

uniform float maxLodDistance;
uniform float minLodDistance;
*/
void main() {

    float depth = length(IN[gl_InvocationID].viewVector);


    gl_TessLevelInner[0] = 5;

	gl_TessLevelOuter[0] = 5;
	gl_TessLevelOuter[1] = 5;
	gl_TessLevelOuter[2] = 5;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;
	OUT[gl_InvocationID].texcoord = IN[gl_InvocationID].texcoord;
	OUT[gl_InvocationID].drawID = IN[gl_InvocationID].drawID;
	OUT[gl_InvocationID].viewVector = IN[gl_InvocationID].viewVector;
}
