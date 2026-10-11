#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// Position d'un fragment de relique dans l'océan
struct Fragment {
    sf::Vector2f worldPos; // position isométrique dans le monde
    bool collected = false;
};

class Boat {
private:
    sf::Vector2f position;
    sf::Vector2f dockPosition;  // position à quai (visible même sans être dedans)
    float speed = 250.f;
    bool active = false;       // le joueur est-il dans le bateau ?
    int direction = 0;         // 0=sud, 1=nord, 2=ouest, 3=est
    float animTimer = 0.f;
    int currentFrame = 0;
    sf::Texture* spriteTexture = nullptr;

    // Fragments de la quête Terre
    std::vector<Fragment> fragments;
    bool fragmentsGenerated = false;

public:
    Boat();

    void generateFragments(sf::Vector2f islandCenter, class World& world);
    void embark(sf::Vector2f playerPos);               // monte dans le bateau
    sf::Vector2f disembark();                          // descend du bateau → retourne position

    void update(float dt, class World& world);
    void draw(sf::RenderWindow& window);
    void drawMap(sf::RenderWindow& window, sf::View& view, sf::Font& font, sf::Vector2f playerPos);

    bool isActive() const { return active; }
    sf::Vector2f getPosition() const { return active ? position : dockPosition; }
    void setSpriteTexture(sf::Texture* tex) { spriteTexture = tex; }
    void setDockPosition(sf::Vector2f pos) { dockPosition = pos; position = pos; }

    // Vérifie si le bateau est sur un fragment et le collecte
    int checkFragmentCollection(); // retourne le nombre collecté cette frame
    int getCollectedCount() const;
    int getTotalFragments() const { return (int)fragments.size(); }
    bool allCollected() const;

    //Pour la sauvegarde la progression
    std::vector<Fragment>& getFragments();
    sf::Vector2f getDockPosition() const;
    void clearFragments();
    void addFragment(sf::Vector2f pos, bool collected);
};