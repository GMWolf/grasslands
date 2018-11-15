#version 450
out vec2 moments;

noperspective in float depth;

void main() {

    moments.x = depth;

    float dx = dFdx(depth);
    float dy = dFdy(depth);

    moments.y = depth*depth + 0.25*(dx*dx + dy*dy);
}
