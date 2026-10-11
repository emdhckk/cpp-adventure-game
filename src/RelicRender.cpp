#include "RelicRender.h"

void drawFireRelic(sf::RenderWindow& window, sf::Vector2f pos) {
    sf::ConvexShape crystal;
    crystal.setPointCount(6);
    crystal.setPoint(0, sf::Vector2f(0.f,  -14.f));
    crystal.setPoint(1, sf::Vector2f(5.f,  -6.f));
    crystal.setPoint(2, sf::Vector2f(4.f,   4.f));
    crystal.setPoint(3, sf::Vector2f(0.f,   7.f));
    crystal.setPoint(4, sf::Vector2f(-4.f,  4.f));
    crystal.setPoint(5, sf::Vector2f(-5.f, -6.f));
    crystal.setOrigin(sf::Vector2f(0.f, 0.f));
    crystal.setPosition(pos);
    crystal.setFillColor(sf::Color(220, 60, 10));
    crystal.setOutlineColor(sf::Color(255, 160, 40));
    crystal.setOutlineThickness(1.5f);
    window.draw(crystal);
    sf::ConvexShape shine;
    shine.setPointCount(3);
    shine.setPoint(0, sf::Vector2f(-1.f, -11.f));
    shine.setPoint(1, sf::Vector2f(-4.f,  -4.f));
    shine.setPoint(2, sf::Vector2f(-1.f,  -5.f));
    shine.setPosition(pos);
    shine.setFillColor(sf::Color(255, 200, 100, 180));
    window.draw(shine);
    for (int i = -1; i <= 1; i += 2) {
        sf::ConvexShape flame;
        flame.setPointCount(3);
        flame.setPoint(0, sf::Vector2f(i * 3.f, -14.f));
        flame.setPoint(1, sf::Vector2f(i * 6.f, -20.f));
        flame.setPoint(2, sf::Vector2f(i * 1.f, -18.f));
        flame.setPosition(pos);
        flame.setFillColor(sf::Color(255, 120, 0, 200));
        window.draw(flame);
    }
}

void drawWaterRelic(sf::RenderWindow& window, sf::Vector2f pos) {
    sf::CircleShape drop(6.f);
    drop.setOrigin(sf::Vector2f(6.f, 6.f));
    drop.setPosition(sf::Vector2f(pos.x, pos.y + 1.f));
    drop.setFillColor(sf::Color(20, 130, 255));
    drop.setOutlineColor(sf::Color(140, 210, 255));
    drop.setOutlineThickness(1.5f);
    window.draw(drop);
    sf::ConvexShape tip;
    tip.setPointCount(3);
    tip.setPoint(0, sf::Vector2f(0.f,  -14.f));
    tip.setPoint(1, sf::Vector2f(-5.f, -4.f));
    tip.setPoint(2, sf::Vector2f(5.f,  -4.f));
    tip.setPosition(pos);
    tip.setFillColor(sf::Color(20, 130, 255));
    tip.setOutlineColor(sf::Color(140, 210, 255));
    tip.setOutlineThickness(1.f);
    window.draw(tip);
    sf::CircleShape shine(2.f);
    shine.setOrigin(sf::Vector2f(2.f, 2.f));
    shine.setPosition(sf::Vector2f(pos.x - 2.f, pos.y - 2.f));
    shine.setFillColor(sf::Color(200, 235, 255, 200));
    window.draw(shine);
}

