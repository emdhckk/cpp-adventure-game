#include <array>
#include "Tile.h"
#include "Chunk.h"
#include <SFML/System/Vector2.hpp>
#include "NoiseGen.h"

// Constructeurs
Chunk::Chunk() {
    // Par défaut on remplit tout de herbe (tile 1)
    for (int y = 0; y < chunkSize; y++) {
        for (int x = 0; x < chunkSize; x++) {
            layout[y][x].setTile(1);
        }
    }
};
Chunk::Chunk(std::array<std::array<Tile, chunkSize>, chunkSize> l) : layout(l){};

// Getters
int Chunk::getChunkSize() const {return chunkSize;};
Tile Chunk::getTile(int x,int y) const {return layout[y][x];};

// Setters — on rebuild le mesh après chaque changement de tile pour que l'affichage reste à jour
void Chunk::setLayout(sf::Vector2i position, int id){
    layout[position.y][position.x].setTile(id);
    buildMesh(storedChunkX, storedChunkY);
};
void Chunk::setLayout(int x, int y, int id){
    layout[y][x].setTile(id);
    buildMesh(storedChunkX, storedChunkY);
};

// Génère le terrain d'un chunk à partir du bruit de Voronoï
void Chunk::genLayout(NoiseGen& noise, int x_relative, int y_relative){

    // Hash déterministe pour placer les reliques de façon reproductible
    auto chunkHash = [](int x, int y, int salt) -> unsigned int {
        unsigned int h = (unsigned int)(x * 73856093 ^ y * 19349663 ^ salt * 83492791);
        h = (h ^ (h >> 16)) * 0x45d9f3b;
        h = (h ^ (h >> 16)) * 0x45d9f3b;
        return h ^ (h >> 16);
    };

    // 1 chunk sur 6 contient une relique, placée à une position aléatoire mais fixe
    bool hasRelic  = (chunkHash(x_relative, y_relative, 1) % 6 == 0);
    int  relicX    =  4 + chunkHash(x_relative, y_relative, 3) % (chunkSize - 8);
    int  relicY    =  4 + chunkHash(x_relative, y_relative, 4) % (chunkSize - 8);
    
    for (int y = 0; y < chunkSize; y++) {
        for (int x = 0; x < chunkSize; x++) {
            float worldX = (float)(x + x_relative * chunkSize);
            float worldY = (float)(y + y_relative * chunkSize);

            // shapeNoise détermine si on est sur une île ou dans l'océan
            // biomeNoise détermine le type d'île (feu, eau, air, terre)
            float distanceToCenter = noise.getShape(worldX, worldY);
            float elementValue = noise.getBiome(worldX, worldY);
            
            if (distanceToCenter > -0.8f) {
                layout[y][x].setTile(0); // Océan
            }
            else if (distanceToCenter > -0.83f) {
                layout[y][x].setTile(2); // Plage (transition entre océan et île)
            }
            else {
                // Intérieur de l'île — on choisit le biome selon la valeur du bruit
                if (elementValue < -0.5f) {
                    layout[y][x].setTile(4); // Île Eau
                }
                else if (elementValue < 0.0f) {
                    layout[y][x].setTile(3); // Île Air
                }
                else if (elementValue < 0.5f) {
                    layout[y][x].setTile(6); // Île Terre
                }
                else {
                    layout[y][x].setTile(5); // Île Feu
                }
            }
        }
    }
    
    // On place la relique sur la bonne tile selon le biome
    if (hasRelic) {
        int tileId = layout[relicY][relicX].getTile_id();
        if (tileId == 3) layout[relicY][relicX].setTile(7);  // Relique d'Air
        else if (tileId == 4) layout[relicY][relicX].setTile(8);  // Relique d'Eau
        else if (tileId == 5) layout[relicY][relicX].setTile(9);  // Relique de Feu
        else if (tileId == 6) layout[relicY][relicX].setTile(10); // Relique de Terre
    }
    buildMesh(x_relative, y_relative);
}

