#include "ScoreSave.h"

// Charge le meilleur score depuis le fichier
void loadHighScore(int& bestKills, float& bestTime) {
    std::ifstream file("savefiles/highscore.json");
    if (!file.is_open()) return;
    nlohmann::json j;
    file >> j;
    if (j.contains("bestKills")) bestKills = j["bestKills"];
    if (j.contains("bestTime"))  bestTime  = j["bestTime"];
}

// Sauvegarde le meilleur score
void saveHighScore(int bestKills, float bestTime) {
    std::filesystem::create_directories("savefiles");
    nlohmann::json j;
    j["bestKills"] = bestKills;
    j["bestTime"]  = bestTime;
    std::ofstream file("savefiles/highscore.json");
    if (file.is_open()) file << j.dump(2);
}
