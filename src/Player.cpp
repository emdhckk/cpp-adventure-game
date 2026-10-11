#include "Player.h"
#include "World.h"
#include <cmath>
#include <iostream>
#include <string>
#include <memory>

Player::Player(sf::Vector2f startPos) : Entity(startPos, 100.f, 200.f)
{
    currentElement = Element::FIRE;
    firePower = 1;
    waterPower = 1;
    airPower = 1;
    earthPower = 1;
    earthCooldown = 0.f;
    killCount = 0;
    powerFlashTimer = 0.f;
}

void Player::update(float dt) {
    if (earthCooldown > 0.f) earthCooldown -= dt;
    if (powerFlashTimer > 0.f) powerFlashTimer -= dt;

    // Anime même sans mouvement (idle cycle lent)
    animTimer += dt;
    if (animTimer >= animSpeed) {
        animTimer = 0.f;
        if (isMoving)
            currentFrame = (currentFrame + 1) % 6;
        else
            currentFrame = 0;
    }
    isMoving = false; // reset chaque frame, mis à true dans update(dt, world)
}

void Player::update(float dt, World& world) {
    if (earthCooldown > 0.f) earthCooldown -= dt;
    if (powerFlashTimer > 0.f) powerFlashTimer -= dt;

    sf::Vector2f move(0.f, 0.f);
    isMoving = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) { move.y -= speed * dt; isMoving = true; direction = 1; } // nord
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { move.y += speed * dt; isMoving = true; direction = 0; } // sud
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) { move.x -= speed * dt; isMoving = true; direction = 2; } // ouest
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { move.x += speed * dt; isMoving = true; direction = 3; } // est

    // Animation
    animTimer += dt;
    float frameSpeed = isMoving ? 0.15f : 0.5f;
    if (animTimer >= frameSpeed) {
        animTimer = 0.f;
        currentFrame = (currentFrame + 1) % 6;
    }

    sf::Vector2f newPos = position + move;

    float gridStepX = 26.0f;
    float gridStepY = 13.0f;

    float cartX = (newPos.x / gridStepX + newPos.y / gridStepY) / 2.0f;
    float cartY = (newPos.y / gridStepY - newPos.x / gridStepX) / 2.0f;

    int tileX = static_cast<int>(std::floor(cartX));
    int tileY = static_cast<int>(std::floor(cartY));

    if (world.isLand(tileX, tileY)) {
        position = newPos;
    }

    // Recalcule les coordonnées tile depuis la position effective du joueur
    // (même formule que World::updateChunks, sans offset)
    int pTileX = static_cast<int>(std::floor((position.x / gridStepX + position.y / gridStepY) / 2.0f));
    int pTileY = static_cast<int>(std::floor((position.y / gridStepY - position.x / gridStepX) / 2.0f));

    // Vérifier si on se trouve sur une relique
    int tileId = world.getTileId(pTileX, pTileY);
    if (tileId >= 7 && tileId <= 10) {
        Element relicElement = Element::AIR;
        if (tileId == 7)  relicElement = Element::AIR;
        if (tileId == 8)  relicElement = Element::WATER;
        if (tileId == 9)  relicElement = Element::FIRE;
        if (tileId == 10) relicElement = Element::EARTH;
        upgradeElement(relicElement);

        world.setWorldTile(pTileX, pTileY, 1);
    }
}

void Player::addKill() {
    killCount++;
}

int Player::getKillCount() const {
    return killCount;
}

int Player::getElementLevel(Element e) const {
    if (e == Element::FIRE)  return firePower;
    if (e == Element::WATER) return waterPower;
    if (e == Element::AIR)   return airPower;
    if (e == Element::EARTH) return earthPower;
    return 1;
}

