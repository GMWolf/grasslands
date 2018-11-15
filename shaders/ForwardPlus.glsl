struct TileLightData {
    uint lightCount;
    int visibleLightIndex[128];
};

struct Light {
  vec4 posRad;
  vec4 colorI;
};

uniform uint tileCountX;
uniform bool showLightDebug;