#include "Boat.h"
#include "World.h"
#include <cmath>
#include <sstream>

Boat::Boat() {
    position = sf::Vector2f(0.f, 0.f);
    dockPosition = sf::Vector2f(0.f, 0.f);
    active = false;
    direction = 0;
}

// Génère les 3 fragments de la quête Terre dans l'océan autour de l'île
// On cherche activement des tiles d'eau pour être sûr qu'ils ne sont pas sur la terre
void Boat::generateFragments(sf::Vector2f islandCenter, World& world) {
    if (fragmentsGenerated) return;
    fragmentsGenerated = true;
    fragments.clear();

    float gridStepX = 26.0f;
    float gridStepY = 13.0f;

    // On explore 6 directions différentes pour trouver 3 positions dans l'océan
    std::vector<sf::Vector2f> searchDirs = {
        sf::Vector2f(-1.f, -1.f),
        sf::Vector2f( 1.f,  1.f),
        sf::Vector2f(-1.f,  1.f),
        sf::Vector2f( 1.f, -1.f),
        sf::Vector2f(-1.f,  0.f),
        sf::Vector2f( 1.f,  0.f),
    };

    int found = 0;
    for (auto& dir : searchDirs) {
        if (found >= 3) break;

        // On s'éloigne progressivement jusqu'à trouver une tile d'eau (tile 0)
        for (int dist = 80; dist <= 200; dist += 10) {
            float isoX = islandCenter.x + dir.x * dist * gridStepX;
            float isoY = islandCenter.y + dir.y * dist * gridStepY;

            // Conversion de la position isométrique en coordonnées tile
            float cartX = (isoX / gridStepX + isoY / gridStepY) / 2.0f;
            float cartY = (isoY / gridStepY - isoX / gridStepX) / 2.0f;
            int tileX = static_cast<int>(std::floor(cartX));
            int tileY = static_cast<int>(std::floor(cartY));

            if (world.getTileId(tileX, tileY) == 0) {
                Fragment f;
                f.worldPos = sf::Vector2f(isoX, isoY);
                f.collected = false;
                fragments.push_back(f);
                found++;
                break;
            }
        }
    }

    // Fallback : si on n'a pas trouvé 3 positions dans l'eau, on en place très loin (120° d'écart)
    while ((int)fragments.size() < 3) {
        Fragment f;
        float angle = fragments.size() * 2.094f;
        f.worldPos = sf::Vector2f(
            islandCenter.x + std::cos(angle) * 5000.f,
            islandCenter.y + std::sin(angle) * 5000.f
        );
        f.collected = false;
        fragments.push_back(f);
    }
}

// Le joueur monte dans le bateau depuis le quai
void Boat::embark(sf::Vector2f playerPos) {
    active = true;
    position = dockPosition;
}

// Le joueur descend du bateau — le quai se déplace à la nouvelle position pour la prochaine fois
sf::Vector2f Boat::disembark() {
    active = false;
    dockPosition = position;
    return position;
}

