#pragma once
#include "FastNoiseLite.h"

class NoiseGen {
private:
    FastNoiseLite shapeNoise; // Pour la forme de l'île (Océan, Plage, Terre)
    FastNoiseLite biomeNoise; // Pour le type d'élément (Feu, Eau, Air, Terre)

public:
    NoiseGen(int seed);
    NoiseGen(); // Constructeur par défaut

    float getShape(float x, float y);
    float getBiome(float x, float y);
};
