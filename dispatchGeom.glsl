#version 430 core


layout(local_size_x = 1) in;

struct DrawCommand {
    uint count;
    uint instanceCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};

layout(std430, binding = 0) readonly buffer inputCommandsBuffer
{
    DrawCommand inputCommands[];
};

layout(location = 0) uniform uint u_Offset;


layout(std430, binding = 1) writeonly buffer outputCommandsBuffer
{
    DrawCommand outputCommands[];
};


void main() {

    uint index = gl_GlobalInvocationID.x + u_Offset;

    outputCommands[index] = inputCommands[index];
}
