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
sf::Vector2u AIGame::enemyLoc() const {
    sf::Vector2f enemyPos = enemy.getPosition();
    int x = static_cast<int>(enemyPos.x / 64);
    int y = static_cast<int>(enemyPos.y / 64);
    return sf::Vector2u(x, y);
}

void AIGame::movePlayer(Direction direction) {
    gameMatrixStack.push(gameMatrix);
    sf::Vector2f prevPos = player.getPosition();
    playerPositions.push(prevPos);

    auto moveP = [this](int x, int y) {
        sf::Vector2f newPos = player.getPosition() + sf::Vector2f(x, y);
        sf::Vector2u ghost = enemyLoc();
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
            sf::Vector2u ghost = enemyLoc();
            if (boxX == static_cast<int>(ghost.x) && boxY == static_cast<int>(ghost.y)) {
                return; 
            }
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

float AIGame::heuristic(sf::Vector2u start, sf::Vector2u goal) const{
    return std::abs((int)start.x - (int)goal.x) + std::abs((int)start.y - (int)goal.y);
}

std::vector<sf::Vector2u> AIGame::findPathAStar(sf::Vector2u start, sf::Vector2u goal) const {
    struct Node {
        sf::Vector2u pos;
        float g = 0;
        float h = 0;
        float f = 0;
        Node* parent = nullptr;
        bool walkable = true;
    };

    auto inBounds = [&](sf::Vector2u p) {
        return (int)p.x < width() && (int)p.y < height();
    };

    auto isWalkable = [&](sf::Vector2u p) {
        return inBounds(p) && gameMatrix[getArrayIndex(p.x, p.y)] != '#'
        && gameMatrix[getArrayIndex(p.x, p.y)] != 'A' && gameMatrix[getArrayIndex(p.x, p.y)] != '1'; 
    };

    std::vector<std::vector<Node>> grid(height(), std::vector<Node>(width()));
    for (int y = 0; y < height(); ++y) {
        for (int x = 0; x < width(); ++x) {
            grid[y][x].pos = {static_cast<unsigned>(x), static_cast<unsigned>(y)};
            grid[y][x].walkable = isWalkable({(unsigned)x, (unsigned)y});
        }
    }

    auto cmp = [](Node* a, Node* b) { return a->f > b->f; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> open(cmp);
    std::unordered_set<size_t> closed;

    Node* startNode = &grid[start.y][start.x];

    startNode->g = 0;
    startNode->h = heuristic(start, goal);
    startNode->f = startNode->h;
    open.push(startNode);

    while (!open.empty()) {
        Node* current = open.top();
        open.pop();

        if (current->pos == goal) {
            std::vector<sf::Vector2u> path;
            while (current) {
                path.push_back(current->pos);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closed.insert(getArrayIndex(current->pos.x, current->pos.y));

        std::vector<sf::Vector2u> neighbors = {
            {current->pos.x + 1, current->pos.y},
            {current->pos.x - 1, current->pos.y},
            {current->pos.x, current->pos.y + 1},
            {current->pos.x, current->pos.y - 1}
        };

        for (auto& npos : neighbors) {
            if (!isWalkable(npos)) continue;
            if (closed.count(getArrayIndex(npos.x, npos.y))) continue;

            Node* neighbor = &grid[npos.y][npos.x];
            float tentativeG = current->g + 1;

            if (neighbor->parent == nullptr || tentativeG < neighbor->g) {
                neighbor->parent = current;
                neighbor->g = tentativeG;
                neighbor->h = heuristic(npos, goal);
                neighbor->f = neighbor->g + neighbor->h;
                open.push(neighbor);
            }
        }
    }
    return {}; 
}
int AIGame::evaluateState() const {
    std::vector<sf::Vector2u> path = findPathAStar(enemyLoc(), playerLoc());
    if (path.empty()) return -1000; // unreachable = bad state
    return -static_cast<int>(path.size()); // shorter path = better for enemy
}

void AIGame::moveEnemy() {
    std::vector<sf::Vector2u> path = findPathAStar(enemyLoc(), playerLoc());
    // If path is valid and has at least 2 nodes (current + next)
    if (path.size() >= 2) {
        sf::Vector2u current = enemyLoc();
        sf::Vector2u next = path[1]; // path[0] is current position
        if (next.x > current.x) {
            enemy.setTexture(*enemyRightTex);
        } else if (next.x < current.x) {
            enemy.setTexture(*enemyLeftTex);
        } else if (next.y > current.y) {
            enemy.setTexture(*enemyDownTex);
        } else if (next.y < current.y) {
            enemy.setTexture(*enemyUpTex);
}
        sf::Vector2f newPos(next.x * 64, next.y * 64);
        enemy.setPosition(newPos);
    }
}


bool AIGame::isGameOver(){
    if (this -> enemyLoc() == this -> playerLoc()) {
        return true;
    } 
    return false;
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
                case 'G':
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
    target.draw(enemy, states);

}

void AIGame::reset(const std::string& filePath) {
    static sf::Texture down;
    if (!down.loadFromFile("P_Down.png")) {
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

    int playerX = -1, playerY = -1, enemyX = -1, enemyY = -1;
    for (int i = 0; i < AIGame.h * AIGame.w; ++i) {
        in >> AIGame.gameMatrix[i];
        if (AIGame.gameMatrix[i] == '@') {
            playerX = i % AIGame.w;
            playerY = i / AIGame.w;
        }
        if (AIGame.gameMatrix[i] == 'G') {
            enemyX = i % AIGame.w;
            enemyY = i / AIGame.w;
        }
    }

    if (playerX != -1 && playerY != -1) {
        sf::Vector2u playerPos(playerX, playerY);
        AIGame.player.setPosition(playerPos.x * 64, playerPos.y * 64);
    }
    if (enemyX != -1 && enemyY != -1) {
        sf::Vector2u enemyPos(enemyX, enemyY);
        AIGame.enemy.setPosition(enemyPos.x * 64, enemyPos.y * 64);
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