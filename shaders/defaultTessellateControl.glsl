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

uniform float tessFactor = 8.0;

uniform mat4 MV;

float getSphereExtent(vec3 o, float d) {
    vec4 clipPos = MV * vec4(o, 1.0);
    return abs(d / clipPos.w);
}

float tesselationFactor(vec3 a, vec3 b) {

   float d = distance(a, b);
   vec3 o = (a + b) / 2.0;
   return max(1, tessFactor * getSphereExtent(o, d));
}

void main() {
    float depth = length(IN[gl_InvocationID].viewVector);

    if(gl_InvocationID == 0) {
        vec3 a = gl_in[gl_InvocationID + 0].gl_Position.xyz;
        vec3 b = gl_in[gl_InvocationID + 1].gl_Position.xyz;
        vec3 c = gl_in[gl_InvocationID + 2].gl_Position.xyz;

        gl_TessLevelOuter[0] = tesselationFactor(a, b);
        gl_TessLevelOuter[1] = tesselationFactor(b, c);
        gl_TessLevelOuter[2] = tesselationFactor(c, a);

        gl_TessLevelInner[0] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[1] + gl_TessLevelOuter[2]) / 3.0;
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;
	OUT[gl_InvocationID].texcoord = IN[gl_InvocationID].texcoord;
	OUT[gl_InvocationID].drawID = IN[gl_InvocationID].drawID;
	OUT[gl_InvocationID].viewVector = IN[gl_InvocationID].viewVector;
}
