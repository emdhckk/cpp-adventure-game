#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "Entity.h"
#include "Neutral.h"
#include "QuestManager.h"

void drawCompass(
    sf::RenderWindow& window,
    sf::View& view,
    sf::Font& font,
    sf::Vector2f playerPos,
    std::vector<std::unique_ptr<Entity>>& entities,
    QuestManager& quests
);