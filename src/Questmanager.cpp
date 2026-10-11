#include "QuestManager.h"
#include <cmath>
#include <sstream>

QuestManager::QuestManager() {
    // Île Feu : infliger 200 dégâts au gardien avec le pouvoir Feu
    fireQuest.goal = 200;
    fireQuest.dialogueLocked = "Je suis le gardien de cette ile.\nProuve ta maitrise du Feu en m'infligeant 200 degats\navec ton pouvoir. Alors seulement tu meriteras la relique.";
    fireQuest.dialogueActive = "Continue ! Tu m'as inflige {p}/{g} degats de Feu.";
    fireQuest.dialogueDone   = "Impressionnant. Tu as prouve ta valeur.\nLa relique du Feu t'appartient.";

    // Île Eau : éteindre 5 volcans avec le pouvoir Eau
    waterQuest.goal = 5;
    waterQuest.dialogueLocked = "Ces volcans menacent notre ile !\nUtilise ton pouvoir de l'eau sur les tiles de lave\npour en eteindre 5. Marche dessus en mode eau.";
    waterQuest.dialogueActive = "Continue ! Tu as eteint {p}/{g} volcans.";
    waterQuest.dialogueDone   = "Les volcans sont calmes. Merci.\nLa relique de l'Eau est a toi.";

    // Île Air : défendre le temple 30 secondes en repoussant les ennemis
    airQuest.goal = 30;
    airQuest.dialogueLocked = "Notre temple sacre est menacee par les ennemis !\nProtege-le pendant 30 secondes en repoussant\nles ennemis avec ton pouvoir de l'air.";
    airQuest.dialogueActive = "Tiens bon ! {p}/{g} secondes ecoulees.";
    airQuest.dialogueDone   = "Le temple est sauve ! Tu es un vrai maitre de l'Air.\nPrends la relique, tu la merites.";

    // Île Terre : trouver 3 fragments cachés dans l'océan avec le bateau
    earthQuest.goal = 3;
    earthQuest.dialogueLocked = "La relique a ete brisee en 3 fragments\njetes dans l'ocean par une tempete.\nMonte dans un bateau [B] pres de l'eau\net trouve-les grace a la carte !";
    earthQuest.dialogueActive = "Tu as recupere {p}/{g} fragments.";
    earthQuest.dialogueDone   = "Tous les fragments reunis ! La relique de la Terre\nest reconstituee et t'appartient.";
}

// Déclenché quand le joueur appuie sur E près d'un Neutral
// Gère l'ouverture de la quête ou l'affichage de la progression
void QuestManager::interact(int islandTileId, sf::Vector2f neutralPos) {
    Quest* q = nullptr;
    if (islandTileId == 5) q = &fireQuest;
    if (islandTileId == 4) q = &waterQuest;
    if (islandTileId == 3) q = &airQuest;
    if (islandTileId == 6) q = &earthQuest;
    if (!q) return;

    currentIslandType = islandTileId;
    showDialogue = true;
    dialogueTimer = 12.f;

    if (q->state == QuestState::LOCKED) {
        // Première interaction : on débloque la quête et on affiche le dialogue d'introduction
        q->state = QuestState::ACTIVE;
        currentDialogue = q->dialogueLocked;
        // Pour l'île Air, on mémorise la position du temple à défendre
        if (islandTileId == 3) {
            templePos = neutralPos;
            templeDefenseActive = false;
            templeDefenseTimer = 0.f;
        }
    } else if (q->state == QuestState::ACTIVE) {
        // Quête Terre : on valide si tous les fragments sont collectés
        if (islandTileId == 6 && q->progress >= q->goal) {
            q->state = QuestState::COMPLETED;
            currentDialogue = q->dialogueDone;
        } else {
            // Sinon on affiche la progression en remplaçant {p} et {g} par les vraies valeurs
            std::string msg = q->dialogueActive;
            std::string ps = std::to_string(q->progress);
            std::string gs = std::to_string(q->goal);
            size_t pp = msg.find("{p}");
            if (pp != std::string::npos) msg.replace(pp, 3, ps);
            size_t gp = msg.find("{g}");
            if (gp != std::string::npos) msg.replace(gp, 3, gs);
            currentDialogue = msg;
        }
    } else if (q->state == QuestState::COMPLETED) {
        currentDialogue = q->dialogueDone;
    }
}

