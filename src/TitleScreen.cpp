#include "TitleScreen.h"
#include <cmath>

static void drawTitleFire(sf::RenderWindow& window, sf::Vector2f center, float scale, float time) {
    float pulse = 1.f + 0.15f * std::sin(time * 3.f);
    sf::Color c1(255, 80, 10), c2(255, 160, 0);
    for (int f = 0; f < 3; f++) {
        float ox = (f - 1) * 18.f * scale;
        float h = (40.f + f * 8.f) * scale * pulse;
        sf::ConvexShape flame;
        flame.setPointCount(5);
        flame.setPoint(0, sf::Vector2f(ox, 0.f));
        flame.setPoint(1, sf::Vector2f(ox + 12.f * scale, -h * 0.4f));
        flame.setPoint(2, sf::Vector2f(ox + 6.f * scale, -h));
        flame.setPoint(3, sf::Vector2f(ox - 6.f * scale, -h));
        flame.setPoint(4, sf::Vector2f(ox - 12.f * scale, -h * 0.4f));
        flame.setPosition(center);
        flame.setFillColor(f == 1 ? c2 : c1);
        window.draw(flame);
    }
    sf::CircleShape base(20.f * scale);
    base.setOrigin(sf::Vector2f(20.f * scale, 20.f * scale));
    base.setPosition(sf::Vector2f(center.x, center.y - 5.f));
    base.setFillColor(sf::Color(200, 50, 0));
    window.draw(base);
}

static void drawTitleWater(sf::RenderWindow& window, sf::Vector2f center, float scale, float time) {
    float bob = std::sin(time * 2.f + 1.f) * 5.f;
    sf::Vector2f pos(center.x, center.y + bob);
    sf::ConvexShape drop;
    drop.setPointCount(8);
    float r = 28.f * scale;
    drop.setPoint(0, sf::Vector2f(0.f, -r * 1.6f));
    drop.setPoint(1, sf::Vector2f(r * 0.5f, -r * 0.8f));
    drop.setPoint(2, sf::Vector2f(r, 0.f));
    drop.setPoint(3, sf::Vector2f(r * 0.8f, r * 0.6f));
    drop.setPoint(4, sf::Vector2f(0.f, r));
    drop.setPoint(5, sf::Vector2f(-r * 0.8f, r * 0.6f));
    drop.setPoint(6, sf::Vector2f(-r, 0.f));
    drop.setPoint(7, sf::Vector2f(-r * 0.5f, -r * 0.8f));
    drop.setPosition(pos);
    drop.setFillColor(sf::Color(30, 140, 255));
    drop.setOutlineColor(sf::Color(140, 210, 255));
    drop.setOutlineThickness(2.f);
    window.draw(drop);
    sf::CircleShape shine(6.f * scale);
    shine.setOrigin(sf::Vector2f(6.f * scale, 6.f * scale));
    shine.setPosition(sf::Vector2f(pos.x - 8.f * scale, pos.y - 12.f * scale));
    shine.setFillColor(sf::Color(200, 235, 255, 160));
    window.draw(shine);
}

static void drawTitleAir(sf::RenderWindow& window, sf::Vector2f center, float scale, float time) {
    for (int ring = 0; ring < 3; ring++) {
        float r = (20.f + ring * 14.f) * scale;
        float offset = time * 1.5f + ring * 1.2f;
        int pts = 20;
        for (int i = 0; i < pts; i++) {
            float a1 = offset + i * 3.14159f * 2.f / pts;
            float a2 = offset + (i + 0.7f) * 3.14159f * 2.f / pts;
            sf::Vertex line[2];
            uint8_t alpha = (uint8_t)(120 + 80 * std::sin(i * 3.14159f / pts));
            line[0].position = sf::Vector2f(center.x + std::cos(a1) * r, center.y + std::sin(a1) * r * 0.5f);
            line[1].position = sf::Vector2f(center.x + std::cos(a2) * r, center.y + std::sin(a2) * r * 0.5f);
            line[0].color = sf::Color(200, 235, 255, alpha);
            line[1].color = sf::Color(200, 235, 255, alpha);
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }
    }
    sf::CircleShape core(10.f * scale);
    core.setOrigin(sf::Vector2f(10.f * scale, 10.f * scale));
    core.setPosition(center);
    core.setFillColor(sf::Color(220, 245, 255, 180));
    window.draw(core);
}

