#pragma once

#include <cmath>
#include <vector>
#include "Chunk.h"
#include "LoadedChunk.h"
#include "TileChange.h"
#include "QuestManager.h"
#include "Boat.h"
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics.hpp>


class Player; 

//On utilisera un array de Loadedchunks pour garder enregistrer ce qu'ils sont et où ils sont par rapport aux autres chunks.
class World{
    private:
        bool isLoadingSave = false; //flag permettant à certaines fonctions de procédér différement si on est en train de load la save ou pas
        std::vector<LoadedChunk> loadedChunks; //Chunks chargés en mémoire
        std::vector<TileChange> newChanges;    //modifications apportées par le joueur, à enregistrer
        std::vector<TileChange> loadedChanges; //modifications enregistrées dans le json savefile
    public:
    //getters
    //Le premier renvoie tous les chunks, le second un chunk spécifique
    std::vector<LoadedChunk>& getLoadedChunks();
    LoadedChunk& getLoadedChunks(int id);

    //Renvoie le nombre de chunks actuellement loadés en mémoire.
    int getSizeLoadedChunks();

    //Génère et insère un chunk dans les chunks loadés
    void LoadChunk(int x,int y,NoiseGen& noise);

    //delete un LoadedChunk du vecteur loadedChunks
    void DeleteChunk(int i);

    //Prend la liste des chunks loadés et vérifie que celui aux coordonnées relatives x,y s'y trouve.
    bool chunkIsLoaded(int x,int y);

    //vérifie si une tile chargée est de l'eau ou du sol.
    bool isLand(int worldX, int worldY) const;

    // renvoie l'ID de la tile aux coordonnées globales
    int getTileId(int worldX, int worldY) const;

    //permet de modifier une tile, mais à l'aide des coordonnées globales
    void setWorldTile(int worldX, int worldY, int tile_id); //

    //ajoute une entrée à newChanges
    void addChange(int worldX, int worldY, int tile_id);

    //sauvegarde vers le json
    void save(const std::string& filename, Player& player, QuestManager& quests, Boat& boat);

    //charge depuis le json (mais prend le joueur en compte mtn)
    void loadSave(const std::string& filename, Player& player, QuestManager& quests, Boat& boat);

    //appplique les changements sur la map
    void applyChanges(LoadedChunk& chunk);

    //gère le chargement et le déchargement isométrique
    void updateChunks(const sf::View& view, NoiseGen& noise);

    //gère le z-sorting et le dessin isométrique
    void drawChunks(sf::RenderWindow& window, sf::Texture* texture);
};