// Appelé à chaque fois que le joueur utilise le pouvoir Feu près du gardien
void QuestManager::onFireDamageDealt(float damage) {
    if (fireQuest.state != QuestState::ACTIVE) return;
    fireQuest.progress += (int)damage;
    if (fireQuest.progress >= fireQuest.goal)
        fireQuest.state = QuestState::COMPLETED;
}

// Appelé quand le joueur éteint un volcan avec le pouvoir Eau
void QuestManager::onVolcanoExtinguished() {
    if (waterQuest.state != QuestState::ACTIVE) return;
    waterQuest.progress++;
    if (waterQuest.progress >= waterQuest.goal)
        waterQuest.state = QuestState::COMPLETED;
}

// Appelé chaque frame pour chaque ennemi — reset le timer si un ennemi touche le temple
void QuestManager::onEnemyNearTemple(sf::Vector2f enemyPos, float dt) {
    if (airQuest.state != QuestState::ACTIVE) return;
    float dx = enemyPos.x - templePos.x;
    float dy = enemyPos.y - templePos.y;
    float dist = std::sqrt(dx*dx + dy*dy);
    if (dist < 80.f) {
        templeDefenseTimer = 0.f;
        templeDefenseActive = false;
        return;
    }
}

// Appelé quand le joueur crée un pont — peut révéler un fragment (1 chance sur 5)
void QuestManager::onBridgeBuilt(int tileX, int tileY) {
    if (earthQuest.state != QuestState::ACTIVE) return;
    unsigned int h = (unsigned int)(tileX * 73856093 ^ tileY * 19349663);
    if (h % 5 == 0) {
        earthQuest.progress++;
        showDialogue = true;
        dialogueTimer = 3.f;
        currentDialogue = "Fragment trouve ! " + std::to_string(earthQuest.progress) + "/" + std::to_string(earthQuest.goal);
        if (earthQuest.progress >= earthQuest.goal)
            earthQuest.state = QuestState::COMPLETED;
    }
}

bool QuestManager::isRelicUnlocked(int islandTileId) const {
    if (islandTileId == 5) return fireQuest.state  == QuestState::COMPLETED;
    if (islandTileId == 4) return waterQuest.state == QuestState::COMPLETED;
    if (islandTileId == 3) return airQuest.state   == QuestState::COMPLETED;
    if (islandTileId == 6) return earthQuest.state == QuestState::COMPLETED;
    return false;
}

void QuestManager::update(float dt) {
    // Ferme le dialogue quand le timer expire
    if (showDialogue) {
        dialogueTimer -= dt;
        if (dialogueTimer <= 0.f) showDialogue = false;
    }

    // Quête Air : le timer de défense avance chaque frame tant qu'aucun ennemi n'est proche
    if (airQuest.state == QuestState::ACTIVE) {
        templeDefenseTimer += dt;
        airQuest.progress = (int)templeDefenseTimer;
        if (templeDefenseTimer >= (float)airQuest.goal)
            airQuest.state = QuestState::COMPLETED;
    }
}

