#include "GameOver.h"

bool showGameOverScreen(sf::RenderWindow& window, sf::Font& font, int kills,
                        float gameTime, bool allQuestsCompleted,
                        int& bestKills, float& bestTime) {
    // Met à jour le meilleur score
    bool newBestKills = (kills > bestKills);
    bool newBestTime  = (allQuestsCompleted && (bestTime < 0.f || gameTime < bestTime));
    if (newBestKills) bestKills = kills;
    if (newBestTime)  bestTime  = gameTime;
    if (newBestKills || newBestTime) saveHighScore(bestKills, bestTime);

    sf::Clock timer;
    bool canInteract = false;
    while (window.isOpen()) {
        float elapsed = timer.getElapsedTime().asSeconds();
        if (elapsed > 1.5f) canInteract = true;
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) { window.close(); return false; }
            if (canInteract && event->is<sf::Event::KeyPressed>()) {
                auto scan = event->getIf<sf::Event::KeyPressed>()->scancode;
                if (scan == sf::Keyboard::Scan::R) return true;
                if (scan == sf::Keyboard::Scan::Escape) { window.close(); return false; }
            }
        }
        window.setView(window.getDefaultView());
        window.clear(sf::Color(80, 0, 0));
        float cx = window.getSize().x / 2.f;
        float cy = window.getSize().y / 2.f;

        sf::Text title(font);
        title.setString("GAME OVER");
        title.setCharacterSize(72);
        title.setFillColor(sf::Color::White);
        auto tb = title.getLocalBounds();
        title.setOrigin(sf::Vector2f(tb.size.x/2.f, 0.f));
        title.setPosition(sf::Vector2f(cx, cy - 160.f));
        window.draw(title);

        // Stats de la partie
        int mins = (int)gameTime / 60;
        int secs = (int)gameTime % 60;
        std::string timeStr = (mins < 10 ? "0" : "") + std::to_string(mins) + ":" +
                              (secs < 10 ? "0" : "") + std::to_string(secs);
        sf::Text stats(font);
        stats.setString("Ennemis elimines : " + std::to_string(kills) +
                        "    Temps : " + timeStr);
        stats.setCharacterSize(26);
        stats.setFillColor(sf::Color(255, 200, 100));
        auto sb = stats.getLocalBounds();
        stats.setOrigin(sf::Vector2f(sb.size.x/2.f, 0.f));
        stats.setPosition(sf::Vector2f(cx, cy - 60.f));
        window.draw(stats);

        // Meilleurs scores
        int bm = (int)bestTime / 60;
        int bs = (int)bestTime % 60;
        std::string bestTimeStr = bestTime < 0.f ? "---" :
            (bm < 10 ? "0" : "") + std::to_string(bm) + ":" +
            (bs < 10 ? "0" : "") + std::to_string(bs);
        sf::Text bsText(font);
        bsText.setString("MEILLEURS SCORES\nKills: " + std::to_string(bestKills) +
                         "    Temps quetes: " + bestTimeStr);
        bsText.setCharacterSize(20);
        bsText.setFillColor(sf::Color(200, 220, 255));
        if (newBestKills || newBestTime)
            bsText.setFillColor(sf::Color(255, 255, 80)); // jaune si nouveau record
        auto bb = bsText.getLocalBounds();
        bsText.setOrigin(sf::Vector2f(bb.size.x/2.f, 0.f));
        bsText.setPosition(sf::Vector2f(cx, cy - 10.f));
        window.draw(bsText);

        if (newBestKills || newBestTime) {
            sf::Text newRecord(font);
            newRecord.setString("NOUVEAU RECORD !");
            newRecord.setCharacterSize(22);
            newRecord.setFillColor(sf::Color(255, 220, 0));
            auto rb = newRecord.getLocalBounds();
            newRecord.setOrigin(sf::Vector2f(rb.size.x/2.f, 0.f));
            newRecord.setPosition(sf::Vector2f(cx, cy + 50.f));
            window.draw(newRecord);
        }

        if (canInteract) {
            sf::Text replay(font);
            replay.setString("[R] Rejouer      [Echap] Quitter");
            replay.setCharacterSize(28);
            replay.setFillColor(sf::Color(180, 255, 180));
            auto rep = replay.getLocalBounds();
            replay.setOrigin(sf::Vector2f(rep.size.x/2.f, 0.f));
            replay.setPosition(sf::Vector2f(cx, cy + 90.f));
            window.draw(replay);
        }
        window.display();
    }
    return false;
}