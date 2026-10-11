#pragma once
#include "Entity.h"
#include "World.h"
#include <SFML/Graphics.hpp>

class Neutral : public Entity
{
private:
    float wanderTimer;
    sf::Vector2f direction;
    bool frozen = false;
    int islandType = -1; // tile_id de l'île (3=air, 4=eau, 5=feu, 6=terre)

    sf::Texture* spriteTexture = nullptr;
    float animTimer = 0.f;
    int currentFrame = 0;
    int direction4 = 0;

public:
    Neutral(sf::Vector2f startPos);

    void update(float dt) override;
    void update(float dt, const World& world);
    void draw(sf::RenderWindow& window) override;

    void setSpriteTexture(sf::Texture* tex) { spriteTexture = tex; }
    void setFrozen(bool f) { frozen = f; }
    bool isFrozen() const { return frozen; }
    void setIslandType(int type) { islandType = type; }
    int getIslandType() const { return islandType; }
};