void QuestManager::drawDialogue(sf::RenderWindow& window, sf::View& view, sf::Font& font) {
    if (!showDialogue) return;

    sf::Vector2f base = view.getCenter() - view.getSize() / 2.f;

    // Messages courts (ex: "Fragment trouvé") → petite boîte centrée sans portrait
    bool isShortMessage = (currentDialogue.find('\n') == std::string::npos && currentDialogue.size() < 40);
    float w = isShortMessage ? 220.f : view.getSize().x - 40.f;
    float h = isShortMessage ? 36.f : 120.f;
    float x = isShortMessage ? (base.x + view.getSize().x/2.f - w/2.f) : base.x + 20.f;
    float y = base.y + view.getSize().y - h - 20.f;

    sf::RectangleShape box(sf::Vector2f(w, h));
    box.setPosition(sf::Vector2f(x, y));
    box.setFillColor(sf::Color(10, 10, 30, 220));
    box.setOutlineColor(sf::Color(200, 180, 100));
    box.setOutlineThickness(2.f);
    window.draw(box);

    // Portrait du Neutral uniquement pour les longs dialogues
    float textX = isShortMessage ? x + 10.f : x + 70.f;
    float textY = isShortMessage ? y + 8.f : y + 12.f;
    if (!isShortMessage) {
        sf::CircleShape portrait(24.f);
        portrait.setOrigin(sf::Vector2f(24.f, 24.f));
        portrait.setPosition(sf::Vector2f(x + 34.f, y + h / 2.f));
        portrait.setFillColor(sf::Color(255, 140, 0));
        portrait.setOutlineColor(sf::Color(200, 100, 0));
        portrait.setOutlineThickness(2.f);
        window.draw(portrait);
    }

    // Affichage du texte ligne par ligne
    std::string line;
    std::istringstream stream(currentDialogue);
    int lineNum = 0;
    while (std::getline(stream, line)) {
        sf::Text txt(font);
        txt.setCharacterSize(11);
        txt.setFillColor(sf::Color::White);
        txt.setPosition(sf::Vector2f(textX, textY + lineNum * 16.f));
        txt.setString(line);
        window.draw(txt);
        lineNum++;
    }

    sf::Text hint(font);
    hint.setCharacterSize(9);
    hint.setFillColor(sf::Color(200, 200, 100, 180));
    hint.setPosition(sf::Vector2f(x + w - 120.f, y + h - 16.f));
    hint.setString("[X] pour fermer");
    window.draw(hint);
}

// HUD des quêtes affiché en haut au centre de l'écran
void QuestManager::drawHUDQuests(sf::RenderWindow& window, sf::View& view, sf::Font& font) {
    sf::Vector2f base = view.getCenter() - view.getSize() / 2.f;
    float startX = base.x + view.getSize().x / 2.f - 100.f;
    float startY = base.y + 12.f;

    struct IslandInfo { std::string name; sf::Color color; Quest* q; };
    IslandInfo islands[4] = {
        {"FEU",   sf::Color(255, 80, 30),  &fireQuest},
        {"EAU",   sf::Color(30, 160, 255), &waterQuest},
        {"AIR",   sf::Color(200, 235, 255),&airQuest},
        {"TERRE", sf::Color(160, 110, 40), &earthQuest},
    };

    sf::Text title(font);
    title.setCharacterSize(10);
    title.setFillColor(sf::Color(200, 200, 200));
    title.setPosition(sf::Vector2f(startX, startY));
    title.setString("RELIQUES");
    window.draw(title);

    for (int i = 0; i < 4; i++) {
        float y = startY + 14.f + i * 18.f;
        Quest* q = islands[i].q;

        // Couleur du point selon l'état : gris = verrouillé, jaune = actif, vert = terminé
        sf::CircleShape dot(5.f);
        dot.setOrigin(sf::Vector2f(5.f, 5.f));
        dot.setPosition(sf::Vector2f(startX + 5.f, y + 6.f));
        if (q->state == QuestState::LOCKED)     dot.setFillColor(sf::Color(80, 80, 80));
        if (q->state == QuestState::ACTIVE)     dot.setFillColor(sf::Color(255, 220, 0));
        if (q->state == QuestState::COMPLETED)  dot.setFillColor(sf::Color(60, 220, 60));
        window.draw(dot);

        sf::Text name(font);
        name.setCharacterSize(10);
        name.setFillColor(islands[i].color);
        name.setPosition(sf::Vector2f(startX + 16.f, y));
        name.setString(islands[i].name);
        window.draw(name);

        // Affiche la progression si la quête est en cours
        if (q->state == QuestState::ACTIVE) {
            sf::Text prog(font);
            prog.setCharacterSize(9);
            prog.setFillColor(sf::Color(200, 200, 200));
            prog.setPosition(sf::Vector2f(startX + 58.f, y));
            prog.setString(std::to_string(q->progress) + "/" + std::to_string(q->goal));
            window.draw(prog);
        } else if (q->state == QuestState::COMPLETED) {
            sf::Text done(font);
            done.setCharacterSize(9);
            done.setFillColor(sf::Color(60, 220, 60));
            done.setPosition(sf::Vector2f(startX + 58.f, y));
            done.setString("COMPLETE");
            window.draw(done);
        }
    }
}

// Utilisé lors du chargement de la sauvegarde pour restaurer l'état des quêtes
void QuestManager::setQuestData(Quest& q, int state, int progress) {
    q.state = static_cast<QuestState>(state);
    q.progress = progress;
}