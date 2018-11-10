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

struct Transform
 {
       vec3 pos;
       float scale;
       vec4 rot;
};

layout(location = 0) uniform mat4 u_viewproj;
layout(location = 1) uniform bool u_doCull = true;

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

layout(binding = 3) uniform atomic_uint commandCount;

layout(std430, binding = 4) readonly buffer transformBuffer
{
    Transform transforms[];
};

//Adapted from glm
mat4 quatToMatrix(vec4 quat) {
        float qxx = quat.x * quat.x;
		float qyy = quat.y * quat.y;
		float qzz = quat.z * quat.z;
		float qxz = quat.x * quat.z;
		float qxy = quat.x * quat.y;
		float qyz = quat.y * quat.z;
		float qwx = quat.w * quat.x;
		float qwy = quat.w * quat.y;
		float qwz = quat.w * quat.z;

		mat4 Result = mat4(1.0);

		Result[0][0] = 1.0 - 2.0 * (qyy +  qzz);
		Result[0][1] = 2.0 * (qxy + qwz);
		Result[0][2] = 2.0 * (qxz - qwy);

		Result[1][0] = 2.0 * (qxy - qwz);
		Result[1][1] = 1.0 - 2.0 * (qxx +  qzz);
		Result[1][2] = 2.0 * (qyz + qwx);

		Result[2][0] = 2.0 * (qxz + qwy);
		Result[2][1] = 2.0 * (qyz - qwx);
		Result[2][2] = 1.0 - 2.0 * (qxx +  qyy);
		return Result;
}


mat4 transformToMatrix(Transform t) {
    mat4 result = quatToMatrix(t.rot);
    result[3] += vec4(t.pos, 0);

    result[0][0] *= t.scale;
    result[1][1] *= t.scale;
    result[2][2] *= t.scale;

    return result;
}


bool OOBBInFrustrum(vec3 min, vec3 max, Transform transform, mat4 viewproj) {

    mat4 toClipSpace = viewproj * transformToMatrix(transform);


    //Project all 8 bbox points
    vec4 bbox[8];
    bbox[0] = toClipSpace * vec4(min[0], max[1], min[2], 1.f);
    bbox[1] = toClipSpace * vec4(min[0], max[1], max[2], 1.f);
    bbox[2] = toClipSpace * vec4(max[0], max[1], max[2], 1.f);
    bbox[3] = toClipSpace * vec4(max[0], max[1], min[2], 1.f);
    bbox[4] = toClipSpace * vec4(max[0], min[1], min[2], 1.f);
    bbox[5] = toClipSpace * vec4(max[0], min[1], max[2], 1.f);
    bbox[6] = toClipSpace * vec4(min[0], min[1], max[2], 1.f);
    bbox[7] = toClipSpace * vec4(min[0], min[1], min[2], 1.f);

    bvec3 allGt = bvec3(true);
    bvec3 allLt = bvec3(true);
    for(int i = 0; i < 8; i++) {
        //Equivalent to / by w, then lessthan(1)
        bvec3 gt = greaterThan(bbox[i].xyz, vec3(bbox[i].w));
        bvec3 lt = lessThan(bbox[i].xyz , -vec3(bbox[i].w));
        allGt = bvec3(allGt.x && gt.x, allGt.y && gt.y, allGt.z && gt.z);
        allLt = bvec3(allLt.x && lt.x, allLt.y && lt.y, allLt.z && lt.z);
    }


    return !(any(allGt) || any(allLt));
}
void main() {

    Mesh m = meshData[inputCommands[gl_GlobalInvocationID.x].meshIndex];


    if(!u_doCull || OOBBInFrustrum(m.bboxMin, m.bboxmax, transforms[gl_GlobalInvocationID.x], u_viewproj)) {
        uint index = atomicCounterIncrement(commandCount);

       outputCommands[index].count = m.elementCount;
       outputCommands[index].instanceCount = 1;
       outputCommands[index].firstIndex = m.first;
       outputCommands[index].baseVertex = m.baseVertex;
       outputCommands[index].baseInstance = gl_GlobalInvocationID.x;
    }


}
