#pragma once
#include "Entity.h"
#include <vector>
#include <memory>
#include "World.h"
#include "Enemy.h"


enum class Element {
    FIRE,   // feu - beaucoup de dégâts
    WATER,  // eau - soigne le joueur
    AIR,    // air - repousse les ennemis
    EARTH   // terre - crée des ponts
};

class Player : public Entity
{
private:
    std::vector<int> inventory;
    Element currentElement;
    int firePower;
    int waterPower;
    int airPower;
    int earthPower;
    float earthCooldown;
    int killCount;
    float powerFlashTimer;
    sf::Texture* spriteTexture = nullptr;

    // Animation
    float animTimer = 0.f;
    float animSpeed = 0.1f;
    int currentFrame = 0;
    bool isMoving = false;
    int direction = 0; // 0=south, 1=north, 2=west, 3=east

public:
    Player(sf::Vector2f startPos);
    void setSpriteTexture(sf::Texture* tex) { spriteTexture = tex; }

    void update(float dt) override;
    void update(float dt, World& world);
    void draw(sf::RenderWindow& window) override;
    void drawHUD(sf::RenderWindow& window, sf::View& view, sf::Font& font);

    void addItem(int itemId);
    bool hasItem(int itemId);
    void removeItem(int itemId);

    void addKill();
    int getKillCount() const;

    void usePower(std::vector<std::unique_ptr<Entity>>& entities, World& world);

    void switchElement(Element e);
    Element getCurrentElement() const;
    void upgradeElement(Element e);
    int getElementLevel(Element e) const;

    // Setters pour la sauvegarde
    void setPosition(sf::Vector2f pos) { position = pos; }
    void setHp(float newHp) { hp = newHp; }
    void setKills(int k) { killCount = k; }
    void setPowers(int f, int w, int a, int e) { firePower = f; waterPower = w; airPower = a; earthPower = e; }
};
