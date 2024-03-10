#include <iostream>

#include "perlin.h"

using namespace std;

int main(int argc, char* args[])
{
    Perlin newPerlin = Perlin();

    float frequency = 8.0f;
    float persistence = 0.5f;
    float randomNoise = 0.0f;

    newPerlin.GeneratePerlinNoise(frequency, persistence, 100, 100, randomNoise, 0, 0);

    cout << "test";

    return 0;
}