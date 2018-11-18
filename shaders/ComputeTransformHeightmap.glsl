#version 430


layout(std430, binding = 0) buffer transformBuffer
{
    vec3 positions[];
};


layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


uniform float texScale;
uniform float heightScale;
uniform sampler2D tex;

void main() {
    positions[gl_GlobalInvocationID.x].y = tex(positions[gl_GlobalInvocationID.x].xz * texScale) * heightScale;
}
