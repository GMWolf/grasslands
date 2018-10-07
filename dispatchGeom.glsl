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


layout(std430, binding = 1) writeonly buffer outputCommandsBuffer
{
    DrawCommand outputCommands[];
};


void main() {

    uint index = gl_GlobalInvocationID.x;

    outputCommands[index] = inputCommands[index];
}
