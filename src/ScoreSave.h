#pragma once
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

void loadHighScore(int& bestKills, float& bestTime);
void saveHighScore(int bestKills, float bestTime);