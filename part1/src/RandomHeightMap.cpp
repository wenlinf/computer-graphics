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
inline T dot(const Vec3<T> &a, const Vec3<T> &b)
{ return a.x * b.x + a.y * b.y + a.z * b.z; }

template<typename T = float>
inline T lerp(const T &lo, const T &hi, const T &t)
{
    return lo * (1 - t) + hi * t;
}

inline
float smoothstep(const float &t)
{
    return t * t * (3 - 2 * t);
}

inline
float quintic(const float &t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

inline
float smoothstepDeriv(const float &t)
{
    return t * (6 - 6 * t);
}

inline
float quinticDeriv(const float &t)
{
    return 30 * t * t * (t * (t - 2) + 1);
}

class PerlinNoise
{
public:
    PerlinNoise(const unsigned &seed = 2016)
    {
        std::mt19937 generator(seed);
        std::uniform_real_distribution<float> distribution;
        auto dice = std::bind(distribution, generator);
        for (unsigned i = 0; i < tableSize; ++i) {
#if 0
            // bad
            float gradientLen2;
            do {
                gradients[i] = Vec3f(2 * dice() - 1, 2 * dice() - 1, 2 * dice() - 1);
                gradientLen2 = gradients[i].length2();
            } while (gradientLen2 > 1);
            gradients[i].normalize();
#else
            // better
            float theta = acos(2 * dice() - 1);
            float phi = 2 * dice() * M_PI;

            float x = cos(phi) * sin(theta);
            float y = sin(phi) * sin(theta);
            float z = cos(theta);
            gradients[i] = Vec3f(x, y, z);
#endif
            permutationTable[i] = i;
        }

        std::uniform_int_distribution<unsigned> distributionInt;
        auto diceInt = std::bind(distributionInt, generator);
        // create permutation table
        for (unsigned i = 0; i < tableSize; ++i)
            std::swap(permutationTable[i], permutationTable[diceInt() & tableSizeMask]);
        // extend the permutation table in the index range [256:512]
        for (unsigned i = 0; i < tableSize; ++i) {
            permutationTable[tableSize + i] = permutationTable[i];
        }
    }
    virtual ~PerlinNoise() {}

    //[comment]
    // Improved Noise implementation (2002)
    // This version compute the derivative of the noise function as well
    //[/comment]
    float eval(const Vec3f &p, Vec3f& derivs) const
    {
        int xi0 = ((int)std::floor(p.x)) & tableSizeMask;
        int yi0 = ((int)std::floor(p.y)) & tableSizeMask;
        int zi0 = ((int)std::floor(p.z)) & tableSizeMask;

        int xi1 = (xi0 + 1) & tableSizeMask;
        int yi1 = (yi0 + 1) & tableSizeMask;
        int zi1 = (zi0 + 1) & tableSizeMask;

        float tx = p.x - ((int)std::floor(p.x));
        float ty = p.y - ((int)std::floor(p.y));
        float tz = p.z - ((int)std::floor(p.z));

        float u = quintic(tx);
        float v = quintic(ty);
        float w = quintic(tz);

        // generate vectors going from the grid points to p
        float x0 = tx, x1 = tx - 1;
        float y0 = ty, y1 = ty - 1;
        float z0 = tz, z1 = tz - 1;

        float a = gradientDotV(hash(xi0, yi0, zi0), x0, y0, z0);
        float b = gradientDotV(hash(xi1, yi0, zi0), x1, y0, z0);
        float c = gradientDotV(hash(xi0, yi1, zi0), x0, y1, z0);
        float d = gradientDotV(hash(xi1, yi1, zi0), x1, y1, z0);
        float e = gradientDotV(hash(xi0, yi0, zi1), x0, y0, z1);
        float f = gradientDotV(hash(xi1, yi0, zi1), x1, y0, z1);
        float g = gradientDotV(hash(xi0, yi1, zi1), x0, y1, z1);
        float h = gradientDotV(hash(xi1, yi1, zi1), x1, y1, z1);

        float du = quinticDeriv(tx);
        float dv = quinticDeriv(ty);
        float dw = quinticDeriv(tz);

        float k0 = a;
        float k1 = (b - a);
        float k2 = (c - a);
        float k3 = (e - a);
        float k4 = (a + d - b - c);
        float k5 = (a + f - b - e);
        float k6 = (a + g - c - e);
        float k7 = (b + c + e + h - a - d - f - g);

        derivs.x = du *(k1 + k4 * v + k5 * w + k7 * v * w);
        derivs.y = dv *(k2 + k4 * u + k6 * w + k7 * v * w);
        derivs.z = dw *(k3 + k5 * u + k6 * v + k7 * v * w);

        return k0 + k1 * u + k2 * v + k3 * w + k4 * u * v + k5 * u * w + k6 * v * w + k7 * u * v * w;
    }

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

    //[comment]
    // classic/original Perlin noise implementation (1985)
    //[/comment]
    float eval(const Vec3f &p) const
    {
        int xi0 = ((int)std::floor(p.x)) & tableSizeMask;
        int yi0 = ((int)std::floor(p.y)) & tableSizeMask;
        int zi0 = ((int)std::floor(p.z)) & tableSizeMask;

        int xi1 = (xi0 + 1) & tableSizeMask;
        int yi1 = (yi0 + 1) & tableSizeMask;
        int zi1 = (zi0 + 1) & tableSizeMask;

        float tx = p.x - ((int)std::floor(p.x));
        float ty = p.y - ((int)std::floor(p.y));
        float tz = p.z - ((int)std::floor(p.z));

        float u = smoothstep(tx);
        float v = smoothstep(ty);
        float w = smoothstep(tz);

        // gradients at the corner of the cell
        const Vec3f &c000 = gradients[hash(xi0, yi0, zi0)];
        const Vec3f &c100 = gradients[hash(xi1, yi0, zi0)];
        const Vec3f &c010 = gradients[hash(xi0, yi1, zi0)];
        const Vec3f &c110 = gradients[hash(xi1, yi1, zi0)];

        const Vec3f &c001 = gradients[hash(xi0, yi0, zi1)];
        const Vec3f &c101 = gradients[hash(xi1, yi0, zi1)];
        const Vec3f &c011 = gradients[hash(xi0, yi1, zi1)];
        const Vec3f &c111 = gradients[hash(xi1, yi1, zi1)];

        // generate vectors going from the grid points to p
        float x0 = tx, x1 = tx - 1;
        float y0 = ty, y1 = ty - 1;
        float z0 = tz, z1 = tz - 1;

        Vec3f p000 = Vec3f(x0, y0, z0);
        Vec3f p100 = Vec3f(x1, y0, z0);
        Vec3f p010 = Vec3f(x0, y1, z0);
        Vec3f p110 = Vec3f(x1, y1, z0);

        Vec3f p001 = Vec3f(x0, y0, z1);
        Vec3f p101 = Vec3f(x1, y0, z1);
        Vec3f p011 = Vec3f(x0, y1, z1);
        Vec3f p111 = Vec3f(x1, y1, z1);

        // linear interpolation
        float a = lerp(dot(c000, p000), dot(c100, p100), u);
        float b = lerp(dot(c010, p010), dot(c110, p110), u);
        float c = lerp(dot(c001, p001), dot(c101, p101), u);
        float d = lerp(dot(c011, p011), dot(c111, p111), u);

        float e = lerp(a, b, v);
        float f = lerp(c, d, v);

        return lerp(e, f, w); // g
    }

private:
    /* inline */
    uint8_t hash(const int &x, const int &y, const int &z) const
    {
        return permutationTable[permutationTable[permutationTable[x] + y] + z];
    }

    //[comment]
    // Compute dot product between vector from cell corners to P with predefined gradient directions
    //
    //    perm: a value between 0 and 255
    //
    //    float x, float y, float z: coordinates of vector from cell corner to shaded point
    //[/comment]
    float gradientDotV(
            uint8_t perm, // a value between 0 and 255
            float x, float y, float z) const
    {
        switch (perm & 15) {
            case  0: return  x + y; // (1,1,0)
            case  1: return -x + y; // (-1,1,0)
            case  2: return  x - y; // (1,-1,0)
            case  3: return -x - y; // (-1,-1,0)
            case  4: return  x + z; // (1,0,1)
            case  5: return -x + z; // (-1,0,1)
            case  6: return  x - z; // (1,0,-1)
            case  7: return -x - z; // (-1,0,-1)
            case  8: return  y + z; // (0,1,1),
            case  9: return -y + z; // (0,-1,1),
            case 10: return  y - z; // (0,1,-1),
            case 11: return -y - z; // (0,-1,-1)
            case 12: return  y + x; // (1,1,0)
            case 13: return -x + y; // (-1,1,0)
            case 14: return -y + z; // (0,-1,1)
            case 15: return -y - z; // (0,-1,-1)
        }
    }

    static const unsigned tableSize = 256;
    static const unsigned tableSizeMask = tableSize - 1;
    Vec3f gradients[tableSize];
    unsigned permutationTable[tableSize * 2];
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
int imageHeight = 512;
int imageWidth = 512;

// Constructor
RandomHeightMap::RandomHeightMap(){

}

RandomHeightMap::~RandomHeightMap(){

}

void RandomHeightMap::generateRandomNoiseMap() {
    PerlinNoise noise;
//    Vec3f derivs;
    const uint32_t width = 512, height = 512;
    float *noiseMap = new float[width * height];
//
//    for (uint32_t j = 0; j < imageHeight; ++j) {
//        for (uint32_t i = 0; i < imageWidth; ++i) {
//            noiseMap[j * imageWidth + i] = (noise.eval(Vec3f(i, 0, j) * (1 / 64.), derivs) + 1) * 0.5;
//        }
//    }
    // create an array that is filled with random numbers between 0 and 1
    srand ( time(NULL) );
    float* fNoiseSeed2D = new float[imageWidth * imageHeight];
    for (int i = 0; i < imageWidth * imageHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
    noise.PerlinNoise2D(imageWidth, imageHeight, fNoiseSeed2D, 7, 2.0f, noiseMap);
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