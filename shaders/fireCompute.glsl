#version 430

float ro = 0;

uniform vec3 spawnPoint;
uniform float spawnRadius;
uniform float life;
uniform float dt;
uniform float time;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float randAuto() {
    float r =  rand(vec2(gl_GlobalInvocationID.x, ro + time));
    ro += 0.1;
    return r * 2 - 1;
}



struct particleVert {
    vec3 pos; float size; vec3 colour; float rot;
};

layout(std430, binding = 0) buffer vertexData {
    particleVert partVertData[];
};

struct particleU {
    vec3 vel; float age;
};

layout(std430, binding = 1) buffer vertexUData {
    particleU partUData[];
};



layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {

    if (partUData[gl_GlobalInvocationID.x].age >= life) {
        //repsawn particle
        partUData[gl_GlobalInvocationID.x].age = 0;//(1 + randAuto()) * 0.25;
        partUData[gl_GlobalInvocationID.x].vel = vec3(randAuto(), 0, randAuto()) * 1.5 ;
        vec2 p = vec2(randAuto(), randAuto()) * spawnRadius;
         partVertData[gl_GlobalInvocationID.x].pos = spawnPoint + (vec3(p.x, 0, p.y));
         partVertData[gl_GlobalInvocationID.x].size = 400.0;
    }


    partVertData[gl_GlobalInvocationID.x].pos += partUData[gl_GlobalInvocationID.x].vel * dt;
    partUData[gl_GlobalInvocationID.x].age += dt;
    //Gravitate to center
    vec3 g = (spawnPoint - partVertData[gl_GlobalInvocationID.x].pos) * 3.5;
    g.y = 3;
    partUData[gl_GlobalInvocationID.x].vel += g * dt;


    float l = (partUData[gl_GlobalInvocationID.x].age / life);
    partVertData[gl_GlobalInvocationID.x].size = 800 *  smoothstep(1.0, 0.0, l*l*l*l*l);
    partVertData[gl_GlobalInvocationID.x].colour = vec3(smoothstep(1.0, 0.0, l*l*l*l), vec2(smoothstep(1.0, 0.0, l*l)));


}
