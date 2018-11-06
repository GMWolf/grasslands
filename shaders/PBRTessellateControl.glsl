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
        float texSize;
} OUT[];

uniform float tessFactor = 32.0;

uniform mat4 MV;
uniform mat4 projection;

float getSphereExtent(vec3 o, float d) {
    vec4 clipPos = MV * vec4(o, 1.0);
    return abs(d /* projection[1][1]*/ / clipPos.w);
}

float clipDistance(vec3 a, vec3 b) {
   float d = distance(a, b);
   vec3 o = (a + b) / 2.0;
   return getSphereExtent(o, d);
   /*vec4 ac = (MV * vec4(a, 1.0));
   vec4 bc = (MV * vec4(b, 1.0));
   return distance(ac.xyz / ac.w, bc.xyz / bc.w);*/
}

float tesselationFactor(float cd) {
    return clamp(tessFactor  * cd, 1, 2048);
}

float maxComp(vec2 v) {
    return max(v.x, v.y);
}

void main() {
    float depth = length(IN[gl_InvocationID].viewVector);

    vec3 a = gl_in[1].gl_Position.xyz;
    vec3 b = gl_in[2].gl_Position.xyz;
    vec3 c = gl_in[0].gl_Position.xyz;

    float ab = clipDistance(a, b);
    float bc = clipDistance(b, c);
    float ca = clipDistance(c, a);

    gl_TessLevelOuter[0] = tesselationFactor(ab);
    gl_TessLevelOuter[1] = tesselationFactor(bc);
    gl_TessLevelOuter[2] = tesselationFactor(ca);

    vec2 uvA = IN[0].texcoord;
    vec2 uvB = IN[1].texcoord;
    vec2 uvC = IN[2].texcoord;

    float abTexSize = gl_TessLevelOuter[0] / maxComp(abs(uvA - uvB));
    float bcTexSize = gl_TessLevelOuter[1] / maxComp(abs(uvB - uvC));
    float caTexSize = gl_TessLevelOuter[2] / maxComp(abs(uvC - uvA));

    gl_TessLevelInner[0] = max(gl_TessLevelOuter[0], max( gl_TessLevelOuter[1], gl_TessLevelOuter[2]));

    if (gl_InvocationID == 0) OUT[gl_InvocationID].texSize = (abTexSize + caTexSize) / 2;
    if (gl_InvocationID == 1) OUT[gl_InvocationID].texSize = (abTexSize + bcTexSize) / 2;
    if (gl_InvocationID == 2) OUT[gl_InvocationID].texSize = (bcTexSize + caTexSize) / 2;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;
	OUT[gl_InvocationID].texcoord = IN[gl_InvocationID].texcoord;
	OUT[gl_InvocationID].drawID = IN[gl_InvocationID].drawID;
	OUT[gl_InvocationID].viewVector = IN[gl_InvocationID].viewVector;
}
