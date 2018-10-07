#version 430 core


layout(local_size_x = 1) in;

struct Mesh {
    vec3 bboxMin; float pad0;
    vec3 bboxmax; float pad1;

    uint first, elementCount, baseVertex;
    float pad2;
};

struct DrawCommand {
    uint count;
    uint instanceCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};

struct InputCommand {
    uint meshIndex;
};

layout(location = 0) uniform uint u_Offset;

layout(std430, binding = 0) readonly buffer meshDataBuffer {
    Mesh meshData[];
};

layout(std430, binding = 1) readonly buffer inputCommandsBuffer
{
    InputCommand inputCommands[];
};

layout(std430, binding = 2) writeonly buffer outputCommandsBuffer
{
    DrawCommand outputCommands[];
};


void main() {

    uint index = gl_GlobalInvocationID.x + u_Offset;

    Mesh m = meshData[inputCommands[index].meshIndex];

    outputCommands[index].count = m.elementCount;
    outputCommands[index].instanceCount = 1;
    outputCommands[index].firstIndex = m.first;
    outputCommands[index].baseVertex = m.baseVertex;
    outputCommands[index].baseInstance = index;
}
