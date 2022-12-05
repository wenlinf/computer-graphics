//
// Created by 方文琳 on 2022/12/3.
//

#ifndef RANDOMHEIGHTMAP_H
#define RANDOMHEIGHTMAP_H

#include <string>
#include <vector>


class RandomHeightMap {
public:
    // RandomHeightMap Constructor
    RandomHeightMap();
    // RandomHeightMap destructor
    ~RandomHeightMap();
    void generateRandomNoiseMap();
    void generateRandomPPM(float *noiseMap);
    void generateBiome(float *noiseMap);

private:
    std::vector<float> normalizedNoiseValues;
};


#endif