// Construit le VertexArray isométrique du chunk
// On utilise des triangles pour afficher chaque tuile depuis la spritesheet
void Chunk::buildMesh(int chunkX, int chunkY) {
    storedChunkX = chunkX;
    storedChunkY = chunkY;
    verticles.setPrimitiveType(sf::PrimitiveType::Triangles);
    verticles.resize(chunkSize * chunkSize * 6); // 2 triangles = 6 sommets par tuile

    // Taille d'une cellule dans la spritesheet
    int texCellWidth = 52;
    int texCellHeight = 52;

    // Paramètres isométriques dérivés de la taille des tuiles
    float gridStepX = texCellWidth / 2.0f;
    float gridStepY = texCellHeight / 4.0f;

    for (int y = 0; y < chunkSize; ++y) {
        for (int x = 0; x < chunkSize; ++x) {
            int tile_id = layout[y][x].getTile_id();
            int tcol = 0, tlig = 0;

            // Correspondance entre l'ID de tuile et sa position dans la spritesheet
            switch(tile_id) {
                case 0:  tcol = 1; tlig = 2; break; // Eau
                case 1:  tcol = 1; tlig = 0; break; // Herbe
                case 2:  tcol = 3; tlig = 1; break; // Sable
                case 3:  tcol = 1; tlig = 3; break; // air_land
                case 4:  tcol = 0; tlig = 1; break; // water_land
                case 5:  tcol = 1; tlig = 1; break; // fire_land
                case 6:  tcol = 2; tlig = 2; break; // earth_land
                case 7:  tcol = 2; tlig = 3; break; // air_relic
                case 8:  tcol = 0; tlig = 2; break; // water_relic
                case 9:  tcol = 2; tlig = 1; break; // fire_relic
                case 10: tcol = 3; tlig = 0; break; // earth_relic
                case 11: tcol = 3; tlig = 0; break; // pouvoir terre
                default: tcol = 1; tlig = 0; break;
            }

            // Coordonnées dans la spritesheet
            float u = tcol * texCellWidth;
            float v = tlig * texCellHeight;

            // Conversion en coordonnées isométriques à l'écran
            float screenX = std::round((x - y) * gridStepX);
            float screenY = std::round((x + y) * gridStepY);

            sf::Vertex* triangles = &verticles[(x + y * chunkSize) * 6];

            // Rectangle d'affichage de la tuile
            sf::Vector2f topLeft(screenX, screenY);
            sf::Vector2f topRight(screenX + texCellWidth, screenY);
            sf::Vector2f bottomRight(screenX + texCellWidth, screenY + texCellHeight);
            sf::Vector2f bottomLeft(screenX, screenY + texCellHeight);

            // 2 triangles pour former le rectangle
            triangles[0].position = topLeft;
            triangles[1].position = topRight;
            triangles[2].position = bottomRight;
            triangles[3].position = topLeft;
            triangles[4].position = bottomRight;
            triangles[5].position = bottomLeft;

            // Coordonnées UV dans la spritesheet
            triangles[0].texCoords = sf::Vector2f(u, v);
            triangles[1].texCoords = sf::Vector2f(u + texCellWidth, v);
            triangles[2].texCoords = sf::Vector2f(u + texCellWidth, v + texCellHeight);
            triangles[3].texCoords = sf::Vector2f(u, v);
            triangles[4].texCoords = sf::Vector2f(u + texCellWidth, v + texCellHeight);
            triangles[5].texCoords = sf::Vector2f(u, v + texCellHeight);
        }
    }
}

// Affiche tout le chunk en un seul draw call grâce au VertexArray
void Chunk::draw(sf::Vector2f position, sf::RenderWindow& window, sf::Texture* texture) {
    sf::RenderStates states;
    sf::Transform transform;
    transform.translate(position); // déplace le chunk à sa position dans le monde
    states.transform = transform;
    states.texture = texture;
    window.draw(verticles, states);
}