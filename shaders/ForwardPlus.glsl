struct TileLightData {
    uint lightCount;
    int visibleLightIndex[16];
};

struct Light {
  vec4 posRad;
  vec4 colorI;
};

uniform uint tileCountX;
uniform bool showLightDebug;