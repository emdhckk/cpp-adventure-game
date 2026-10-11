#include "TileFinder.h"

// Trouve la première tile d'une île et retourne sa position isométrique

sf::Vector2f findIslandTile(World& world, int targetTileId) {
    float gridStepX = 26.0f;
    float gridStepY = 13.0f;
    for (auto& lc : world.getLoadedChunks()) {
        for (int y = 2; y < Chunk::chunkSize - 2; y++) {
            for (int x = 2; x < Chunk::chunkSize - 2; x++) {
                if (lc.chunk.getTile(x, y).getTile_id() == targetTileId) {
                    float chunkScreenX = (lc.x_relative - lc.y_relative) * (Chunk::chunkSize * gridStepX);
                    float chunkScreenY = (lc.x_relative + lc.y_relative) * (Chunk::chunkSize * gridStepY);
                    float px = chunkScreenX + (x - y) * gridStepX;
                    float py = chunkScreenY + (x + y) * gridStepY;
                    return sf::Vector2f(px, py);
                }
            }
        }
    }
    return sf::Vector2f(-999999.f, -999999.f);
}