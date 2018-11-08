uniform float u_minVariance = 0.0001;
//https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch08.html
float ChebyshecUpperBound(vec2 moments, float t) {

    float p = float(t <= moments.x);

    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, u_minVariance);
    float d = t - moments.x;
    float p_max = variance / (variance + d*d);
    return max(p, p_max);

}

float shadowIntensity(vec3 pos) {
    vec4 shadowCoord = (shadowVP * vec4(pos, 1.0));
    shadowCoord.xy /= shadowCoord.w;
    shadowCoord.xy = (shadowCoord.xy + 1) / 2.0f;
    vec4 shadowFrag =  texture(shadowMap, shadowCoord.xy);

    //return int(shadowFrag.x > shadowCoord.z);

    vec2 moments = shadowFrag.xy;
    return ChebyshecUpperBound(moments, shadowCoord.z);
}