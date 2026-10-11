#include "Player.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "World.h"
#include "Chunk.h"
#include "LoadedChunk.h"
#include <nlohmann/json.hpp>
#include <algorithm>

//outils pour passer de coordonnées globales à coordonnées chunk+ intérieur du chunk
auto floorDiv = [](int a, int b) { //à revoir pour les deux fonctions
    int r = a / b;
    if ((a ^ b) < 0 && a % b) --r;
    return r;
};
auto mod = [](int a, int b) {
    int m = a % b;
    if (m < 0) m += b;
    return m;
};


std::vector<LoadedChunk>& World::getLoadedChunks(){
    return loadedChunks;
};
LoadedChunk& World::getLoadedChunks(int id){
    return loadedChunks[id];
};

int World::getSizeLoadedChunks(){
return loadedChunks.size();
};

//Génère et insère un chunk dans les chunks loadés
void World::LoadChunk(int x_relative,int y_relative, NoiseGen& noise){
    Chunk newChunk;
    newChunk.genLayout(noise, x_relative, y_relative);
    LoadedChunk newLoadedChunk;
    newLoadedChunk.chunk = newChunk;
    newLoadedChunk.x_relative=x_relative;
    newLoadedChunk.y_relative=y_relative;
    applyChanges(newLoadedChunk); //rajoute les changements sauvegardés
    loadedChunks.push_back(newLoadedChunk);
};

//delete un LoadedChunk du vecteur loadedChunks. Attention, comme erase() change les indices, c'est mieux de commencer à effacer par la fin si on en efface plusieurs.
void World::DeleteChunk(int i){
    loadedChunks.erase(loadedChunks.begin() + i);
};

//Prend la liste des chunks loadés et vérifie que celui aux coordonnées relatives x,y s'y trouve.
bool World::chunkIsLoaded(int x,int y){
    for(int i =0; i<loadedChunks.size(); i++){
        if(loadedChunks[i].x_relative==x && loadedChunks[i].y_relative==y){
            return true;
        };
    };
    return false;
};

//vérifie si une tile chargée est de l'eau ou du sol.
bool World::isLand(int worldX, int worldY) const {

    int relative_x = floorDiv(worldX, Chunk::chunkSize);
    int relative_y = floorDiv(worldY, Chunk::chunkSize);
    int x = mod(worldX, Chunk::chunkSize);
    int y = mod(worldY, Chunk::chunkSize);
    for (const auto& chunk : loadedChunks) {
        if (chunk.x_relative == relative_x &&
            chunk.y_relative == relative_y) {
            const Tile& tile = chunk.chunk.getTile(x, y);
            return tile.getTile_id() != 0;
        }
    }
    return false;
};

int World::getTileId(int worldX, int worldY) const {
    int relative_x = floorDiv(worldX, Chunk::chunkSize);
    int relative_y = floorDiv(worldY, Chunk::chunkSize);
    int x = mod(worldX, Chunk::chunkSize);
    int y = mod(worldY, Chunk::chunkSize);
    for (const auto& chunk : loadedChunks) {
        if (chunk.x_relative == relative_x && chunk.y_relative == relative_y) {
            return chunk.chunk.getTile(x, y).getTile_id();
        }
    }
    return 0; // 0 = eau par défaut si le chunk est pas chargé
}

void World::setWorldTile(int worldX, int worldY, int tile_id) {
    int relative_x = floorDiv(worldX, Chunk::chunkSize);
    int relative_y = floorDiv(worldY, Chunk::chunkSize);

    int x = mod(worldX, Chunk::chunkSize);
    int y = mod(worldY, Chunk::chunkSize);

    for (auto& chunk : loadedChunks) {
        if (chunk.x_relative == relative_x && chunk.y_relative == relative_y) {
            chunk.chunk.setLayout(x, y, tile_id);
            if(isLoadingSave==false) addChange(worldX, worldY, tile_id); //Si c'est pas la phase de load, enregistre le changement
            return;
        }
    }
};

void World::addChange(int worldX, int worldY, int tile_id){
    newChanges.push_back({worldX, worldY, tile_id});
};

