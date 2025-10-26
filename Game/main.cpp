#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <regex>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "AIGame.hpp"

namespace fs = std::filesystem;

std::vector<std::string> getLevelFiles(const std::string& folder) {
    std::vector<std::string> levels;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.path().extension() == ".lvl") {
            levels.push_back(entry.path().string());
        }
    }
    std::sort(levels.begin(), levels.end(), [](const std::string& a, const std::string& b) {
        std::regex re("level(\\d+)\\.lvl");
        std::smatch ma, mb;
        std::string fa = fs::path(a).filename().string();
        std::string fb = fs::path(b).filename().string();
        if (std::regex_search(fa, ma, re) && std::regex_search(fb, mb, re)) {
            return std::stoi(ma[1]) < std::stoi(mb[1]);
        }
        return fa < fb;
    });
    return levels;
}

int runGame(const std::string& selectedLevel, const sf::Font& font) {
    std::ifstream input_file(selectedLevel);
    if (!input_file.is_open()) {
        std::cerr << "Failed to open file: " << selectedLevel << std::endl;
        return 1;
    }

    sf::Texture wallT, boxT, emptyT, storageT, playerT, up, down, left, right;
    sf::Texture enemyU, enemyD, enemyL, enemyR;
    if (!wallT.loadFromFile("Wall.png") ||
        !boxT.loadFromFile("Crate.png") ||
        !emptyT.loadFromFile("floor.png") ||
        !storageT.loadFromFile("Storage.png") ||
        !enemyU.loadFromFile("E_Up.png") ||
        !enemyD.loadFromFile("E_Down.png") ||
        !enemyL.loadFromFile("E_Left.png") ||
        !enemyR.loadFromFile("E_Right.png") ||
        !playerT.loadFromFile("P_Down.png") ||
        !up.loadFromFile("P_Up.png") ||
        !down.loadFromFile("P_Down.png") ||
        !left.loadFromFile("P_Left.png") ||
        !right.loadFromFile("P_Right.png")) return EXIT_FAILURE;

    SB::AIGame game;
    game.wall.setTexture(wallT);
    game.box.setTexture(boxT);
    game.empty.setTexture(emptyT);
    game.storage.setTexture(storageT);
    game.player.setTexture(playerT);
    game.enemyUpTex = &enemyU;
    game.enemyDownTex = &enemyD;
    game.enemyLeftTex = &enemyL;
    game.enemyRightTex = &enemyR;
    input_file >> game;
    input_file.close();

    sf::SoundBuffer winBuffer, failBuffer;
    if (!winBuffer.loadFromFile("sound.wav") || !failBuffer.loadFromFile("fail-trumpet-242645.wav")) return EXIT_FAILURE;
    sf::Sound winSound(winBuffer), failSound(failBuffer);
    bool winSoundPlayed = false, failSoundPlayed = false;
    sf::Clock enemyClock;
    const sf::Time enemyMoveInterval = sf::milliseconds(500);

    sf::RenderWindow window(sf::VideoMode(game.width() * 64, game.height() * 64), "Block Pusher");
    sf::Text backButton("Go Back", font, 30);
    backButton.setFillColor(sf::Color::White);
    backButton.setPosition((window.getSize().x - backButton.getLocalBounds().width) / 2,
                           (window.getSize().y / 2) + 60);

    bool showEndScreen = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    winSoundPlayed = false;
                    failSoundPlayed = false;
                    game.reset(selectedLevel);
                    showEndScreen = false;
                } else if (!game.isWon() && !game.isGameOver()) {
                    if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
                        game.player.setTexture(up);
                        game.movePlayer(SB::Direction::Up);
                    } else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
                        game.player.setTexture(down);
                        game.movePlayer(SB::Direction::Down);
                    } else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
                        game.player.setTexture(left);
                        game.movePlayer(SB::Direction::Left);
                    } else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
                        game.player.setTexture(right);
                        game.movePlayer(SB::Direction::Right);
                    }
                }
            } else if (showEndScreen && event.type == sf::Event::MouseButtonPressed) {
                if (backButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    window.close();
                    return 2; // signal to restart
                }
            }
        }

        if (!game.isWon() && !game.isGameOver() && enemyClock.getElapsedTime() >= enemyMoveInterval) {
            game.moveEnemies();
            enemyClock.restart();
        }

        window.clear();
        window.draw(game);

        if (game.isWon() || game.isGameOver()) {
            sf::Text endText(game.isWon() ? "You win!" : "Game Over!", font, 50);
            endText.setPosition((window.getSize().x - endText.getLocalBounds().width) / 2,
                                (window.getSize().y - endText.getLocalBounds().height) / 2);
            window.draw(endText);
            window.draw(backButton);
            sf::sleep(sf::seconds(0.1));
            if (game.isWon() && !winSoundPlayed) {
                winSound.play();
                winSoundPlayed = true;
            } else if (game.isGameOver() && !failSoundPlayed) {
                failSound.play();
                failSoundPlayed = true;
            }
            showEndScreen = true;
        }

        window.display();
    }

    return 0;
}

int main() {
    sf::Font font;
    if (!font.loadFromFile("OpenSans-Bold.ttf")) return EXIT_FAILURE;

    while (true) {
        std::vector<std::string> levels = getLevelFiles("levels/");
        if (levels.empty()) {
            std::cerr << "No level files found in ./levels/" << std::endl;
            return 1;
        }

        sf::RenderWindow menu(sf::VideoMode(800, 600), "Select Level");
        std::vector<sf::Text> levelButtons;
        for (size_t i = 0; i < levels.size(); ++i) {
            sf::Text text(fs::path(levels[i]).filename().string(), font, 30);
            text.setPosition(100, 100 + i * 50);
            levelButtons.push_back(text);
        }

        std::string selectedLevel;
        while (menu.isOpen()) {
            sf::Event event;
            while (menu.pollEvent(event)) {
                if (event.type == sf::Event::Closed) return 0;
                else if (event.type == sf::Event::MouseButtonPressed) {
                    for (size_t i = 0; i < levelButtons.size(); ++i) {
                        if (levelButtons[i].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            selectedLevel = levels[i];
                            menu.close();
                        }
                    }
                }
            }
            menu.clear();
            for (const auto& button : levelButtons) menu.draw(button);
            menu.display();
        }

        if (selectedLevel.empty()) return 0;
        int result = runGame(selectedLevel, font);
        if (result != 2) break; // 2 means restart, otherwise exit
    }

    return 0;
}