#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include "Tile.h"
#include "TileRegistry.h"
#include <SFML/System/Vector2.hpp>


Tile::Tile(){setTile(1);} //Constructeur par défaut

//Getters
int Tile::getTile_id() const {return tile_id;};
bool Tile::getIs_interactable() const {return is_interactable;};
bool Tile::getIs_walkable() const {return is_walkable;};

//Setters
void Tile::setTile(int x){
    if (x < 0 || x >= TileRegistry.size()) return; //évite le hors limite
    tile_id=x;
    name=TileRegistry[x].name;
    is_walkable=TileRegistry[x].is_walkable;
    is_interactable=TileRegistry[x].is_interactable;
}; 
        
//Permet de dessiner une tile aux coordonnées demandées.
void Tile::draw(sf::Vector2f position, sf::RenderWindow& window){
    sf::RectangleShape square;
    square.setSize(sf::Vector2f(tileSize, tileSize));
    square.setPosition(position);
    switch(tile_id){
        case 0: square.setFillColor(sf::Color::Blue); break; //Bleu
        case 1: square.setFillColor(sf::Color(100, 250, 50)); break; //Vert
        case 2: square.setFillColor(sf::Color(194, 178, 128)); break; //Ocre
        case 3: square.setFillColor(sf::Color(100, 250, 50)); break; //Vert
        case 4: square.setFillColor(sf::Color(43, 255, 255)); break; //Cyan
        case 5: square.setFillColor(sf::Color::Red); break;    //Rouge
        case 6: square.setFillColor(sf::Color(123, 63, 0)); break; //Brun
        case 7: square.setFillColor(sf::Color::White); break;     //Blanc
        case 8: square.setFillColor(sf::Color(0, 139, 139)); break; //Cyan foncé
        case 9: square.setFillColor(sf::Color(255, 165, 0)); break; //Orange
        case 10: square.setFillColor(sf::Color(255, 255, 0)); break; // Jaune
        default: square.setFillColor(sf::Color::Magenta); break; //Magenta, au cas où ça déconne on le voit.
    };
    window.draw(square);
};