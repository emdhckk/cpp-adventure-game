#pragma once

#include <SFML/Graphics.hpp>
#include "ScoreSave.h"

bool showGameOverScreen(
    sf::RenderWindow& window,
    sf::Font& font,
    int kills,
    float gameTime,
    bool allQuestsCompleted,
    int& bestKills,
    float& bestTime
);