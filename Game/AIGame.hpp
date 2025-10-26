#ifndef AIGame_HPP
#define AIGame_HPP

#include <stack>
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Audio.hpp>
#include <queue>
#include <unordered_set>

namespace SB {

enum Direction { Up, Down, Left, Right };

class AIGame : public sf::Drawable {
public:
    sf::Sprite wall;
    sf::Sprite box;
    sf::Sprite empty;
    sf::Sprite storage;
    sf::Sprite player;
    sf::Sprite enemy;
    sf::Texture* enemyUpTex = nullptr;
    sf::Texture* enemyDownTex = nullptr;
    sf::Texture* enemyLeftTex = nullptr;
    sf::Texture* enemyRightTex = nullptr;

    AIGame();

    int height() const;
    int width() const;
    sf::Vector2u playerLoc() const;
    sf::Vector2u enemyLoc() const;
    void movePlayer(Direction direction);
    float heuristic(sf::Vector2u start, sf::Vector2u goal) const;
    std::vector<sf::Vector2u> findPathAStar(sf::Vector2u start, sf::Vector2u goal) const;
    void moveEnemy();
    int evaluateState() const;
    bool isWon();
    bool isGameOver();

    friend std::ifstream& operator>>(std::ifstream& in, AIGame& AIGame);
    friend std::ostream& operator<<(std::ostream& out, const AIGame& AIGame);

    int getArrayIndex(int x, int y) const;
    void convertToMatrixSpace(int i, int& x, int& y) const;
    void reset(const std::string& filePath);

protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    int h;
    int w;
    std::vector<char> gameMatrix;
    std::stack<std::vector<char>> gameMatrixStack;
    std::stack<sf::Vector2f> playerPositions;
    std::stack<Direction> playerDirections;
};

} // namespace SB

#endif // AIGame_HPP