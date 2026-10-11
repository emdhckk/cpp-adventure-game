#include "Compass.h"

// Dessine la boussole en bas à droite - pointe vers les Neutrals
void drawCompass(sf::RenderWindow& window, sf::View& view, sf::Font& font,
                 sf::Vector2f playerPos, std::vector<std::unique_ptr<Entity>>& entities, QuestManager& quests) {
    sf::Vector2f base = view.getCenter() + view.getSize() / 2.f;
    float cx = base.x - 70.f;
    float cy = base.y - 70.f;
    float radius = 40.f;

    // Fond
    sf::CircleShape bg(radius + 8.f);
    bg.setOrigin(sf::Vector2f(radius + 8.f, radius + 8.f));
    bg.setPosition(sf::Vector2f(cx, cy));
    bg.setFillColor(sf::Color(10, 10, 30, 180));
    bg.setOutlineColor(sf::Color(200, 180, 100));
    bg.setOutlineThickness(2.f);
    window.draw(bg);

    // Titre
    sf::Text label(font);
    label.setCharacterSize(8);
    label.setFillColor(sf::Color(200, 200, 200));
    label.setString("GUIDES");
    auto lb = label.getLocalBounds();
    label.setOrigin(sf::Vector2f(lb.size.x/2.f, 0.f));
    label.setPosition(sf::Vector2f(cx, cy - radius - 6.f));
    window.draw(label);

    // Couleurs par île
    struct IslandInfo { sf::Color color; std::string name; QuestState state; };
    IslandInfo islands[4] = {
        {sf::Color(255, 80, 30),   "FEU",   quests.getFireQuest().state},
        {sf::Color(30, 160, 255),  "EAU",   quests.getWaterQuest().state},
        {sf::Color(200, 235, 255), "AIR",   quests.getAirQuest().state},
        {sf::Color(160, 110, 40),  "TERRE", quests.getEarthQuest().state},
    };

    // Collecte les positions des Neutrals avec leur type d'île
    for (auto& ent : entities) {
        Neutral* neutral = dynamic_cast<Neutral*>(ent.get());
        if (!neutral) continue;

        int type = neutral->getIslandType();
        if (type == -1) continue;

        // Trouve les infos de l'île correspondante
        int infoIdx = -1;
        if (type == 5) infoIdx = 0; // feu
        if (type == 4) infoIdx = 1; // eau
        if (type == 3) infoIdx = 2; // air
        if (type == 6) infoIdx = 3; // terre
        if (infoIdx == -1) continue;

        IslandInfo& info = islands[infoIdx];
        if (info.state == QuestState::COMPLETED) continue;

        float dx = neutral->getPosition().x - playerPos.x;
        float dy = neutral->getPosition().y - playerPos.y;
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist < 1.f) continue;

        float nx = dx / dist;
        float ny = dy / dist;

        float arrowX = cx + nx * radius;
        float arrowY = cy + ny * radius;

        sf::Color col = info.color;

        sf::Vertex line[2];
        line[0].position = sf::Vector2f(cx, cy);
        line[1].position = sf::Vector2f(arrowX, arrowY);
        line[0].color = sf::Color(col.r, col.g, col.b, 100);
        line[1].color = col;
        window.draw(line, 2, sf::PrimitiveType::Lines);

        sf::CircleShape tip(5.f);
        tip.setOrigin(sf::Vector2f(5.f, 5.f));
        tip.setPosition(sf::Vector2f(arrowX, arrowY));
        tip.setFillColor(col);
        window.draw(tip);

        int distTiles = (int)(dist / 26.f);
        sf::Text txt(font);
        txt.setCharacterSize(8);
        txt.setFillColor(col);
        txt.setString(info.name + " " + std::to_string(distTiles) + "t");
        txt.setPosition(sf::Vector2f(arrowX + 6.f, arrowY - 6.f));
        window.draw(txt);
    }

    // Point central (joueur)
    sf::CircleShape center(4.f);
    center.setOrigin(sf::Vector2f(4.f, 4.f));
    center.setPosition(sf::Vector2f(cx, cy));
    center.setFillColor(sf::Color::White);
    window.draw(center);
}