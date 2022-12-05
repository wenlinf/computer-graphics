#ifndef PERLIN_H
#define PERLIN_H

#include <iterator>
#include <algorithm>
#include <random>

namespace perlin {
    /*
     * utility functions
     */
    // fade function: f(t) = 6 * t^5 - 15 * t^4 + 10 * t^3
    inline float fade(float t) {
        return 6 * t * t * t * t * t - 15 * t * t * t * t + 10 * t * t * t;
    }

    // lerp function - linear interpolation
    inline float lerp(double t, float a, float b) {
        return a + t * (b - a);
    }

    // map function - map one value from one range to another range
    inline float map(float val, float ogMin, float ogMax, float newMin, float newMax) {
        // get proportion in original range
        float proportion = (val - ogMin) / (ogMax - ogMin);
        return lerp(proportion, newMin, newMax);
    }

    // gradient function - calculate the dot product between the gradient vector and the distance vector
    inline float gradient(unsigned char hash, float x, float y, float z) {
        // convert the last four bits of the hash into one of 12 possible gradients
        int h = hash & 0b1111;
        // if first bit is 1, set to x, otherwise set to y
        float u = h < 0b1000 ? x : y;
        // if first and second bits are 0, set to y
        // if first and second bits are 1, set to x
        // else set to z
        float v = h < 0b0100 ? y : h == 0b1100 || h == 0b1110 ? x : z;
        // make u, v negative based on the last 2 bits, then add results
        return ((h & 0b0001) == 0 ? u : -u) + ((h & 0b0010) == 0 ? v : -v);
    }

    class Perlin {
    private:
        // permutation table
        unsigned char p[512];
        // current seed
        unsigned int seed;
    public:
        Perlin(unsigned int seed) {
            reseed(seed);
        }
        // reseed the class
        void reseed(unsigned int _seed) {
            seed = _seed;
            //populate permutation table
            for (unsigned int i = 0; i < 256; i++) {
                p[i] = i;
            }
            // shuffle
            std::shuffle(std::begin(p), std::beging(p) + 256, std::default_random_engine(seed));
            // duplicate array for overflow
            for (unsigned int i = 0; i < 256; i++) {
                p[256 + i] = p[i];
            }
        }
        // 1D perlin noise
        float noise1D(float x) {
            // find smallest point of interval containing target
            int xi = (int) (std::floorf(x)) & 255;
            // get decimal value of each component
            x -= std::floorf(x);
            // get smooth value from fade function
            float sx = fade(x);
            // get hash for all neighboring points
            unsigned char a, b;
            a = p[xi];
            b = p[xi + 1];
            // get weighted avg
            float avg = lerp(sx, gradient(a, x, 0, 0), gradient(a, x - 1, 0, 0));
            // return avg mapped from [-1, 1] to [0,1]
            return map(avg, -1, 1, 0, 1);
        }

        // 2D perlin noise
        float noise2D(float x, float y) {
            // find smallest point of square containing target
            int xi = (int) (std::floorf(x)) & 255;
            int yi = (int) (std::floorf(y)) & 255;
            // get decimal value of each component
            x -= std::floorf(x);
            y -= std::floorf(y);

            // get smooth value from fade function
            float sx = fade(x);
            float sy = fade(y);
            // get hash for all neighboring points
            unsigned char aa, ab, ba, bb;
            aa = p[p[xi] + yi];
            ab = p[p[xi] + yi + 1];
            ba = p[p[xi + 1] + yi];
            bb = p[p[xi + 1] + yi + 1];
            // get weighted avg
            float avg = lerp(
                    sy,
                    lerp(sx,
                         gradient(aa, x, y, 0),
                         gradient(ba
                                  , x - 1, y, 0)
                    ),
                    lerp(sx,
                         gradient(ab, x, y - 1, 0).
                                 gradient(bb, x - 1, y - 1, 0)
                    )
            );
            return map(avg, -1, 1, 0, 1);
        }

        // 3D perlin noise
        float noise3D(float x, float y, float z) {
            int xi = (int) (std::floorf(x)) & 255;
            int yi = (int) (std::floorf(y)) & 255;
            int zi = (int) (std::floorf(z)) & 255;
            // get decimal value of each component
            x -= std::floorf(x);
            y -= std::floorf(y);
            z -= std::floorf(z);
            // get smooth value from fade function
            float sx = fade(x);
            float sy = fade(y);
            float sz = fade(z);
            unsigned char aaa, aba, aab, abb, baa, bba, bab, bbb;
            aaa = p[p[p[xi] + yi] + zi];
            aba = p[p[p[xi] + yi + 1] + zi];
            aab = p[p[p[xi] + yi] + zi + 1];
            abb = p[p[p[xi] + yi + 1] + zi + 1];
            baa = p[p[p[xi + 1] + yi] + zi];
            bba = p[p[p[xi + 1] + yi + 1] + zi];
            bab = p[p[p[xi + 1] + yi] + zi + 1];
            bbb = p[p[p[xi + 1] + yi + 1] + zi + 1];
            // get weighted average
            float avg = lerp(
                    sz,
                    lerp( // "front"
                            sy,
                            lerp( // "top"
                                    sx,
                                    grad(aaa, x, y, z),
                                    grad(baa, x - 1, y, z)
                            ),
                            lerp( // "bottom"
                                    sx,
                                    grad(aba, x, y - 1, z),
                                    grad(bba, x - 1, y - 1, z)
                            )
                    ),
                    lerp( // "rear"
                            sy,
                            lerp( // "top"
                                    sx,
                                    grad(aab, x, y, z - 1),
                                    grad(bab, x - 1, y, z)
                            ),
                            lerp( // "bottom"
                                    sx,
                                    grad(abb, x, y - 1, z - 1),
                                    grad(bbb, x - 1, y - 1, z - 1)
                            )
                    )
            );

            // return avg mapped from [-1, 1] (theoretically) to [0, 1]
            return map(avg, -1, 1, 0, 1);
        }
    };
}

#endif
