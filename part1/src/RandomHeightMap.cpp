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

template<typename T>
class Vec2
{
public:
    Vec2() : x(T(0)), y(T(0)) {}
    Vec2(T xx, T yy) : x(xx), y(yy) {}
    Vec2 operator * (const T &r) const { return Vec2(x * r, y * r); }
    Vec2& operator *= (const T &r) { x *= r, y *= r; return *this; }
    T x, y;
};

template<typename T>
class Vec3
{
public:
    Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
    Vec3 operator * (const T &r) const { return Vec3(x * r, y * r, z * r); }
    Vec3 operator - (const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3& operator *= (const T &r) { x *= r, y *= r, z *= r; return *this; }
    T length2() const { return x * x + y * y + z * z; }
    Vec3& operator /= (const T &r) { x /= r, y /= r, z /= r; return *this; }
    Vec3 cross(const Vec3 &v) const
    {
        return Vec3(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
        );
    }
    Vec3& normalize()
    {
        T len2 = length2();
        if (len2 > 0) {
            T invLen = T(1) / sqrt(len2);
            x *= invLen, y *= invLen, z *= invLen;
        }
        return *this;
    }
    friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v)
    {
        os << v.x << ", " << v.y << ", " << v.z;
        return os;
    }
    T x, y, z;
};

typedef Vec2<float> Vec2f;
typedef Vec3<float> Vec3f;

template<typename T = float>
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

// Noise params
// the number of levels of detail you want you perlin noise to have.
//int numLayers = 5;
//float meshHeight = 32;  // Vertical scaling
//float noiseScale = 64;  // Horizontal scaling
////  number that determines how much each octave contributes to the overall shape (adjusts amplitude)
//float persistence = 0.5;
////number that determines how much detail is added or removed at each octave (adjusts frequency).
//float lacunarity = 2;
int imageHeight = 1024;
int imageWidth = 1024;

// Constructor
RandomHeightMap::RandomHeightMap(){

}

RandomHeightMap::~RandomHeightMap(){

}

void RandomHeightMap::generateRandomNoiseMap(int octaves) {
    PerlinNoise noise;
//    Vec3f derivs;
//    const uint32_t width = 512, height = 512;
    float *noiseMap = new float[imageWidth * imageHeight];
//
//    for (uint32_t j = 0; j < imageHeight; ++j) {
//        for (uint32_t i = 0; i < imageWidth; ++i) {
//            noiseMap[j * imageWidth + i] = (noise.eval(Vec3f(i, 0, j) * (1 / 64.), derivs) + 1) * 0.5;
//        }
//    }
    // create an array that is filled with random numbers between 0 and 1
//    srand ( time(NULL) );
    float* fNoiseSeed2D = new float[imageWidth * imageHeight];
    std::cout <<"oc" << octaves << std::endl;
    for (int i = 0; i < imageWidth * imageHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
    noise.PerlinNoise2D(imageWidth, imageHeight, fNoiseSeed2D, octaves, 2.0f, noiseMap);
    generateRandomPPM(noiseMap);
}

void RandomHeightMap::generateRandomPPM(float *noiseMap) {
    // output noise map to PPM
    std::ofstream ofs;
    ofs.open("./noise2.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
    for (unsigned k = 0; k < imageWidth * imageHeight; ++k) {
//        unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
        int n = (int)(noiseMap[k] * 255);
        ofs << n << "\n";
        ofs << n << "\n";
        ofs << n << "\n";
    }
    ofs.close();

    delete[] noiseMap;
}