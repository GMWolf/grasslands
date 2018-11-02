#version 450

#define PI 3.1415926535897932384626433832795

struct material {
    uint albedo;
    uint normal;
    uint roughAlpha;
    uint translucency;
};


layout(std430, binding = 1) buffer MaterialIndexBuffer {
    uint materialIndex[];
};

layout(std430, binding = 2) buffer MaterialDataBuffer {
    material materials[];
};

uniform sampler2DArray tex[8];

in Vertex {
    vec3 normal;
    flat uint drawID;
    vec2 texcoord;
    vec3 viewVector;
} IN;

out vec4 outColor;


// http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );

    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construct a scale-invariant frame
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}


vec3 perturb_normal( vec3 vertex_normal, vec3 viewVector, vec2 texcoord, vec3 map )
{
    // assume vertex_normal, the interpolated vertex normal and
    // viewVector, the view vector (vertex to eye)
    mat3 TBN = cotangent_frame(vertex_normal, -viewVector, texcoord);
    return normalize(TBN * map);
}

//Distribution GGX
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


void main()
{

    material mat = materials[materialIndex[IN.drawID]];
    vec3 RA = texture(tex[1], vec3(IN.texcoord, mat.roughAlpha)).xyz;

    float roughness = RA.x;
    float Alpha = RA.y;

    if (Alpha < 0.45) {
        discard;
    }
   /* if (Alpha < 0.45) {
        discard;
    }
*/
    //outColor.a = Alpha;

    vec3 albedo = texture(tex[1], vec3(IN.texcoord, mat.albedo)).xyz;

    vec3 N = normalize(IN.normal);
    vec3 normalMap = normalize(texture(tex[1], vec3(IN.texcoord, mat.normal)).xyz * 2.0 - 1.0);
    normalMap *= vec3(1,-1,1);

    N = perturb_normal(N, IN.viewVector, IN.texcoord, normalMap);

    vec3 V = normalize(IN.viewVector);

    vec3 L = normalize(vec3(0, 1, 1));

    vec3 H = normalize(L + V);

    //fresnel
    vec3 F0 = vec3(0.04);
    vec3 F = fresnel(max(dot(H, V), 0.0), F0);


    float NDF = D_GGX(N, H, roughness); //normal distribution
    float G = G_smith(N, V, L, roughness); //geometry

    //cook-torance
    vec3 numerator = NDF * G * F;

    float denom = 4.0 * max(dot(N,V), 0.0)*max(dot(N,L), 0.0);
    vec3 specular = numerator / max(denom, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    float NdotL = max(dot(N, L), 0.0);

    vec3 radiance = vec3(4.0, 4.0, 4.0);
    vec3 light = (kD * albedo / PI + specular)  * radiance  * NdotL;

    vec3 ambient = vec3(0.15) * albedo;

    //translucency
    vec3 translucency = texture(tex[1], vec3(IN.texcoord, mat.translucency)).xyz;
    float NdotLI = min(max(dot(-N, L), 0), 1);
    /*float GI = G_smith(-N, V, L, roughness);
    vec3 transmit = (translucency * radiance) * GI;*/
    /*float VdotL = max(dot(normalize(IN.viewVector), -L), 0);
    vec3 transmit = VdotL * translucency;*/

    float EdotL = min(max(dot(normalize(IN.viewVector), -L), 0), 1);

    vec3 transmit = (EdotL * NdotLI + ambient) * translucency * radiance * albedo;

    //outColor = vec4(transmit, 1.0);

    outColor = vec4(light + transmit + ambient, Alpha);


    //outColor = vec4(albedo, 1.0);
}