void Player::draw(sf::RenderWindow& window) {
    // Effet visuel du pouvoir : anneau qui s'étend
    if (powerFlashTimer > 0.f) {
        float ratio = 1.f - (powerFlashTimer / 0.3f);

        sf::Color flashColor;
        float maxRadius = 0.f;
        if (currentElement == Element::FIRE)  { flashColor = sf::Color(255, 100, 0,  (uint8_t)(180 * (1.f - ratio))); maxRadius = 100.f; }
        if (currentElement == Element::WATER) { flashColor = sf::Color(0,   160, 255,(uint8_t)(180 * (1.f - ratio))); maxRadius = 40.f;  }
        if (currentElement == Element::AIR)   { flashColor = sf::Color(200, 235, 255,(uint8_t)(180 * (1.f - ratio))); maxRadius = 150.f; }
        if (currentElement == Element::EARTH) { flashColor = sf::Color(160, 100, 20, (uint8_t)(180 * (1.f - ratio))); maxRadius = 60.f;  }

        float r = maxRadius * ratio;
        sf::CircleShape ring(r);
        ring.setOrigin(sf::Vector2f(r, r));
        ring.setPosition(position);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineColor(flashColor);
        ring.setOutlineThickness(3.f);
        window.draw(ring);

        if (currentElement == Element::FIRE || currentElement == Element::AIR) {
            float r2 = maxRadius * std::max(0.f, ratio - 0.2f);
            sf::CircleShape ring2(r2);
            ring2.setOrigin(sf::Vector2f(r2, r2));
            ring2.setPosition(position);
            ring2.setFillColor(sf::Color::Transparent);
            ring2.setOutlineColor(flashColor);
            ring2.setOutlineThickness(2.f);
            window.draw(ring2);
        }
    }

    // Sprite du joueur selon l'élément actif
    if (spriteTexture && spriteTexture->getSize().x > 0) {
        // Ligne = elem*4 + direction (0=sud, 1=nord, 2=ouest, 3=est)
        int elemRow = 0;
        if (currentElement == Element::FIRE)  elemRow = 0;
        if (currentElement == Element::WATER) elemRow = 1;
        if (currentElement == Element::AIR)   elemRow = 2;
        if (currentElement == Element::EARTH) elemRow = 3;

        int sheetRow = elemRow * 4 + direction;

        sf::Sprite sprite(*spriteTexture);
        sprite.setTextureRect(sf::IntRect(
            sf::Vector2i(currentFrame * 64, sheetRow * 64),
            sf::Vector2i(64, 64)
        ));
        sprite.setScale(sf::Vector2f(0.7f, 0.7f));
        sprite.setOrigin(sf::Vector2f(32.f, 48.f));
        sprite.setPosition(position);
        window.draw(sprite);
    } else {
        // Fallback si la texture n'est pas chargée : cercles colorés
        sf::CircleShape shape(10.f);
        shape.setOrigin(sf::Vector2f(10.f, 10.f));
        shape.setPosition(sf::Vector2f(position.x, position.y - 10.f));
        sf::Color bodyColor;
        if (currentElement == Element::FIRE)  bodyColor = sf::Color(255, 80, 30);
        if (currentElement == Element::WATER) bodyColor = sf::Color(30, 160, 255);
        if (currentElement == Element::AIR)   bodyColor = sf::Color(200, 230, 255);
        if (currentElement == Element::EARTH) bodyColor = sf::Color(100, 60, 10);
        shape.setFillColor(bodyColor);
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(2.f);
        window.draw(shape);
    }
}

void Player::addItem(int itemId) {
    inventory.push_back(itemId);
}

bool Player::hasItem(int itemId) {
    for (int i = 0; i < (int)inventory.size(); i++) {
        if (inventory[i] == itemId) return true;
    }
    return false;
}

void Player::removeItem(int itemId) {
    for (int i = 0; i < (int)inventory.size(); i++) {
        if (inventory[i] == itemId) {
            inventory.erase(inventory.begin() + i);
            return;
        }
    }
}

