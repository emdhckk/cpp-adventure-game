#include <iostream>
#include <vector>
#include <string>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include "Chunk.h"
#include "World.h"
#include "Tile.h"
#include "TileRegistry.h"
#include "Player.h"
#include "Enemy.h"
#include "Neutral.h"
#include <cstdio>
#include <cmath>
#include "QuestManager.h"
#include "Boat.h"
#include "RelicRender.h"
#include "TileFinder.h"
#include "Compass.h"
#include "ScoreSave.h"
#include "TitleScreen.h"
#include "GameOver.h"

int main(){
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Element Island");

    sf::Texture tilemapTexture;
    if (!tilemapTexture.loadFromFile("assets/textures/spritesheet.png")) return -1;
    tilemapTexture.setSmooth(false);

    sf::Texture boatTexture;
    if (!boatTexture.loadFromFile(ASSETS_PATH + std::string("textures/boat_spritesheet.png")))
        std::cerr << "Attention: impossible de charger boat_spritesheet.png" << std::endl;
    boatTexture.setSmooth(false);

    sf::Font font;
    bool fontLoaded = font.openFromFile(ASSETS_PATH + std::string("fonts/arial.ttf"));

    int bestKills = 0;
    float bestTime = -1.f;
    loadHighScore(bestKills, bestTime);

    sf::Music music;
    if (music.openFromFile(ASSETS_PATH + std::string("island_0.ogg"))) {
        music.setLooping(true);
        music.setVolume(40.f);
        music.play();
    }

    showTitleScreen(window, font);

    while (window.isOpen()) {
    

        World world;
        Player player(sf::Vector2f(-600.f, 50.f));
        QuestManager quests;
        Boat boat;
        world.loadSave("savefiles/savefile.json", player, quests, boat);
        NoiseGen noise(666);

        sf::Texture playerTexture;
        if (!playerTexture.loadFromFile(ASSETS_PATH + std::string("textures/player_spritesheet.png")))
            std::cerr << "Attention: impossible de charger player_spritesheet.png" << std::endl;
        playerTexture.setSmooth(false);

        sf::Texture enemyTexture;
        if (!enemyTexture.loadFromFile(ASSETS_PATH + std::string("textures/enemy_spritesheet.png")))
            std::cerr << "Attention: impossible de charger enemy_spritesheet.png" << std::endl;
        enemyTexture.setSmooth(false);

        sf::Texture neutralTexture;
        if (!neutralTexture.loadFromFile(ASSETS_PATH + std::string("textures/neutral_spritesheet.png")))
            std::cerr << "Attention: impossible de charger neutral_spritesheet.png" << std::endl;
        neutralTexture.setSmooth(false);

        player.setSpriteTexture(&playerTexture);

        std::vector<std::unique_ptr<Entity>> entities;

        for (int i = 0; i < 4; i++) {
            float angle = i * 90.f * 3.14159f / 180.f;
            sf::Vector2f spawnPos = player.getPosition() + sf::Vector2f(
                std::cos(angle) * 400.f, std::sin(angle) * 400.f);
            float hp = 80.f + rand() % 60; // 80 à 140 HP
            auto e = std::make_unique<Enemy>(spawnPos, 8.f, 350.f, hp);
            e->setSpriteTexture(&enemyTexture);
            entities.push_back(std::move(e));
        }
        sf::View view(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(window.getSize())));
        view.zoom(0.5f);
        window.setView(view);

        // Charge quelques chunks autour du spawn pour détecter les îles
        world.updateChunks(view, noise);

        // Neutrals placés dynamiquement dans la boucle quand les îles sont chargées
        int islandTiles[4] = {5, 4, 3, 6};
        bool neutralPlaced[4] = {false, false, false, false};
        // Place le Neutral de l'île de Terre tout de suite (île de départ)
        {
            sf::Vector2f center = findIslandTile(world, 6);
            if (center.x > -99999.f) {
                auto n = std::make_unique<Neutral>(center);
                n->setSpriteTexture(&neutralTexture);
                n->setFrozen(true);
                n->setIslandType(6);
                entities.push_back(std::move(n));
                neutralPlaced[3] = true;
            }
        }

        
        boat.setSpriteTexture(&boatTexture);
        std::cout << "Boat texture size: " << boatTexture.getSize().x << "x" << boatTexture.getSize().y << std::endl;
        // Cherche une tile d'eau proche du joueur pour spawner le bateau
        {
            if (boat.getDockPosition() == sf::Vector2f(0.f, 0.f)) {
                float gridStepX = 26.0f;
                float gridStepY = 13.0f;
                sf::Vector2f boatSpawn = player.getPosition() + sf::Vector2f(200.f, 100.f);
            for (int radius = 3; radius < 20; radius++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    for (int dy = -radius; dy <= radius; dy++) {
                        if (abs(dx) != radius && abs(dy) != radius) continue;
                        float wx = player.getPosition().x + dx * gridStepX;
                        float wy = player.getPosition().y + dy * gridStepY;
                        float cx = (wx / gridStepX + wy / gridStepY) / 2.0f;
                        float cy = (wy / gridStepY - wx / gridStepX) / 2.0f;
                        int tx = static_cast<int>(std::floor(cx));
                        int ty = static_cast<int>(std::floor(cy));
                        if (world.getTileId(tx, ty) == 0) {
                            boatSpawn = sf::Vector2f(wx, wy);
                            goto foundWater;
                        }
                    }
                }
            }
            foundWater:
            boat.setDockPosition(boatSpawn);
        }
        }
        bool eWasPressed = false;
        bool bWasPressed = false;
        bool spaceWasPressed = false;
        float questAnimTimer = 0.f;
        std::string lastQuestName = "";
        sf::Color lastQuestColor = sf::Color::White;

        // État précédent des quêtes pour détecter les completions
        QuestState prevFire = QuestState::LOCKED;
        QuestState prevWater = QuestState::LOCKED;
        QuestState prevAir = QuestState::LOCKED;
        QuestState prevEarth = QuestState::LOCKED;

        sf::Clock gameClock;

        sf::Clock clock;
        float spawnTimer = 0.f;
        float spawnInterval = 15.f;
        float enemyPower = 1.f;
        bool gameOver = false;
        bool gameWon = false;

        float gridStepX = 26.0f;
        float gridStepY = 13.0f;

        while (window.isOpen() && !gameOver && !gameWon) {
            float dt = clock.restart().asSeconds();

            // Tile sous le joueur
            int pTileX = static_cast<int>(std::floor((player.getPosition().x / gridStepX + player.getPosition().y / gridStepY) / 2.0f));
            int pTileY = static_cast<int>(std::floor((player.getPosition().y / gridStepY - player.getPosition().x / gridStepX) / 2.0f));
            int currentIslandTile = world.getTileId(pTileX, pTileY);

            bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
            bool bPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::B);
            bool ePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);

            // Touche B : monter/descendre du bateau
            if (bPressed && !bWasPressed) {
                if (boat.isActive()) {
                    // Débarquer uniquement sur la plage ou la terre
                    sf::Vector2f boatPos = boat.getPosition();
                    float gX = 26.0f, gY = 13.0f;
                    float cX = (boatPos.x/gX + boatPos.y/gY)/2.f;
                    float cY = (boatPos.y/gY - boatPos.x/gX)/2.f;
                    int tX = static_cast<int>(std::floor(cX));
                    int tY = static_cast<int>(std::floor(cY));
                    int tileId = world.getTileId(tX, tY);
                    if (tileId == 2 || tileId >= 3) {
                        sf::Vector2f landPos = boat.disembark();
                        player.setPosition(landPos);
                    }
                } else {
                    sf::Vector2f boatPos = boat.getPosition();
                    float dx = boatPos.x - player.getPosition().x;
                    float dy = boatPos.y - player.getPosition().y;
                    if (std::sqrt(dx*dx + dy*dy) < 100.f) {
                        boat.embark(boatPos);
                    }
                }
            }
            bWasPressed = bPressed;

            // Génère les fragments de la quête Terre dès que l'île est visible
            if (quests.getEarthQuest().state == QuestState::ACTIVE && boat.getTotalFragments() == 0) {
                sf::Vector2f earthCenter = findIslandTile(world, 6);
                if (earthCenter.x > -99999.f) boat.generateFragments(earthCenter, world);
            }

            // Met à jour le bateau
            if (boat.isActive()) {
                boat.update(dt, world);
                int collected = boat.checkFragmentCollection();
                // Met à jour la progression sans compléter automatiquement
                if (quests.getEarthQuest().state == QuestState::ACTIVE) {
                    quests.getEarthQuest().progress = boat.getCollectedCount();
                    // Ne complète PAS ici — le Neutral valide quand on lui parle
                }
            }

            while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    world.save("savefiles/savefile.json", player, quests, boat);
                    window.close();
                }
                if (event->is<sf::Event::KeyPressed>()) {
                    auto scan = event->getIf<sf::Event::KeyPressed>()->scancode;
                    if (scan == sf::Keyboard::Scan::Num1) player.switchElement(Element::FIRE);
                    if (scan == sf::Keyboard::Scan::Num2) player.switchElement(Element::WATER);
                    if (scan == sf::Keyboard::Scan::Num3) player.switchElement(Element::AIR);
                    if (scan == sf::Keyboard::Scan::Num4) player.switchElement(Element::EARTH);
                    if (scan == sf::Keyboard::Scan::X) quests.closeDialogue();
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F5))
                world.save("savefiles/savefile.json", player, quests, boat);

            // Pouvoir Espace (une seule fois par appui)
            if (spacePressed && !spaceWasPressed) {
                float fireDmgBefore = 0.f;
                if (player.getCurrentElement() == Element::FIRE) {
                    // Dégâts sur les ennemis
                    for (auto& ent : entities) {
                        Enemy* enemy = dynamic_cast<Enemy*>(ent.get());
                        if (!enemy) continue;
                        float dx = enemy->getPosition().x - player.getPosition().x;
                        float dy = enemy->getPosition().y - player.getPosition().y;
                        if (std::sqrt(dx*dx + dy*dy) < 100.f * player.getElementLevel(Element::FIRE))
                            fireDmgBefore += 20.f * player.getElementLevel(Element::FIRE);
                    }
                    // Dégâts sur le Neutral de Feu (gardien)
                    for (auto& ent : entities) {
                        Neutral* neutral = dynamic_cast<Neutral*>(ent.get());
                        if (!neutral || neutral->getIslandType() != 5) continue;
                        float dx = neutral->getPosition().x - player.getPosition().x;
                        float dy = neutral->getPosition().y - player.getPosition().y;
                        if (std::sqrt(dx*dx + dy*dy) < 100.f * player.getElementLevel(Element::FIRE)) {
                            fireDmgBefore += 20.f * player.getElementLevel(Element::FIRE);
                            neutral->takeDamage(20.f * player.getElementLevel(Element::FIRE));
                        }
                    }
                    quests.onFireDamageDealt(fireDmgBefore);
                }
                // Eau : soigne et vérifie volcans
                if (player.getCurrentElement() == Element::WATER) {
                    // Éteindre le volcan sous le joueur si tile de lave (tile 5 = feu)
                    if (currentIslandTile == 5) {
                        world.setWorldTile(pTileX, pTileY, 4); // transforme en eau/herbe
                        quests.onVolcanoExtinguished();
                    }
                }
                // Terre : pont = fragment possible
                if (player.getCurrentElement() == Element::EARTH) {
                    quests.onBridgeBuilt(pTileX, pTileY);
                }
                player.usePower(entities, world);
            }

            // Touche E : interagir avec le Neutral le plus proche
            if (ePressed && !eWasPressed) {
                for (auto& ent : entities) {
                    Neutral* neutral = dynamic_cast<Neutral*>(ent.get());
                    if (!neutral) continue;
                    float dx = neutral->getPosition().x - player.getPosition().x;
                    float dy = neutral->getPosition().y - player.getPosition().y;
                    if (std::sqrt(dx*dx + dy*dy) < 100.f) {
                        // Utilise le type d'île du Neutral, pas la tile sous le joueur
                        int neutralIslandType = neutral->getIslandType();
                        if (neutralIslandType == -1) neutralIslandType = currentIslandTile;
                        quests.interact(neutralIslandType, neutral->getPosition());
                        neutral->setFrozen(true);
                        break;
                    }
                }
            }
            // Libère UNIQUEMENT le Neutral avec qui on parlait quand le dialogue se ferme
            // (les autres restent figés à leur position)
            eWasPressed = ePressed;
            spaceWasPressed = spacePressed;

            if (!boat.isActive()) {
                player.update(dt, world);
            }
            quests.update(dt);

            // Spawn dynamique des Neutrals quand les îles sont découvertes (une seule fois par île)
            for (int i = 0; i < 4; i++) {
                if (neutralPlaced[i]) continue;
                sf::Vector2f center = findIslandTile(world, islandTiles[i]);
                if (center.x > -99999.f) {
                    // Vérifie qu'il n'y a pas déjà un Neutral à cet endroit
                    bool alreadyThere = false;
                    for (auto& ent : entities) {
                        Neutral* n = dynamic_cast<Neutral*>(ent.get());
                        if (!n) continue;
                        float dx = n->getPosition().x - center.x;
                        float dy = n->getPosition().y - center.y;
                        if (std::sqrt(dx*dx + dy*dy) < 200.f) { alreadyThere = true; break; }
                    }
                    if (!alreadyThere) {
                        auto n = std::make_unique<Neutral>(center);
                        n->setSpriteTexture(&neutralTexture);
                        n->setFrozen(true);
                        n->setIslandType(islandTiles[i]);
                        entities.push_back(std::move(n));
                    }
                    neutralPlaced[i] = true;
                }
            }

            for (auto& ent : entities) {
                Enemy* enemy = dynamic_cast<Enemy*>(ent.get());
                Neutral* neutral = dynamic_cast<Neutral*>(ent.get());
                if (enemy) {
                    enemy->update(dt, player.getPosition());
                    if (!boat.isActive()) {
                        float dx = enemy->getPosition().x - player.getPosition().x;
                        float dy = enemy->getPosition().y - player.getPosition().y;
                        if (std::sqrt(dx*dx + dy*dy) < 16.f)
                            player.takeDamage(enemy->getDamage() * dt);
                    }
                    quests.onEnemyNearTemple(enemy->getPosition(), dt);
                }
                else if (neutral) {
                    neutral->update(dt, world);
                }
            }

            // Séparation ennemis
            for (size_t i = 0; i < entities.size(); ++i) {
                Enemy* e1 = dynamic_cast<Enemy*>(entities[i].get());
                if (!e1) continue;
                for (size_t j = i + 1; j < entities.size(); ++j) {
                    Enemy* e2 = dynamic_cast<Enemy*>(entities[j].get());
                    if (!e2) continue;
                    float dx = e1->getPosition().x - e2->getPosition().x;
                    float dy = e1->getPosition().y - e2->getPosition().y;
                    float distance = std::sqrt(dx*dx + dy*dy);
                    if (distance < 32.f && distance > 0.01f) {
                        float overlap = 32.f - distance;
                        sf::Vector2f push((dx/distance)*overlap*0.5f, (dy/distance)*overlap*0.5f);
                        e1->setPosition(e1->getPosition() + push);
                        e2->setPosition(e2->getPosition() - push);
                    }
                }
            }

            // Suppression entités mortes
            for (int i = (int)entities.size() - 1; i >= 0; i--) {
                if (!entities[i]->isAlive()) {
                    if (dynamic_cast<Enemy*>(entities[i].get())) player.addKill();
                    entities.erase(entities.begin() + i);
                }
            }

            if (!player.isAlive()) {
                gameOver = true;
                break;
            }

            // Vérification victoire : 4 quêtes complétées
            if (quests.getFireQuest().state  == QuestState::COMPLETED &&
                quests.getWaterQuest().state == QuestState::COMPLETED &&
                quests.getAirQuest().state   == QuestState::COMPLETED &&
                quests.getEarthQuest().state == QuestState::COMPLETED) {
                gameWon = true;
                break;
            }

            // Spawn ennemis
            spawnTimer += dt;
            if (spawnTimer >= spawnInterval) {
                spawnTimer = 0.f;
                int groupSize = 2 + (rand() % 2);
                for (int i = 0; i < groupSize; i++) {
                    float angle = (rand() % 360) * 3.14159f / 180.f;
                    float dist = 300.f + rand() % 150;
                    sf::Vector2f spawnPos = player.getPosition() + sf::Vector2f(
                        std::cos(angle) * dist, std::sin(angle) * dist);
                    float baseHp = 80.f + rand() % 80;
                    auto e = std::make_unique<Enemy>(spawnPos, 8.f * enemyPower, 350.f, baseHp * enemyPower);
                    e->setSpriteTexture(&enemyTexture);
                    entities.push_back(std::move(e));
                }
                enemyPower += 0.15f;
                spawnInterval = std::max(8.f, spawnInterval - 1.f);
            }

            // Détecte les quêtes nouvellement complétées
            auto checkComplete = [&](QuestState prev, QuestState curr, std::string name, sf::Color color) {
                if (prev != QuestState::COMPLETED && curr == QuestState::COMPLETED) {
                    questAnimTimer = 3.f;
                    lastQuestName = name;
                    lastQuestColor = color;
                }
            };
            checkComplete(prevFire,  quests.getFireQuest().state,  "FEU",   sf::Color(255, 80, 30));
            checkComplete(prevWater, quests.getWaterQuest().state, "EAU",   sf::Color(30, 160, 255));
            checkComplete(prevAir,   quests.getAirQuest().state,   "AIR",   sf::Color(200, 235, 255));
            checkComplete(prevEarth, quests.getEarthQuest().state, "TERRE", sf::Color(160, 110, 40));
            prevFire  = quests.getFireQuest().state;
            prevWater = quests.getWaterQuest().state;
            prevAir   = quests.getAirQuest().state;
            prevEarth = quests.getEarthQuest().state;

            if (boat.isActive()) {
                view.setCenter(boat.getPosition());
            } else {
                view.setCenter(player.getPosition());
            }
            window.setView(view);
            window.clear();

            world.updateChunks(view, noise);
            world.drawChunks(window, &tilemapTexture);
            drawRelics(window, world, view);

            for (auto& ent : entities)
                ent->draw(window);

            if (boat.isActive()) {
                boat.draw(window);
                boat.drawMap(window, view, font, boat.getPosition());
                // Animation victoire fragments
                if (boat.allCollected()) {
                    float t = clock.getElapsedTime().asSeconds();
                    sf::Vector2f center = view.getCenter();
                    // Cercles qui s'expandent
                    for (int i = 0; i < 3; i++) {
                        float r = 30.f + i * 20.f + std::fmod(t * 60.f, 40.f);
                        sf::CircleShape ring(r);
                        ring.setOrigin(sf::Vector2f(r, r));
                        ring.setPosition(center);
                        ring.setFillColor(sf::Color::Transparent);
                        ring.setOutlineColor(sf::Color(255, 220, 80, (uint8_t)(180 - i * 50)));
                        ring.setOutlineThickness(3.f);
                        window.draw(ring);
                    }
                    // Texte central
                    sf::Text win(font);
                    win.setString("Tous les fragments reunis !");
                    win.setCharacterSize(18);
                    win.setFillColor(sf::Color(255, 220, 80));
                    win.setOutlineColor(sf::Color(100, 60, 0));
                    win.setOutlineThickness(2.f);
                    auto wb = win.getLocalBounds();
                    win.setOrigin(sf::Vector2f(wb.size.x/2.f, wb.size.y/2.f));
                    win.setPosition(sf::Vector2f(center.x, center.y - 40.f));
                    window.draw(win);
                    sf::Text hint(font);
                    hint.setString("[B] Retourner sur l'ile");
                    hint.setCharacterSize(13);
                    hint.setFillColor(sf::Color(255, 255, 200));
                    auto hb = hint.getLocalBounds();
                    hint.setOrigin(sf::Vector2f(hb.size.x/2.f, 0.f));
                    hint.setPosition(sf::Vector2f(center.x, center.y - 15.f));
                    window.draw(hint);
                }
            } else {
                boat.draw(window); // dessine le bateau à quai
                player.draw(window);
            }
            player.drawHUD(window, view, font);

            // Chrono de partie
            {
                int totalSec = (int)gameClock.getElapsedTime().asSeconds();
                int minutes = totalSec / 60;
                int seconds = totalSec % 60;
                std::string timeStr = (minutes < 10 ? "0" : "") + std::to_string(minutes) +
                                     ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
                sf::Vector2f base = view.getCenter() - view.getSize() / 2.f;
                sf::Text timeText(font);
                timeText.setCharacterSize(11);
                timeText.setFillColor(sf::Color::Black);
                timeText.setString("Temps: " + timeStr);
                timeText.setPosition(sf::Vector2f(base.x + 12.f, base.y + 70.f));
                window.draw(timeText);
            }
            quests.drawHUDQuests(window, view, font);
            quests.drawDialogue(window, view, font);
            drawCompass(window, view, font, boat.isActive() ? boat.getPosition() : player.getPosition(), entities, quests);

            // Animation quête complétée
            if (questAnimTimer > 0.f) {
                questAnimTimer -= dt;
                float t = clock.getElapsedTime().asSeconds();
                sf::Vector2f center = view.getCenter();
                for (int i = 0; i < 4; i++) {
                    float r = 20.f + i * 25.f + std::fmod(t * 80.f, 50.f);
                    sf::CircleShape ring(r);
                    ring.setOrigin(sf::Vector2f(r, r));
                    ring.setPosition(center);
                    ring.setFillColor(sf::Color::Transparent);
                    ring.setOutlineColor(sf::Color(
                        lastQuestColor.r, lastQuestColor.g, lastQuestColor.b,
                        (uint8_t)(200 * (questAnimTimer / 3.f) - i * 40)));
                    ring.setOutlineThickness(3.f);
                    window.draw(ring);
                }
                sf::Text win(font);
                win.setString("Quete " + lastQuestName + " accomplie !");
                win.setCharacterSize(20);
                win.setFillColor(lastQuestColor);
                win.setOutlineColor(sf::Color(0, 0, 0, 180));
                win.setOutlineThickness(2.f);
                auto wb = win.getLocalBounds();
                win.setOrigin(sf::Vector2f(wb.size.x/2.f, wb.size.y/2.f));
                win.setPosition(sf::Vector2f(center.x, center.y - 30.f));
                window.draw(win);
            }

            window.display();
        }

        // Écran de victoire
        if (gameWon && window.isOpen()) {
            sf::Clock victoryTimer;
            while (window.isOpen()) {
                float elapsed = victoryTimer.getElapsedTime().asSeconds();
                while (const std::optional event = window.pollEvent()) {
                    if (event->is<sf::Event::Closed>()) { window.close(); }
                    if (elapsed > 1.f && event->is<sf::Event::KeyPressed>()) {
                        auto scan = event->getIf<sf::Event::KeyPressed>()->scancode;
                        if (scan == sf::Keyboard::Scan::R) { gameWon = false; break; }
                        if (scan == sf::Keyboard::Scan::Escape) { window.close(); }
                    }
                }
                if (!window.isOpen() || !gameWon) break;
                window.setView(window.getDefaultView());
                window.clear(sf::Color(10, 30, 60));
                sf::Text title(font);
                title.setString("VICTOIRE !");
                title.setCharacterSize(80);
                title.setFillColor(sf::Color(255, 220, 50));
                title.setOutlineColor(sf::Color(255, 140, 0));
                title.setOutlineThickness(3.f);
                auto tb = title.getLocalBounds();
                title.setOrigin(sf::Vector2f(tb.size.x/2.f, tb.size.y/2.f));
                title.setPosition(sf::Vector2f(window.getSize().x/2.f, window.getSize().y/2.f - 100.f));
                window.draw(title);
                sf::Text sub(font);
                sub.setString("Tu as maitrise les 4 elements !");
                sub.setCharacterSize(30);
                sub.setFillColor(sf::Color(200, 230, 255));
                auto sb = sub.getLocalBounds();
                sub.setOrigin(sf::Vector2f(sb.size.x/2.f, 0.f));
                sub.setPosition(sf::Vector2f(window.getSize().x/2.f, window.getSize().y/2.f));
                window.draw(sub);
                if (elapsed > 1.f) {
                    sf::Text replay(font);
                    replay.setString("[R] Rejouer      [Echap] Quitter");
                    replay.setCharacterSize(24);
                    replay.setFillColor(sf::Color(180, 255, 180));
                    auto rb = replay.getLocalBounds();
                    replay.setOrigin(sf::Vector2f(rb.size.x/2.f, 0.f));
                    replay.setPosition(sf::Vector2f(window.getSize().x/2.f, window.getSize().y/2.f + 80.f));
                    window.draw(replay);
                }
                window.display();
            }
        }

        if (gameOver && window.isOpen()) {
            bool allDone =
                quests.getFireQuest().state == QuestState::COMPLETED &&
                quests.getWaterQuest().state == QuestState::COMPLETED &&
                quests.getAirQuest().state == QuestState::COMPLETED &&
                quests.getEarthQuest().state == QuestState::COMPLETED;

            bool replay = showGameOverScreen(
                window,
                font,
                player.getKillCount(),
                gameClock.getElapsedTime().asSeconds(),
                allDone,
                bestKills,
                bestTime
            );

        if (replay) {
            std::remove("savefiles/savefile.json");
        }
        else {
            break;
        }
    }
}
    return 0;
}