void drawAirRelic(sf::RenderWindow& window, sf::Vector2f pos) {
    for (int i = 0; i < 3; i++) {
        float angle = i * 60.f * 3.14159f / 180.f;
        float dx = std::cos(angle) * 11.f;
        float dy = std::sin(angle) * 11.f;
        sf::Vertex line[2];
        line[0].position = sf::Vector2f(pos.x - dx, pos.y - dy);
        line[1].position = sf::Vector2f(pos.x + dx, pos.y + dy);
        line[0].color = sf::Color(200, 235, 255, 220);
        line[1].color = sf::Color(200, 235, 255, 220);
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
    sf::CircleShape core(4.f);
    core.setOrigin(sf::Vector2f(4.f, 4.f));
    core.setPosition(pos);
    core.setFillColor(sf::Color(230, 245, 255));
    core.setOutlineColor(sf::Color(150, 210, 255));
    core.setOutlineThickness(1.5f);
    window.draw(core);
    for (int i = 0; i < 6; i++) {
        float angle = i * 60.f * 3.14159f / 180.f;
        sf::Vector2f tip(pos.x + std::cos(angle) * 11.f, pos.y + std::sin(angle) * 11.f);
        sf::CircleShape dot(2.f);
        dot.setOrigin(sf::Vector2f(2.f, 2.f));
        dot.setPosition(tip);
        dot.setFillColor(sf::Color(200, 235, 255));
        window.draw(dot);
    }
}

void drawEarthRelic(sf::RenderWindow& window, sf::Vector2f pos) {
    sf::ConvexShape gem;
    gem.setPointCount(6);
    gem.setPoint(0, sf::Vector2f(0.f,  -13.f));
    gem.setPoint(1, sf::Vector2f(8.f,  -5.f));
    gem.setPoint(2, sf::Vector2f(8.f,   5.f));
    gem.setPoint(3, sf::Vector2f(0.f,  10.f));
    gem.setPoint(4, sf::Vector2f(-8.f,  5.f));
    gem.setPoint(5, sf::Vector2f(-8.f, -5.f));
    gem.setPosition(pos);
    gem.setFillColor(sf::Color(100, 65, 15));
    gem.setOutlineColor(sf::Color(210, 160, 40));
    gem.setOutlineThickness(2.f);
    window.draw(gem);
    sf::ConvexShape face;
    face.setPointCount(4);
    face.setPoint(0, sf::Vector2f(0.f,  -8.f));
    face.setPoint(1, sf::Vector2f(5.f,  -2.f));
    face.setPoint(2, sf::Vector2f(0.f,   6.f));
    face.setPoint(3, sf::Vector2f(-5.f, -2.f));
    face.setPosition(pos);
    face.setFillColor(sf::Color(160, 110, 40, 200));
    window.draw(face);
    sf::ConvexShape shine;
    shine.setPointCount(3);
    shine.setPoint(0, sf::Vector2f(-1.f, -10.f));
    shine.setPoint(1, sf::Vector2f(-6.f,  -3.f));
    shine.setPoint(2, sf::Vector2f(-2.f,  -4.f));
    shine.setPosition(pos);
    shine.setFillColor(sf::Color(255, 220, 100, 160));
    window.draw(shine);
}

void drawRelics(sf::RenderWindow& window, World& world, const sf::View& view) {
    float gridStepX = 26.0f;
    float gridStepY = 13.0f;
    for (auto& lc : world.getLoadedChunks()) {
        for (int y = 0; y < Chunk::chunkSize; y++) {
            for (int x = 0; x < Chunk::chunkSize; x++) {
                int id = lc.chunk.getTile(x, y).getTile_id();
                if (id < 7 || id > 10) continue;
                float chunkScreenX = (lc.x_relative - lc.y_relative) * (Chunk::chunkSize * gridStepX);
                float chunkScreenY = (lc.x_relative + lc.y_relative) * (Chunk::chunkSize * gridStepY);
                float tileX = chunkScreenX + (x - y) * gridStepX + 26.f;
                float tileY = chunkScreenY + (x + y) * gridStepY + 13.f;
                sf::Vector2f pos(tileX, tileY);
                if (id == 7)  drawAirRelic(window, pos);
                if (id == 8)  drawWaterRelic(window, pos);
                if (id == 9)  drawFireRelic(window, pos);
                if (id == 10) drawEarthRelic(window, pos);
            }
        }
    }
}
