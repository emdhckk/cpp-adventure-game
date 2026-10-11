#pragma once
#include <string>
#include <SFML/Graphics.hpp>

// État d'une quête
enum class QuestState {
    LOCKED,      // pas encore débloquée (le joueur n'a pas parlé au Neutral)
    ACTIVE,      // en cours
    COMPLETED    // terminée, relique disponible
};

// Une quête par île élémentale
struct Quest {
    QuestState state = QuestState::LOCKED;
    int progress = 0;    // progression actuelle
    int goal = 0;        // objectif à atteindre
    std::string dialogueLocked;   // texte avant d'accepter
    std::string dialogueActive;   // texte pendant la quête
    std::string dialogueDone;     // texte une fois terminée
};

class QuestManager {
private:
    Quest fireQuest;   // Île Feu  : battre le gardien
    Quest waterQuest;  // Île Eau  : éteindre des volcans
    Quest airQuest;    // Île Air  : défendre le temple
    Quest earthQuest;  // Île Terre: trouver les fragments sous l'eau

    // Boîte de dialogue
    bool showDialogue = false;
    std::string currentDialogue;
    float dialogueTimer = 0.f;
    int currentIslandType = -1; // type de tile de l'île active

    // Temple Air : position du point à défendre
    sf::Vector2f templePos;
    float templeDefenseTimer = 0.f;
    bool templeDefenseActive = false;

public:
    QuestManager();

    // Appelé quand le joueur appuie sur E près d'un Neutral
    void interact(int islandTileId, sf::Vector2f neutralPos);

    // Mises à jour de progression
    void onFireDamageDealt(float damage);    // dégâts infligés avec le pouvoir Feu
    void onVolcanoExtinguished();            // volcan éteint avec Eau
    void onEnemyNearTemple(sf::Vector2f enemyPos, float dt); // ennemi près du temple
    void onBridgeBuilt(int tileX, int tileY); // pont créé → fragment possible

    // Vérifie si la relique d'une île est débloquée
    bool isRelicUnlocked(int islandTileId) const;

    // Getters pour le HUD
    Quest& getFireQuest()  { return fireQuest; }
    Quest& getWaterQuest() { return waterQuest; }
    Quest& getAirQuest()   { return airQuest; }
    Quest& getEarthQuest() { return earthQuest; }

    //Setter
    void setQuestData(Quest& q, int state, int progress);

    bool isDialogueVisible() const { return showDialogue; }
    std::string getDialogue() const { return currentDialogue; }
    void closeDialogue() { showDialogue = false; dialogueTimer = 0.f; }
    sf::Vector2f getTemplePos() const { return templePos; }
    void setTemplePos(sf::Vector2f pos) { templePos = pos; }

    void update(float dt);
    void drawDialogue(sf::RenderWindow& window, sf::View& view, sf::Font& font);
    void drawHUDQuests(sf::RenderWindow& window, sf::View& view, sf::Font& font);


}; 