void World::loadSave(const std::string& filename,
                     Player& player,
                     QuestManager& quests,
                     Boat& boat)
{
    isLoadingSave = true;

    loadedChanges.clear();

    std::ifstream file(filename);

    if (!file.is_open()) {

        isLoadingSave = false;

        return;
    }

    nlohmann::json json;

    file >> json;


    //Chargement Tiles modifiées
    if (json.contains("changes")) {

        for (auto& e : json["changes"]) {

            TileChange chunk;

            chunk.x = e["x"];
            chunk.y = e["y"];
            chunk.tile_id = e["tile"];

            loadedChanges.push_back(chunk);
        }

        for (auto& change : loadedChanges) {

            setWorldTile(
                change.x,
                change.y,
                change.tile_id
            );
        }
    }

    
    //Chargement données player
    if (json.contains("player")) {

        player.setPosition(
            sf::Vector2f(
                json["player"]["x"],
                json["player"]["y"]
            )
        );

        player.setHp(
            json["player"]["hp"]
        );

        player.setKills(
            json["player"]["kills"]
        );

        player.setPowers(
            json["player"]["fire"],
            json["player"]["water"],
            json["player"]["air"],
            json["player"]["earth"]
        );
    }

   
    //Chargement état quêtes
    if (json.contains("quests")) {

        auto& q = json["quests"];

        quests.setQuestData(
            quests.getFireQuest(),
            q["fire"]["state"],
            q["fire"]["progress"]
        );

        quests.setQuestData(
            quests.getWaterQuest(),
            q["water"]["state"],
            q["water"]["progress"]
        );

        quests.setQuestData(
            quests.getAirQuest(),
            q["air"]["state"],
            q["air"]["progress"]
        );

        quests.setQuestData(
            quests.getEarthQuest(),
            q["earth"]["state"],
            q["earth"]["progress"]
        );
    }

    
    //Chargement bateau
    if (json.contains("boat")) {

        boat.setDockPosition(

            sf::Vector2f(
                json["boat"]["dock_x"],
                json["boat"]["dock_y"]
            )
        );

        if (json["boat"]["active"]) {

            boat.embark(
                boat.getPosition()
            );
        }

        // Fragments
        if (json["boat"].contains("fragments")) {

            boat.clearFragments();

            for (auto& frag : json["boat"]["fragments"]) {

                boat.addFragment(

                    sf::Vector2f(
                        frag["x"],
                        frag["y"]
                    ),

                    frag["collected"]
                );
            }
        }
    }

    isLoadingSave = false;
}

void World::save(const std::string& filename,
                 Player& player,
                 QuestManager& quests,
                 Boat& boat)
{
    nlohmann::json json;

    std::filesystem::create_directories(
        std::filesystem::path(filename).parent_path()
    );

  
    //Changements du terrain
    std::vector<TileChange> allChanges = loadedChanges;

    allChanges.insert(
        allChanges.end(),
        newChanges.begin(),
        newChanges.end()
    );

    for (const auto& change : allChanges) {

        json["changes"].push_back({
            {"x", change.x},
            {"y", change.y},
            {"tile", change.tile_id}
        });
    }

   
    //Données joueur
    json["player"]["x"] = player.getPosition().x;
    json["player"]["y"] = player.getPosition().y;

    json["player"]["hp"] = player.getHp();
    json["player"]["kills"] = player.getKillCount();

    json["player"]["fire"]  = player.getElementLevel(Element::FIRE);
    json["player"]["water"] = player.getElementLevel(Element::WATER);
    json["player"]["air"]   = player.getElementLevel(Element::AIR);
    json["player"]["earth"] = player.getElementLevel(Element::EARTH);

    
    //Données quêtes
    json["quests"]["fire"] = {
        {"state", (int)quests.getFireQuest().state},
        {"progress", quests.getFireQuest().progress}
    };

    json["quests"]["water"] = {
        {"state", (int)quests.getWaterQuest().state},
        {"progress", quests.getWaterQuest().progress}
    };

    json["quests"]["air"] = {
        {"state", (int)quests.getAirQuest().state},
        {"progress", quests.getAirQuest().progress}
    };

    json["quests"]["earth"] = {
        {"state", (int)quests.getEarthQuest().state},
        {"progress", quests.getEarthQuest().progress}
    };

    //Données bateau
    json["boat"]["x"] = boat.getPosition().x;
    json["boat"]["y"] = boat.getPosition().y;

    json["boat"]["dock_x"] = boat.getDockPosition().x;
    json["boat"]["dock_y"] = boat.getDockPosition().y;

    json["boat"]["active"] = boat.isActive();

    for (auto& frag : boat.getFragments()) {

        json["boat"]["fragments"].push_back({

            {"x", frag.worldPos.x},
            {"y", frag.worldPos.y},
            {"collected", frag.collected}

        });
    }

    
    //écriture fichier
    std::ofstream file(filename);

    if (file.is_open()) {
        file << json.dump(2);
    }

    loadedChanges = allChanges;

    newChanges.clear();
}

