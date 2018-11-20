#version 430

//based on https://github.com/bcrusco/Forward-Plus-Renderer/blob/master/Forward-Plus/Forward-Plus/source/shaders/light_culling.comp.glsl

uniform sampler2D depthMapA;
uniform sampler2D depthMapB;
uniform mat4 projection;
uniform mat4 viewProj;
uniform mat4 view;
uniform vec2 size;

shared uint minDepthi;
shared uint maxDepthi;
shared vec4 frustumPlanes[6];

struct Light {
  vec4 posRad;
  vec4 colorI;
};

layout(std430, binding = 0) readonly buffer LightDataBuffer {
  uint lightCount, pad0, pad1, pad2;
  Light lights[];
};

struct TileLightData {
    uint lightCount;
    int visibleLightIndex[16];
};

layout(std430, binding = 1) writeonly buffer VisibleLightBuffer {
    TileLightData tileData[];
};

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main() {

    ivec2 tilePos = ivec2(gl_WorkGroupID.xy);
    vec2 texPos = vec2(gl_GlobalInvocationID.xy) / size;
    ivec2 tileNumber = ivec2(gl_NumWorkGroups.xy);
    uint tileIndex = tilePos.y * tileNumber.x + tilePos.x;

    //Compute minmax depth
    if (gl_LocalInvocationIndex == 0) {
        minDepthi = 0xFFFFFFFF;
        maxDepthi = 0;
    }

    barrier();

    float depthA = texture(depthMapA, texPos).r;
    depthA = (0.5 * projection[3][2]) / (depthA + 0.5 * projection[2][2] - 0.5);
    float depthB = texture(depthMapB, texPos).r;
    depthB = (0.5 * projection[3][2]) / (depthB + 0.5 * projection[2][2] - 0.5);

    uint depthAi = floatBitsToUint(depthA);
    uint depthBi = floatBitsToUint(min(depthA, depthB));

    atomicMin(minDepthi, depthBi);
    atomicMax(maxDepthi, depthAi);

    barrier();

    //frustum planes
    if (gl_LocalInvocationIndex == 0) {
        float minDepth = uintBitsToFloat(minDepthi);
        float maxDepth = uintBitsToFloat(maxDepthi);

        vec2 nStep = (2.0 * vec2(tilePos)) / vec2(tileNumber);
        vec2 pStep = (2.0 * vec2(tilePos + ivec2(1,1))) / vec2(tileNumber);

        frustumPlanes[0] = vec4(1.0, 0.0, 0.0, 1.0 - nStep.x);
        frustumPlanes[1] = vec4(-1.0, 0.0, 0.0, -1.0 + pStep.x);
        frustumPlanes[2] = vec4(0.0, 1.0, 0.0, 1.0 - nStep.y);
        frustumPlanes[3] = vec4(0.0, -1.0, 0.0, -1.0 + pStep.y);
        frustumPlanes[4] = vec4(0.0, 0.0, -1.0, -minDepth);
        frustumPlanes[5] = vec4(0.0, 0.0, 1.0, maxDepth);

        for(uint i = 0; i < 4; i++) {
            frustumPlanes[i] *= viewProj;
            frustumPlanes[i] /= length(frustumPlanes[i].xyz);
        }

        frustumPlanes[4] *= view;
        frustumPlanes[4] /= length(frustumPlanes[4].xyz);
        frustumPlanes[5] *= view;
        frustumPlanes[5] /= length(frustumPlanes[5].xyz);

        tileData[tileIndex].lightCount = 0;
    }

    barrier();

    //Each invocation handles n lights. where n depends on passCount.
    uint threadCount = gl_WorkGroupSize.x * gl_WorkGroupSize.y;
    uint passCount = (lightCount + threadCount - 1) / threadCount;

    for(uint i = 0; i < passCount; i++) {
        uint lightIndex = i * threadCount + gl_LocalInvocationIndex;
        if (lightIndex >= lightCount) break;

        vec4 pos = vec4(lights[lightIndex].posRad.xyz, 1.0);
        float r = lights[lightIndex].posRad.w;
        bool inside = true;
        for(uint j = 0; j < 6; j++) {
            float dist = dot(pos, frustumPlanes[j]) + r;
            inside = inside && (dist > 0.0f);
        }

        if (inside) {
            uint offset = atomicAdd(tileData[tileIndex].lightCount, 1);
            tileData[tileIndex].visibleLightIndex[offset] = int(lightIndex);
            if (offset >= tileData[tileIndex].visibleLightIndex.length()) {
                break;
            }
        }

    }


}
