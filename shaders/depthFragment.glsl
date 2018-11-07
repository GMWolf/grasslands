#version 450
out vec4 moments;

noperspective in float depth;

void main() {

    moments.x = depth;

    float dx = dFdx(depth);
    float dy = dFdy(depth);

    moments.y = depth*depth + 0.25*(dx*dx + dy*dy);

   /* outColor.x = z;
    outColor.y = z2;
    outColor.z = z3;
    outColor.w = z4;*/
}
