#include "RandomHeightMap.hpp"
#include "perlin.h"
#include <string>
#include <fstream>
#include <iostream>


#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <random>
#include <functional>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

class PerlinNoise
{
public:
    void PerlinNoise2D(int nWidth, int nHeight, float *fSeed, int nOctaves, float fBias, float *fOutput)
    {
        // Used 1D Perlin Noise
        for (int x = 0; x < nWidth; x++)
            for (int y = 0; y < nHeight; y++)
            {
                float fNoise = 0.0f;
                float fScaleAcc = 0.0f;
                float fScale = 1.0f;

                for (int o = 0; o < nOctaves; o++)
                {
                    int nPitch = nWidth >> o;
                    int nSampleX1 = (x / nPitch) * nPitch;
                    int nSampleY1 = (y / nPitch) * nPitch;

                    int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
                    int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

                    float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
                    float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

                    float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
                    float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

                    fScaleAcc += fScale;
                    fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
                    fScale = fScale / fBias;
                }

                // Scale to seed range
                fOutput[y * nWidth + x] = fNoise / fScaleAcc;

            }
    }
};

int imageHeight = 1024;
int imageWidth = 1024;

// Constructor
RandomHeightMap::RandomHeightMap(){

}

RandomHeightMap::~RandomHeightMap(){

}

void RandomHeightMap::generateRandomNoiseMap(int octaves) {
    PerlinNoise noise;
    float *noiseMap = new float[imageWidth * imageHeight];
    float* fNoiseSeed2D = new float[imageWidth * imageHeight];
    if (octaves > 9) {
        octaves = 9;
    }
    if (octaves < 3) {
        octaves = 3;
    }
    srand(1);
    for (int i = 0; i < imageWidth * imageHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
    noise.PerlinNoise2D(imageWidth, imageHeight, fNoiseSeed2D, octaves, 2.0f, noiseMap);
    generateRandomPPM(noiseMap);
}

void RandomHeightMap::generateRandomPPM(float *noiseMap) {
    // output noise map to PPM
    std::ofstream ofs;
    ofs.open("./noise2.ppm", std::ios::out | std::ios::binary);
    ofs << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";
    for (unsigned k = 0; k < imageWidth * imageHeight; ++k) {
        int n = (int)(noiseMap[k] * 255);
        ofs << n << "\n";
        ofs << n << "\n";
        ofs << n << "\n";
    }
    ofs.close();

    delete[] noiseMap;
}