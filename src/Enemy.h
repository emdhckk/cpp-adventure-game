#pragma once
#include "Entity.h"

class Enemy : public Entity
{
private:
    float damage;
    float detectionRange;
    float maxHpSnapshot;

    sf::Texture* spriteTexture = nullptr;
    float animTimer = 0.f;
    int currentFrame = 0;
    int direction = 0; // 0=sud, 1=nord, 2=ouest, 3=est

public:
    Enemy(sf::Vector2f startPos, float damage, float detectionRange, float hp = 80.f);

    float getDamage() const;
    void setPosition(sf::Vector2f newPos);
    void setSpriteTexture(sf::Texture* tex) { spriteTexture = tex; }

    void update(float dt, sf::Vector2f playerPos);
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
};