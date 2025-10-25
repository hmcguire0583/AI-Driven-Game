#include "AIGame.hpp"

namespace SB {

AIGame::AIGame() : h(0), w(0) {}

int AIGame::height() const {
    return h;
}

int AIGame::width() const {
    return w;
}

sf::Vector2u AIGame::playerLoc() const {
    sf::Vector2f playerPos = player.getPosition();
    int x = static_cast<int>(playerPos.x / 64);
    int y = static_cast<int>(playerPos.y / 64);
    return sf::Vector2u(x, y);
}

void AIGame::movePlayer(Direction direction) {
    gameMatrixStack.push(gameMatrix);
    sf::Vector2f prevPos = player.getPosition();
    playerPositions.push(prevPos);

    auto moveP = [this](int x, int y) {
        sf::Vector2f newPos = player.getPosition() + sf::Vector2f(x, y);
        int newX = newPos.x / 64;
        int newY = newPos.y / 64;
        int boxX = newX + x / 64;
        int boxY = newY + y / 64;

        if (newX >= 0 && newX < w && newY >= 0 && newY < h &&
            gameMatrix[getArrayIndex(newX, newY)] != '#' &&
            gameMatrix[getArrayIndex(newX, newY)] != 'A' &&
            gameMatrix[getArrayIndex(newX, newY)] != '1') {
            player.move(x, y);
        }

        if (boxX >= 0 && boxX < w && boxY >= 0 && boxY < h &&
            gameMatrix[getArrayIndex(boxX, boxY)] != 'A' &&
            gameMatrix[getArrayIndex(boxX, boxY)] != '#' &&
            gameMatrix[getArrayIndex(boxX, boxY)] != '1') {

            if (gameMatrix[getArrayIndex(newX, newY)] == 'A') {
                if (gameMatrix[getArrayIndex(boxX, boxY)] == 'a') {
                    gameMatrix[getArrayIndex(boxX, boxY)] = '1';
                    gameMatrix[getArrayIndex(newX, newY)] = '.';
                    player.move(x, y);
                } else {
                    gameMatrix[getArrayIndex(boxX, boxY)] = 'A';
                    gameMatrix[getArrayIndex(newX, newY)] = '.';
                    player.move(x, y);
                }
            }

            if (gameMatrix[getArrayIndex(newX, newY)] == '1') {
                if (gameMatrix[getArrayIndex(boxX, boxY)] == 'a') {
                    gameMatrix[getArrayIndex(boxX, boxY)] = '1';
                    gameMatrix[getArrayIndex(newX, newY)] = 'a';
                    player.move(x, y);
                } else {
                    gameMatrix[getArrayIndex(boxX, boxY)] = 'A';
                    gameMatrix[getArrayIndex(newX, newY)] = 'a';
                    player.move(x, y);
                }
            }
        }
    };

    switch (direction) {
        case Up:
            playerDirections.push(Up);
            moveP(0, -64);
            break;
        case Down:
            playerDirections.push(Down);
            moveP(0, 64);
            break;
        case Left:
            moveP(-64, 0);
            playerDirections.push(Left);
            break;
        case Right:
            playerDirections.push(Right);
            moveP(64, 0);
            break;
    }
}

bool AIGame::isWon() {
    int numBoxes = std::count_if(gameMatrix.begin(), gameMatrix.end(), [](char c) { return c == 'A'; });
    int numStorage = std::count_if(gameMatrix.begin(), gameMatrix.end(), [](char c) { return c == 'a'; });

    for (int i = 0; i < h * w; ++i) {
        int x, y;
        convertToMatrixSpace(i, x, y);
        if ((gameMatrix[i] == '1' && numBoxes == 0) ||
            (gameMatrix[i] == '1' && numStorage == 0) ||
            (numBoxes == 0)) {
            return true;
        }
    }
    return false;
}

void AIGame::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            char tile = gameMatrix[getArrayIndex(x, y)];
            sf::Sprite sprite;

            switch (tile) {
                case '#':
                    sprite = wall;
                    break;
                case 'A':
                    sprite = empty;
                    sprite.setPosition(x * 64, y * 64);
                    target.draw(sprite, states);
                    sprite = box;
                    break;
                case '.':
                    sprite = empty;
                    break;
                case 'a':
                    sprite = storage;
                    break;
                case '1':
                    sprite = storage;
                    sprite.setPosition(x * 64, y * 64);
                    target.draw(sprite, states);
                    sprite = box;
                    break;
                case '@':
                    sprite = empty;
                    break;
                default:
                    continue;
            }

            sprite.setPosition(x * 64, y * 64);
            target.draw(sprite, states);
        }
    }
    target.draw(player, states);
}

void AIGame::reset(const std::string& filePath) {
    static sf::Texture down;
    if (!down.loadFromFile("P_down.png")) {
        exit(1);
    }
    player.setTexture(down);

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Error opening file!" << std::endl;
        exit(1);
    }
    file >> *this;
}

std::ifstream& operator>>(std::ifstream& in, AIGame& AIGame) {
    in >> AIGame.h >> AIGame.w;
    AIGame.gameMatrix.resize(AIGame.h * AIGame.w);

    int playerX = -1, playerY = -1;
    for (int i = 0; i < AIGame.h * AIGame.w; ++i) {
        in >> AIGame.gameMatrix[i];
        if (AIGame.gameMatrix[i] == '@') {
            playerX = i % AIGame.w;
            playerY = i / AIGame.w;
        }
    }

    if (playerX != -1 && playerY != -1) {
        sf::Vector2u playerPos(playerX, playerY);
        AIGame.player.setPosition(playerPos.x * 64, playerPos.y * 64);
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const AIGame& AIGame) {
    out << AIGame.h << " " << AIGame.w;
    return out;
}

int AIGame::getArrayIndex(int x, int y) const {
    return x + y * w;
}

void AIGame::convertToMatrixSpace(int i, int& x, int& y) const {
    x = i % w;
    y = i / w;
}


} // namespace SB