static void drawTitleEarth(sf::RenderWindow& window, sf::Vector2f center, float scale, float time) {
    sf::ConvexShape hex;
    hex.setPointCount(6);
    float r = 30.f * scale;
    float shake = std::sin(time * 4.f) * 1.5f;
    for (int i = 0; i < 6; i++) {
        float a = i * 3.14159f / 3.f;
        hex.setPoint(i, sf::Vector2f(std::cos(a) * r, std::sin(a) * r * 0.8f));
    }
    hex.setPosition(sf::Vector2f(center.x + shake, center.y));
    hex.setFillColor(sf::Color(100, 65, 15));
    hex.setOutlineColor(sf::Color(210, 160, 40));
    hex.setOutlineThickness(3.f);
    window.draw(hex);
    for (int i = 0; i < 3; i++) {
        float a = i * 3.14159f / 3.f + 0.5f;
        sf::Vertex line[2];
        line[0].position = sf::Vector2f(center.x + shake, center.y);
        line[1].position = sf::Vector2f(center.x + shake + std::cos(a) * r * 0.8f, center.y + std::sin(a) * r * 0.6f);
        line[0].color = sf::Color(210, 160, 40, 180);
        line[1].color = sf::Color(210, 160, 40, 60);
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
}

void showTitleScreen(sf::RenderWindow& window, sf::Font& font) {
    sf::Clock clock;
    struct Star { sf::Vector2f pos; float size; float phase; };
    std::vector<Star> stars;
    srand(42);
    for (int i = 0; i < 120; i++)
        stars.push_back({sf::Vector2f((float)(rand() % window.getSize().x), (float)(rand() % window.getSize().y)), 1.f + (rand() % 3) * 0.5f, (float)(rand() % 100) / 10.f});

    while (window.isOpen()) {
        float t = clock.getElapsedTime().asSeconds();
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) { window.close(); return; }
            if (event->is<sf::Event::KeyPressed>()) return;
            if (event->is<sf::Event::MouseButtonPressed>()) return;
        }
        sf::Vector2f center((float)window.getSize().x / 2.f, (float)window.getSize().y / 2.f);
        window.clear(sf::Color(10, 18, 40));
        for (auto& s : stars) {
            float brightness = (uint8_t)(150 + 80 * std::sin(t * 1.2f + s.phase));
            sf::CircleShape star(s.size);
            star.setOrigin(sf::Vector2f(s.size, s.size));
            star.setPosition(s.pos);
            star.setFillColor(sf::Color(255, 255, 255, (uint8_t)brightness));
            window.draw(star);
        }
        sf::CircleShape glow(80.f);
        glow.setOrigin(sf::Vector2f(80.f, 80.f));
        glow.setPosition(center);
        glow.setFillColor(sf::Color(255, 180, 50, (uint8_t)(20 + 10 * std::sin(t * 2.f))));
        window.draw(glow);
        float islandDist = 260.f;
        sf::Vector2f positions[4] = {
            sf::Vector2f(center.x - islandDist, center.y - 180.f),
            sf::Vector2f(center.x + islandDist, center.y - 180.f),
            sf::Vector2f(center.x - islandDist, center.y + 120.f),
            sf::Vector2f(center.x + islandDist, center.y + 120.f),
        };
        sf::Color islandColors[4] = {
            sf::Color(60, 20, 10), sf::Color(10, 30, 60),
            sf::Color(20, 40, 60), sf::Color(40, 25, 10)
        };
        std::string labels[4] = {"FEU [1]", "EAU [2]", "AIR [3]", "TERRE [4]"};
        sf::Color labelColors[4] = {
            sf::Color(255, 100, 30), sf::Color(80, 180, 255),
            sf::Color(200, 235, 255), sf::Color(200, 150, 40)
        };
        for (int i = 0; i < 4; i++) {
            float bob = std::sin(t * 1.5f + i * 1.2f) * 8.f;
            sf::Vector2f ipos(positions[i].x, positions[i].y + bob);
            sf::CircleShape island(48.f);
            island.setOrigin(sf::Vector2f(48.f, 48.f));
            island.setPosition(ipos);
            island.setFillColor(islandColors[i]);
            island.setOutlineColor(labelColors[i]);
            island.setOutlineThickness(2.f);
            window.draw(island);
            if (i == 0) drawTitleFire(window, ipos, 0.9f, t);
            if (i == 1) drawTitleWater(window, ipos, 0.9f, t);
            if (i == 2) drawTitleAir(window, ipos, 0.9f, t);
            if (i == 3) drawTitleEarth(window, ipos, 0.9f, t);
            sf::Text lbl(font);
            lbl.setString(labels[i]);
            lbl.setCharacterSize(14);
            lbl.setFillColor(labelColors[i]);
            auto lb = lbl.getLocalBounds();
            lbl.setOrigin(sf::Vector2f(lb.size.x / 2.f, 0.f));
            lbl.setPosition(sf::Vector2f(ipos.x, ipos.y + 54.f));
            window.draw(lbl);
        }
        sf::Text title(font);
        title.setString("ELEMENT ISLAND");
        title.setCharacterSize(64);
        title.setFillColor(sf::Color::White);
        title.setOutlineColor(sf::Color(255, 140, 0, (uint8_t)(180 + 60 * std::sin(t * 2.f))));
        title.setOutlineThickness(3.f);
        auto tb = title.getLocalBounds();
        title.setOrigin(sf::Vector2f(tb.size.x / 2.f, tb.size.y / 2.f));
        title.setPosition(sf::Vector2f(center.x, center.y - 30.f));
        window.draw(title);
        sf::Text sub(font);
        sub.setString("Maitrisez les 4 elements. Survivez.");
        sub.setCharacterSize(20);
        sub.setFillColor(sf::Color(180, 210, 255));
        auto sb = sub.getLocalBounds();
        sub.setOrigin(sf::Vector2f(sb.size.x / 2.f, 0.f));
        sub.setPosition(sf::Vector2f(center.x, center.y + 20.f));
        window.draw(sub);
        if ((int)(t * 2.f) % 2 == 0) {
            sf::Text prompt(font);
            prompt.setString("Appuyez sur une touche pour commencer");
            prompt.setCharacterSize(18);
            prompt.setFillColor(sf::Color(255, 220, 100));
            auto pb = prompt.getLocalBounds();
            prompt.setOrigin(sf::Vector2f(pb.size.x / 2.f, 0.f));
            prompt.setPosition(sf::Vector2f(center.x, center.y + 55.f));
            window.draw(prompt);
        }
        sf::Text controls(font);
        controls.setString("ZQSD : Deplacer   |   1-4 : Element   |   Espace : Pouvoir   |   E : Parler   |   B : Bateau   |   X : Fermer");
        controls.setCharacterSize(13);
        controls.setFillColor(sf::Color(100, 120, 160));
        auto cb = controls.getLocalBounds();
        controls.setOrigin(sf::Vector2f(cb.size.x / 2.f, 0.f));
        controls.setPosition(sf::Vector2f(center.x, (float)window.getSize().y - 30.f));
        window.draw(controls);
        window.display();
    }
}
