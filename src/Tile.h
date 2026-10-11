#pragma once


#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include "TileRegistry.h"
#include <SFML/System/Vector2.hpp>

class Tile{
    private:

        int tile_id;
        std::string name;
        bool is_walkable;
        bool is_interactable;
        
    public:

        Tile();
        static const int tileSize = 16;
    //Getters
        int getTile_id() const;
        bool getIs_interactable() const;
        bool getIs_walkable() const;

    //Setters
        void setTile(int x);

    //Méthodes
    //Permet de dessiner une tile aux coordonnées demandées.
    void draw(sf::Vector2f, sf::RenderWindow& window);
};