#include "Enemy.h"
#include <cmath>

Enemy::Enemy(sf::Vector2f startPos, float damage, float detectionRange, float hp) 
    : Entity(startPos, hp, 50.f) {
    this->damage = damage;
    this->detectionRange = detectionRange;
    this->maxHpSnapshot = hp;
}

float Enemy::getDamage() const {
    return damage;
}

void Enemy::setPosition(sf::Vector2f newPos) {
    position = newPos;
}

void Enemy::update(float dt, sf::Vector2f playerPos) {
    float dx = playerPos.x - position.x;
    float dy = playerPos.y - position.y;
    float distance = std::sqrt(dx*dx + dy*dy);

    if (distance < detectionRange && distance > 0) {
        position.x += (dx / distance) * speed * dt;
        position.y += (dy / distance) * speed * dt;

        // Direction selon le vecteur vers le joueur
        if (std::abs(dx) > std::abs(dy))
            direction = (dx > 0) ? 3 : 2; // est ou ouest
        else
            direction = (dy > 0) ? 0 : 1; // sud ou nord

        // Animation de marche
        animTimer += dt;
        if (animTimer >= 0.12f) {
            animTimer = 0.f;
            currentFrame = (currentFrame + 1) % 6;
        }
    } else {
        currentFrame = 0; // idle si hors portée
    }
}

void Enemy::update(float dt) {
    // Version sans position du joueur
}

void Enemy::draw(sf::RenderWindow& window) {
    if (spriteTexture && spriteTexture->getSize().x > 0) {
        // Ligne = direction (0=sud, 1=nord, 2=ouest, 3=est)
        sf::Sprite sprite(*spriteTexture);
        sprite.setTextureRect(sf::IntRect(
            sf::Vector2i(currentFrame * 64, direction * 64),
            sf::Vector2i(64, 64)
        ));
        sprite.setScale(sf::Vector2f(0.7f, 0.7f));
        sprite.setOrigin(sf::Vector2f(32.f, 48.f));
        sprite.setPosition(position);
        window.draw(sprite);
    } else {
        // Fallback : cercle rouge
        float radius = 9.f;
        sf::CircleShape shape(radius);
        shape.setOrigin(sf::Vector2f(radius, radius));
        shape.setPosition(position);
        shape.setFillColor(sf::Color(180, 20, 20));
        shape.setOutlineColor(sf::Color(255, 80, 80));
        shape.setOutlineThickness(2.f);
        window.draw(shape);
    }

    // Barre de vie flottante
    float barW = 28.f;
    float barH = 4.f;
    float barX = position.x - barW / 2.f;
    float barY = position.y - 28.f;

    sf::RectangleShape hpBg(sf::Vector2f(barW, barH));
    hpBg.setPosition(sf::Vector2f(barX, barY));
    hpBg.setFillColor(sf::Color(60, 0, 0, 200));
    window.draw(hpBg);

    float ratio = std::max(0.f, hp / maxHpSnapshot);
    sf::RectangleShape hpBar(sf::Vector2f(barW * ratio, barH));
    hpBar.setPosition(sf::Vector2f(barX, barY));
    sf::Color barColor;
    if (ratio > 0.6f)      barColor = sf::Color(60, 200, 60);
    else if (ratio > 0.3f) barColor = sf::Color(220, 200, 0);
    else                   barColor = sf::Color(220, 60, 0);
    hpBar.setFillColor(barColor);
    window.draw(hpBar);
}