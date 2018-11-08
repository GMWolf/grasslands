float D_GGX(vec3 N, vec3 H, float a) {
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float nom = a2;
    float denom = (NdotH2*(a2- 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float G_GGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float G_smith(vec3 N, vec3 V, vec3 L, float K) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = G_GGX(NdotV, K);
    float ggx2 = G_GGX(NdotL, K);
    return ggx1 * ggx2;
}

vec3 fresnel(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0-cosTheta, 5.0);
}