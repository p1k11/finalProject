#pragma once
#include <SFML/Graphics.hpp>

class Game {
private:
    sf::RenderWindow window;
    sf::Texture texture;
    std::optional<sf::Sprite> sprite;

    void processEvents();
    void update();
    void render();

public:
    Game();
    void run();
};