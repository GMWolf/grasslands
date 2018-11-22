
//
// Created by Felix Bridault on 22/11/2018.
//

#ifndef GRASSLANDS_NOISE_H
#define GRASSLANDS_NOISE_H

#include <cstdlib>

float* initNoise() {
    float * t = new float[512];
    for(int i = 0; i < 512; i++) {
        t[i] = (rand() / (float) RAND_MAX) * 2 - 1;
    }
    return t;
}

const float* perm = initNoise();

float noise(float x) {
    const int a = ((int)x) % 512;
    const int b = (a + 1) % 512;
    const float d = fmodf(x, 512) - a;
    const float af = perm[a];
    const float bf = perm[b];
    float ap = af * d;
    float bp = -bf * (1 - d);
    float u = d * d * (3.0 - 2.0 * d);
    return (ap*(1-u)) + (bp*u);
}


#endif //GRASSLANDS_NOISE_H
