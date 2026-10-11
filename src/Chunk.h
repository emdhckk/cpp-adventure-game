#pragma once

#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include "NoiseGen.h"
#include "Tile.h"


class Chunk{
    public:
        //taille de chaque chunk, est une constante. écrit comme ça au cas où on veut changer de taille au fil du dev
        static const int chunkSize = 32;
        //Constructeurs
        Chunk();
        Chunk(std::array<std::array<Tile, chunkSize>, chunkSize> l);

        //Getters
        int getChunkSize() const;
        //renvoie le type de terrain (int) d'une case dans le chunk.
        Tile getTile(int x,int y) const;

        //Setter
        //Permet de changer une tile dans le layout aux coordonnées (x,y) au sein du chunk et de le remplacer par une tile avec son id.
        void setLayout(sf::Vector2i position, int id);
        void setLayout(int x, int y, int id);

        //Méthodes
        //permet de créer un layout à l'aide du bruit de perlin
        void genLayout(NoiseGen& noise, int x_relative, int y_relative);

        //Construit le VertexArray isométrique
        void buildMesh(int chunkX, int chunkY);

        //Permet d'afficher un chunk à une position (x;y)
        void draw(sf::Vector2f position, sf::RenderWindow& window, sf::Texture* texture);

    private:
        std::array<std::array<Tile, chunkSize>, chunkSize> layout;
        sf::VertexArray verticles;
        int storedChunkX = 0; // mémorisés pour rebuild du mesh après setLayout
        int storedChunkY = 0;
};