void Player::drawHUD(sf::RenderWindow& window, sf::View& view, sf::Font& font) {
    // Coin supérieur gauche (barre de vie + kills)
    sf::Vector2f baseLeft = view.getCenter() - view.getSize() / 2.f;
    baseLeft.x += 12.f;
    baseLeft.y += 12.f;

    // Coin supérieur droit (éléments)
    sf::Vector2f baseRight = view.getCenter() - view.getSize() / 2.f;
    baseRight.x += view.getSize().x - 4 * 48.f - 12.f;
    baseRight.y += 12.f;

    sf::Vector2f base = baseLeft; // alias pour la barre de vie

    // --- Barre de vie ---
    float barW = 160.f;
    float barH = 14.f;

    sf::RectangleShape hpBg(sf::Vector2f(barW, barH));
    hpBg.setPosition(base);
    hpBg.setFillColor(sf::Color(60, 0, 0, 200));
    window.draw(hpBg);

    sf::RectangleShape hpBar(sf::Vector2f(barW * (hp / maxHp), barH));
    hpBar.setPosition(base);
    hpBar.setFillColor(sf::Color(220, 40, 40));
    window.draw(hpBar);

    sf::Text hpText(font);
    hpText.setCharacterSize(11);
    hpText.setFillColor(sf::Color::Black);
    hpText.setPosition(sf::Vector2f(base.x + 3.f, base.y));
    hpText.setString("PV: " + std::to_string((int)hp) + " / " + std::to_string((int)maxHp));
    window.draw(hpText);

    // --- Kills ---
    sf::Text killText(font);
    killText.setCharacterSize(11);
    killText.setFillColor(sf::Color::Black);
    killText.setPosition(sf::Vector2f(base.x, base.y + barH + 4.f));
    killText.setString("Kills: " + std::to_string(killCount));
    window.draw(killText);

    // --- Barre de cooldown TERRE (bas gauche, sous kills) ---
    float cdY = base.y + barH + 22.f;
    float maxCd = 10.f;
    float cdRatio = std::max(0.f, 1.f - earthCooldown / maxCd);

    sf::RectangleShape cdBg(sf::Vector2f(barW, 10.f));
    cdBg.setPosition(sf::Vector2f(base.x, cdY));
    cdBg.setFillColor(sf::Color(30, 30, 30, 180));
    window.draw(cdBg);

    sf::RectangleShape cdBar(sf::Vector2f(barW * cdRatio, 10.f));
    cdBar.setPosition(sf::Vector2f(base.x, cdY));
    cdBar.setFillColor(earthCooldown > 0.f ? sf::Color(100, 60, 10) : sf::Color(160, 100, 20));
    window.draw(cdBar);

    sf::Text cdText(font);
    cdText.setCharacterSize(9);
    cdText.setFillColor(sf::Color::Black);
    cdText.setPosition(sf::Vector2f(base.x + 3.f, cdY));
    cdText.setString(earthCooldown > 0.f
        ? "TERRE: " + std::to_string((int)std::ceil(earthCooldown)) + "s"
        : "TERRE: PRET");
    window.draw(cdText);

    // --- 4 éléments avec niveau (haut droite) ---
    float elemY = baseRight.y;
    struct ElemInfo { Element e; std::string name; sf::Color color; int power; };
    ElemInfo elems[4] = {
        {Element::FIRE,  "FEU",   sf::Color(255, 80, 30),   firePower},
        {Element::WATER, "EAU",   sf::Color(30, 160, 255),  waterPower},
        {Element::AIR,   "AIR",   sf::Color(180, 220, 255), airPower},
        {Element::EARTH, "TERRE", sf::Color(100, 60, 10),   earthPower},
    };

    for (int i = 0; i < 4; i++) {
        float ex = baseRight.x + i * 48.f;
        bool isActive = (currentElement == elems[i].e);

        // Fond : surligné si actif
        sf::RectangleShape bg(sf::Vector2f(44.f, 36.f));
        bg.setPosition(sf::Vector2f(ex, elemY));
        bg.setFillColor(isActive ? sf::Color(255,255,255,60) : sf::Color(0,0,0,80));
        bg.setOutlineColor(isActive ? sf::Color::Black : sf::Color(120,120,120,120));
        bg.setOutlineThickness(isActive ? 2.f : 1.f);
        window.draw(bg);

        // Carré de couleur
        sf::RectangleShape dot(sf::Vector2f(10.f, 10.f));
        dot.setPosition(sf::Vector2f(ex + 4.f, elemY + 4.f));
        dot.setFillColor(elems[i].color);
        window.draw(dot);

        // Nom + niveau
        sf::Text name(font);
        name.setCharacterSize(9);
        name.setFillColor(sf::Color::Black);
        name.setPosition(sf::Vector2f(ex + 4.f, elemY + 17.f));
        name.setString(elems[i].name);
        window.draw(name);

        sf::Text lvl(font);
        lvl.setCharacterSize(9);
        lvl.setFillColor(isActive ? sf::Color::Yellow : sf::Color(180,180,180));
        lvl.setPosition(sf::Vector2f(ex + 28.f, elemY + 4.f));
        lvl.setString("Lv" + std::to_string(elems[i].power));
        window.draw(lvl);

        // Touche de raccourci
        sf::Text key(font);
        key.setCharacterSize(8);
        key.setFillColor(sf::Color(200,200,200,180));
        key.setPosition(sf::Vector2f(ex + 28.f, elemY + 22.f));
        key.setString("[" + std::to_string(i+1) + "]");
        window.draw(key);
    }

}

