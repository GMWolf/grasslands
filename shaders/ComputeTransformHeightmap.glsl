#version 430

struct Transform
 {
       vec3 pos;
       float scale;
       vec4 rot;
};

layout(std430, binding = 0) buffer transformBuffer
{
    Transform transforms[];
};


layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


uniform float texScale;
uniform float heightScale;
uniform sampler2DArray tex;
uniform uint layer;

void main() {
    transforms[gl_GlobalInvocationID.x].pos.y = (texture(tex, vec3(transforms[gl_GlobalInvocationID.x].pos.xz * texScale, layer)).x * 2 - 1.0) *  heightScale;
}
