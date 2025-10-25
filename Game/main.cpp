#include "AIGame.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " level" << std::endl;
        return 1;
    }

    std::string level = argv[1];
    std::ifstream input_file(level);
    if (!input_file.is_open()) {
        std::cout << "Failed to open file!" << std::endl;
        return 1;
    }

    sf::Texture wallT, boxT, emptyT, storageT, playerT, up, down, left, right;
    if (!wallT.loadFromFile("Wall.png") ||
        !boxT.loadFromFile("Crate.png") ||
        !emptyT.loadFromFile("floor.png") ||
        !storageT.loadFromFile("Storage.png") ||
        !playerT.loadFromFile("P_Down.png") ||
        !up.loadFromFile("P_Up.png") ||
        !down.loadFromFile("P_Down.png") ||
        !left.loadFromFile("P_Left.png") ||
        !right.loadFromFile("P_Right.png")) {
        return EXIT_FAILURE;
    }

    SB::AIGame game;
    game.wall.setTexture(wallT);
    game.box.setTexture(boxT);
    game.empty.setTexture(emptyT);
    game.storage.setTexture(storageT);
    game.player.setTexture(playerT);
    input_file >> game;

    sf::Vector2u playerLocation = game.playerLoc();
    std::cout << "Player's location: (" << playerLocation.x << ", "
              << playerLocation.y << ")" << std::endl;
    std::cout << game << std::endl;

    input_file.close();

    sf::SoundBuffer winBuffer;
    if (!winBuffer.loadFromFile("sound.wav")) {
        return EXIT_FAILURE;
    }

    sf::Sound winSound;
    winSound.setBuffer(winBuffer);
    bool winSoundPlayed = false;

    sf::Font font;
    if (!font.loadFromFile("OpenSans-Bold.ttf")) {
        return EXIT_FAILURE;
    }

    sf::RenderWindow window(sf::VideoMode(game.width() * 64, game.height() * 64),
                            "Block Pusher");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    winSoundPlayed = false;
                    game.reset(level);
                } else if (!game.isWon()) {
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
            }
        }

        window.clear();
        window.draw(game);

        if (game.isWon()) {
            sf::Text victoryText("You win!", font, 50);
            victoryText.setPosition((window.getSize().x - victoryText.getLocalBounds().width) / 2,
                                    (window.getSize().y - victoryText.getLocalBounds().height) / 2);
            window.draw(victoryText);
            sf::sleep(sf::seconds(0.1));
            if (!winSoundPlayed) {
                winSound.play();
                winSoundPlayed = true;
            }
        }

        window.display();
    }
}