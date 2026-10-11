#include "Neutral.h"
#include <cstdlib>
#include <cmath>

Neutral::Neutral(sf::Vector2f startPos) : Entity(startPos, 99999.f, 60.f) {
    wanderTimer = 0.f;
    direction = sf::Vector2f(1.f, 0.f);
    direction4 = 3; // est par défaut
}

void Neutral::update(float dt) {
    if (frozen) return;

    wanderTimer -= dt;
    if (wanderTimer <= 0.f) {
        wanderTimer = 2.f + (rand() % 3);
        int dir = rand() % 4;
        if (dir == 0) { direction = sf::Vector2f(1.f, 0.f);  direction4 = 3; }
        if (dir == 1) { direction = sf::Vector2f(-1.f, 0.f); direction4 = 1; }
        if (dir == 2) { direction = sf::Vector2f(0.f, 1.f);  direction4 = 0; }
        if (dir == 3) { direction = sf::Vector2f(0.f, -1.f); direction4 = 2; }
    }
    position += direction * speed * dt;

    animTimer += dt;
    if (animTimer >= 0.15f) {
        animTimer = 0.f;
        currentFrame = (currentFrame + 1) % 6;
    }
}

void Neutral::update(float dt, const World& world) {
    if (frozen) return;

    wanderTimer -= dt;
    if (wanderTimer <= 0.f) {
        wanderTimer = 2.f + (rand() % 3);
        int dir = rand() % 4;
        if (dir == 0) { direction = sf::Vector2f(1.f, 0.f);  direction4 = 3; }
        if (dir == 1) { direction = sf::Vector2f(-1.f, 0.f); direction4 = 1; }
        if (dir == 2) { direction = sf::Vector2f(0.f, 1.f);  direction4 = 0; }
        if (dir == 3) { direction = sf::Vector2f(0.f, -1.f); direction4 = 2; }
    }

    sf::Vector2f newPos = position + direction * speed * dt;

    float gridStepX = 26.0f;
    float gridStepY = 13.0f;
    float cartX = (newPos.x / gridStepX + newPos.y / gridStepY) / 2.0f;
    float cartY = (newPos.y / gridStepY - newPos.x / gridStepX) / 2.0f;
    int tileX = static_cast<int>(std::floor(cartX));
    int tileY = static_cast<int>(std::floor(cartY));

    if (world.isLand(tileX, tileY)) {
        position = newPos;
    } else {
        direction = -direction;
        direction4 = (direction4 + 2) % 4;
        wanderTimer = 1.f;
    }

    animTimer += dt;
    if (animTimer >= 0.15f) {
        animTimer = 0.f;
        currentFrame = (currentFrame + 1) % 6;
    }
}

void Neutral::draw(sf::RenderWindow& window) {
    if (spriteTexture && spriteTexture->getSize().x > 0) {
        sf::Sprite sprite(*spriteTexture);
        sprite.setTextureRect(sf::IntRect(
            sf::Vector2i(currentFrame * 64, direction4 * 64),
            sf::Vector2i(64, 64)
        ));
        sprite.setScale(sf::Vector2f(0.7f, 0.7f));
        sprite.setOrigin(sf::Vector2f(32.f, 48.f));
        sprite.setPosition(position);
        window.draw(sprite);
    } else {
        // Fallback : cercle orange
        sf::CircleShape shape(9.f);
        shape.setOrigin(sf::Vector2f(9.f, 9.f));
        shape.setPosition(sf::Vector2f(position.x, position.y - 8.f));
        shape.setFillColor(sf::Color(255, 140, 0));
        shape.setOutlineColor(sf::Color(200, 100, 0));
        shape.setOutlineThickness(2.f);
        window.draw(shape);

        sf::CircleShape head(5.f);
        head.setOrigin(sf::Vector2f(5.f, 5.f));
        head.setPosition(sf::Vector2f(position.x, position.y - 22.f));
        head.setFillColor(sf::Color(255, 210, 160));
        head.setOutlineColor(sf::Color(180, 120, 60));
        head.setOutlineThickness(1.5f);
        window.draw(head);
    }
}