void Boat::update(float dt, World& world) {
    if (!active) return;

    sf::Vector2f move(0.f, 0.f);
    bool moving = false;

    // Déplacement avec ZQSD
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) { move.y -= speed * dt; direction = 1; moving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { move.y += speed * dt; direction = 0; moving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) { move.x -= speed * dt; direction = 2; moving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { move.x += speed * dt; direction = 3; moving = true; }

    sf::Vector2f newPos = position + move;

    // Conversion en coordonnées tile pour vérifier si on peut naviguer
    float gridStepX = 26.0f;
    float gridStepY = 13.0f;
    float cartX = (newPos.x / gridStepX + newPos.y / gridStepY) / 2.0f;
    float cartY = (newPos.y / gridStepY - newPos.x / gridStepX) / 2.0f;
    int tileX = static_cast<int>(std::floor(cartX));
    int tileY = static_cast<int>(std::floor(cartY));
    int tileId = world.getTileId(tileX, tileY);

    // Le bateau peut naviguer sur l'eau et la plage
    if (tileId == 0 || tileId == 2) {
        position = newPos;
    }

    if (moving) {
        animTimer += dt;
        if (animTimer >= 0.2f) {
            animTimer = 0.f;
            currentFrame = (currentFrame + 1) % 2;
        }
    }
}

void Boat::draw(sf::RenderWindow& window) {
    // Le bateau s'affiche à sa position active ou à quai
    sf::Vector2f drawPos = active ? position : dockPosition;

    if (spriteTexture && spriteTexture->getSize().x > 0) {
        // On choisit la bonne colonne dans la spritesheet selon la direction
        int spriteCol = 0;
        if (direction == 0) spriteCol = 1; // sud
        if (direction == 1) spriteCol = 0; // nord
        if (direction == 2) spriteCol = 2; // ouest
        if (direction == 3) spriteCol = 3; // est

        sf::Sprite sprite(*spriteTexture);
        sprite.setTextureRect(sf::IntRect(
            sf::Vector2i(spriteCol * 96, 0),
            sf::Vector2i(96, 64)
        ));
        sprite.setScale(sf::Vector2f(0.7f, 0.7f));
        sprite.setOrigin(sf::Vector2f(48.f, 48.f));
        sprite.setPosition(drawPos);
        window.draw(sprite);
    } else {
        // Fallback si la texture n'est pas chargée
        sf::ConvexShape hull;
        hull.setPointCount(4);
        hull.setPoint(0, sf::Vector2f(0.f, -12.f));
        hull.setPoint(1, sf::Vector2f(16.f, 0.f));
        hull.setPoint(2, sf::Vector2f(0.f, 12.f));
        hull.setPoint(3, sf::Vector2f(-16.f, 0.f));
        hull.setPosition(drawPos);
        hull.setFillColor(sf::Color(139, 90, 43));
        window.draw(hull);
    }

    if (!active) return;

    // Affiche les fragments non encore collectés dans l'océan
    for (auto& frag : fragments) {
        if (frag.collected) continue;
        sf::ConvexShape gem;
        gem.setPointCount(4);
        gem.setPoint(0, sf::Vector2f(0.f,  -10.f));
        gem.setPoint(1, sf::Vector2f(8.f,   0.f));
        gem.setPoint(2, sf::Vector2f(0.f,   10.f));
        gem.setPoint(3, sf::Vector2f(-8.f,  0.f));
        gem.setPosition(frag.worldPos);
        gem.setFillColor(sf::Color(160, 110, 40));
        gem.setOutlineColor(sf::Color(255, 220, 80));
        gem.setOutlineThickness(2.f);
        window.draw(gem);
    }
}

// Affiche la carte au trésor en bas à gauche quand on est dans le bateau
void Boat::drawMap(sf::RenderWindow& window, sf::View& view, sf::Font& font, sf::Vector2f playerPos) {
    sf::Vector2f base = view.getCenter() - view.getSize() / 2.f;
    float mapX = base.x + 10.f;
    float mapY = base.y + view.getSize().y - 150.f;
    float mapW = 130.f;
    float mapH = 130.f;

    // Fond parchemin
    sf::RectangleShape mapBg(sf::Vector2f(mapW, mapH));
    mapBg.setPosition(sf::Vector2f(mapX, mapY));
    mapBg.setFillColor(sf::Color(200, 170, 100, 220));
    mapBg.setOutlineColor(sf::Color(100, 70, 30));
    mapBg.setOutlineThickness(3.f);
    window.draw(mapBg);

    sf::Text title(font);
    title.setCharacterSize(8);
    title.setFillColor(sf::Color(80, 40, 10));
    title.setString("~ CARTE AU TRESOR ~");
    title.setPosition(sf::Vector2f(mapX + 6.f, mapY + 4.f));
    window.draw(title);

    // Calcul de la zone visible sur la carte (englobe le bateau et tous les fragments)
    float minX = playerPos.x - 400.f, maxX = playerPos.x + 400.f;
    float minY = playerPos.y - 400.f, maxY = playerPos.y + 400.f;
    for (auto& f : fragments) {
        minX = std::min(minX, f.worldPos.x - 100.f);
        maxX = std::max(maxX, f.worldPos.x + 100.f);
        minY = std::min(minY, f.worldPos.y - 100.f);
        maxY = std::max(maxY, f.worldPos.y + 100.f);
    }
    float rangeX = maxX - minX;
    float rangeY = maxY - minY;

    // Conversion d'une position monde en position sur la minimap
    auto worldToMap = [&](sf::Vector2f wp) -> sf::Vector2f {
        float mx = mapX + 8.f + (wp.x - minX) / rangeX * (mapW - 16.f);
        float my = mapY + 18.f + (wp.y - minY) / rangeY * (mapH - 26.f);
        return sf::Vector2f(mx, my);
    };

    // Point bleu = position du bateau
    sf::Vector2f playerMapPos = worldToMap(playerPos);
    sf::CircleShape playerDot(4.f);
    playerDot.setOrigin(sf::Vector2f(4.f, 4.f));
    playerDot.setPosition(playerMapPos);
    playerDot.setFillColor(sf::Color(30, 100, 255));
    playerDot.setOutlineColor(sf::Color::White);
    playerDot.setOutlineThickness(1.f);
    window.draw(playerDot);

    // X rouge = fragment à collecter, v vert = fragment collecté
    for (int i = 0; i < (int)fragments.size(); i++) {
        sf::Vector2f fMapPos = worldToMap(fragments[i].worldPos);
        sf::Text cross(font);
        cross.setCharacterSize(14);
        cross.setFillColor(fragments[i].collected ? sf::Color(60, 200, 60) : sf::Color(180, 20, 20));
        cross.setString(fragments[i].collected ? "v" : "X");
        cross.setPosition(sf::Vector2f(fMapPos.x - 5.f, fMapPos.y - 8.f));
        window.draw(cross);
    }

    sf::Text legend(font);
    legend.setCharacterSize(8);
    legend.setFillColor(sf::Color(80, 40, 10));
    legend.setString(std::to_string(getCollectedCount()) + "/" + std::to_string((int)fragments.size()) + " fragments  [B]=quitter");
    legend.setPosition(sf::Vector2f(mapX + 4.f, mapY + mapH - 13.f));
    window.draw(legend);
}

// Vérifie si le bateau est sur un fragment et le marque comme collecté
int Boat::checkFragmentCollection() {
    if (!active) return 0;
    int collected = 0;
    for (auto& frag : fragments) {
        if (frag.collected) continue;
        float dx = position.x - frag.worldPos.x;
        float dy = position.y - frag.worldPos.y;
        if (std::sqrt(dx*dx + dy*dy) < 40.f) {
            frag.collected = true;
            collected++;
        }
    }
    return collected;
}

int Boat::getCollectedCount() const {
    int count = 0;
    for (auto& f : fragments) if (f.collected) count++;
    return count;
}

bool Boat::allCollected() const {
    for (auto& f : fragments) if (!f.collected) return false;
    return !fragments.empty();
}

std::vector<Fragment>& Boat::getFragments() { return fragments; }

sf::Vector2f Boat::getDockPosition() const {
    return dockPosition;
}

// Réinitialise les fragments (utilisé lors du chargement d'une sauvegarde)
void Boat::clearFragments() {
    fragments.clear();
    fragmentsGenerated = false;
}

// Ajoute un fragment manuellement (utilisé lors du chargement d'une sauvegarde)
void Boat::addFragment(sf::Vector2f pos, bool collected) {
    Fragment f;
    f.worldPos = pos;
    f.collected = collected;
    fragments.push_back(f);
    fragmentsGenerated = true;
}