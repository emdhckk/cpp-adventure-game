#pragma once

#include "Chunk.h"
//c'est un header only, pas besoin de .cpp car c'est uniquement une structure.
//On utilisera un array de Loadedchunks pour garder enregistrer ce qu'ils sont et où ils sont par rapport aux autres chunks.
struct LoadedChunk{
        Chunk chunk;
        int x_relative;
        int y_relative;
};





