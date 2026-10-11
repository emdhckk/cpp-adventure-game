#include "NoiseGen.h"

// Constructeur avec Seed
NoiseGen::NoiseGen(int seed) {
    // Configuration du bruit pour la forme
    shapeNoise.SetSeed(seed);
    shapeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    shapeNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
    shapeNoise.SetFrequency(0.01f);
    shapeNoise.SetCellularJitter(1.0f);

    //Configuration du bruit pour les éléments (Valeur de la cellule)
    biomeNoise.SetSeed(seed);
    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    biomeNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
    biomeNoise.SetFrequency(0.008f);
}

NoiseGen::NoiseGen() : NoiseGen(1337) {} // Appel du constructeur avec une seed par défaut

float NoiseGen::getShape(float x, float y) {
    return shapeNoise.GetNoise(x, y);
}

float NoiseGen::getBiome(float x, float y) {
    return biomeNoise.GetNoise(x, y);
}
