#version 450
out vec4 outColor;

noperspective in float depth;


void main() {
    float z = depth;
    float z2 = z * z;
    float z3 = z2 * z;
    float z4 = z3 * z;
    outColor.x = z;
    outColor.y = z2;
    outColor.z = z3;
    outColor.w = z4;
}
