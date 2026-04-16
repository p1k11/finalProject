#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <random>

class Game {
private:
    sf::RenderWindow window;
    sf::Texture texture;
    std::vector<sf::Sprite> tiles;

    float puzzleDisplaySize = 600.0f; // size of the whole puzzle on screen
    float scaleFactor;

    const int GRID_SIZE = 3;
    int tileSize;
    int emptyX = 0;
    int emptyY = 0;

    void createTiles();
    void shuffleTiles();

    void processEvents();
    void update();
    void render();

public:
    Game();
    void run();
};