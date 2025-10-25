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

    AIGame();

    int height() const;
    int width() const;
    sf::Vector2u playerLoc() const;
    void movePlayer(Direction direction);
    bool isWon();

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