void World::applyChanges(LoadedChunk& chunk) {
    int relative_x = chunk.x_relative;
    int relative_y = chunk.y_relative;

    //parcours tous les changements pour vérifier si il faut les appliquer
    for (const auto& change : loadedChanges) {

        int change_relative_x = floorDiv(change.x, Chunk::chunkSize);
        int change_relative_y = floorDiv(change.y, Chunk::chunkSize);

        // vérifier si la modification appartient à ce chunk
        if (change_relative_x == relative_x && change_relative_y == relative_y) {

            int x = mod(change.x, Chunk::chunkSize);
            int y = mod(change.y, Chunk::chunkSize);

            chunk.chunk.setLayout(x, y, change.tile_id);
        };
    };

    //Meme chose, mais pour newChanges
    for (const auto& change : newChanges) {

        int change_relative_x = floorDiv(change.x, Chunk::chunkSize);
        int change_relative_y = floorDiv(change.y, Chunk::chunkSize);

        //vérifie si la modification appartient au chunk
        if (change_relative_x == relative_x && change_relative_y == relative_y) {

            int x = mod(change.x, Chunk::chunkSize);
            int y = mod(change.y, Chunk::chunkSize);

            chunk.chunk.setLayout(x, y, change.tile_id);
        };
    };
}
void World::updateChunks(const sf::View& view, NoiseGen& noise) {
    float gridStepX = 26.0f;
    float gridStepY = 13.0f;

    auto isoToCartesian = [gridStepX, gridStepY](float isoX, float isoY) -> sf::Vector2f {
        float cartX = (isoX / gridStepX + isoY / gridStepY) / 2.0f;
        float cartY = (isoY / gridStepY - isoX / gridStepX) / 2.0f;
        return {cartX, cartY};
    };

    sf::FloatRect viewBorder(view.getCenter() - view.getSize() / 2.f, view.getSize());
    sf::Vector2f topLeft = viewBorder.position;
    sf::Vector2f topRight = {viewBorder.position.x + viewBorder.size.x, viewBorder.position.y};
    sf::Vector2f bottomLeft = {viewBorder.position.x, viewBorder.position.y + viewBorder.size.y};
    sf::Vector2f bottomRight = {viewBorder.position.x + viewBorder.size.x, viewBorder.position.y + viewBorder.size.y};

    sf::Vector2f tlCart = isoToCartesian(topLeft.x, topLeft.y);
    sf::Vector2f trCart = isoToCartesian(topRight.x, topRight.y);
    sf::Vector2f blCart = isoToCartesian(bottomLeft.x, bottomLeft.y);
    sf::Vector2f brCart = isoToCartesian(bottomRight.x, bottomRight.y);

    float minCartX = std::min({tlCart.x, trCart.x, blCart.x, brCart.x});
    float maxCartX = std::max({tlCart.x, trCart.x, blCart.x, brCart.x});
    float minCartY = std::min({tlCart.y, trCart.y, blCart.y, brCart.y});
    float maxCartY = std::max({tlCart.y, trCart.y, blCart.y, brCart.y});

    int minChunkX = std::floor(minCartX / Chunk::chunkSize) - 2;
    int maxChunkX = std::floor(maxCartX / Chunk::chunkSize) + 2;
    int minChunkY = std::floor(minCartY / Chunk::chunkSize) - 2;
    int maxChunkY = std::floor(maxCartY / Chunk::chunkSize) + 2;

    for (int x = minChunkX; x <= maxChunkX; ++x) {
        for (int y = minChunkY; y <= maxChunkY; ++y) {
            if (!chunkIsLoaded(x, y)) {
                LoadChunk(x, y, noise);
            }
        }
    }

    for(int i = getSizeLoadedChunks() - 1; i >= 0; --i){
        auto& c = getLoadedChunks(i);
        if(c.x_relative < minChunkX || c.x_relative > maxChunkX ||
           c.y_relative < minChunkY || c.y_relative > maxChunkY) {
            DeleteChunk(i);
        }
    }
}

void World::drawChunks(sf::RenderWindow& window, sf::Texture* texture) {
    float gridStepX = 26.0f;
    float gridStepY = 13.0f;
    auto& chunksToDraw = getLoadedChunks();

    std::sort(chunksToDraw.begin(), chunksToDraw.end(), [](const LoadedChunk& a, const LoadedChunk& b) {
        return (a.x_relative + a.y_relative) < (b.x_relative + b.y_relative);
    });

    for (int i = 0; i < getSizeLoadedChunks(); ++i) {
        LoadedChunk& currChunk = getLoadedChunks(i);
        float chunkScreenX = (currChunk.x_relative - currChunk.y_relative) * (Chunk::chunkSize * gridStepX);
        float chunkScreenY = (currChunk.x_relative + currChunk.y_relative) * (Chunk::chunkSize * gridStepY);

        sf::Vector2f position = {chunkScreenX, chunkScreenY};
        currChunk.chunk.draw(position, window, texture);
    }
}