void Player::switchElement(Element e) {
    currentElement = e;
}

Element Player::getCurrentElement() const {
    return currentElement;
}

void Player::upgradeElement(Element e) {
    if (e == Element::FIRE)  firePower++;
    if (e == Element::WATER) waterPower++;
    if (e == Element::AIR)   airPower++;
    if (e == Element::EARTH) earthPower++;
}

void Player::usePower(std::vector<std::unique_ptr<Entity>>& entities, World& world) {
    powerFlashTimer = 0.3f;

    if (currentElement == Element::FIRE) {
        for (auto& ent : entities) {
            Enemy* enemy = dynamic_cast<Enemy*>(ent.get());
            if (!enemy) continue;
            float dx = enemy->getPosition().x - position.x;
            float dy = enemy->getPosition().y - position.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            if (distance < 100.f * firePower)
                enemy->takeDamage(20.f * firePower);
        }
    }
    else if (currentElement == Element::WATER) {
        hp += 20.f * waterPower;
        if (hp > maxHp) hp = maxHp;
    }
    else if (currentElement == Element::AIR) {
        for (auto& ent : entities) {
            Enemy* enemy = dynamic_cast<Enemy*>(ent.get());
            if (!enemy) continue;
            float dx = enemy->getPosition().x - position.x;
            float dy = enemy->getPosition().y - position.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            if (distance < 150.f && distance > 0) {
                enemy->setPosition(sf::Vector2f(
                    enemy->getPosition().x + (dx/distance) * 100.f * airPower,
                    enemy->getPosition().y + (dy/distance) * 100.f * airPower
                ));
            }
        }
    }
    else if (currentElement == Element::EARTH && earthCooldown <= 0.f) {
        float gridStepX = 26.0f;
        float gridStepY = 13.0f;
        float cartX = (position.x / gridStepX + position.y / gridStepY) / 2.0f;
        float cartY = (position.y / gridStepY - position.x / gridStepX) / 2.0f;
        int tileX = static_cast<int>(std::floor(cartX));
        int tileY = static_cast<int>(std::floor(cartY));
        int range = 2 + earthPower;
        for (int dx = -range; dx <= range; dx++)
            for (int dy = -range; dy <= range; dy++)
                if (!world.isLand(tileX + dx, tileY + dy))
                    world.setWorldTile(tileX + dx, tileY + dy, 6);
        earthCooldown = 10.f;
    }
}
