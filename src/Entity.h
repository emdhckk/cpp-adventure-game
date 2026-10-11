#pragma once
#include <SFML/Graphics.hpp>

class Entity {
    protected:
        sf::Vector2f position; //Position du personnage sur la carte
        float hp; //PV du personnage
        float maxHp; //PV maximum possible du personnage
        float speed; //Vitesse du personnage

    public:
        Entity(sf::Vector2f startPos, float hp, float speed); //Donne les valeurs initiales quand on crée l'entité
        sf::Vector2f getPosition() const;
        float getHp() const;
        bool isAlive() const;

        void takeDamage(float amount);

        virtual void update(float dt) = 0; //Le = 0 veut dire que chaque classe fille doit obligatoirement écrire sa propre version de update
        virtual void draw(sf::RenderWindow& window) = 0; //draw déssine l'entity sur l'écran et sf::RenderWindow& window c'est la fe,etre SFML dans laquelle on déssine

        virtual ~Entity() = default; //Nettoayge automatique pour libérer la mémoire
};