#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <random>

class Game {
private:
    sf::RenderWindow window;
    sf::Texture texture;

    float puzzleDisplaySize = 600.0f; // size of the whole puzzle on screen
    float scaleFactor;

    const int GRID_SIZE = 3;
    int tileSize;
    int emptyX = 0;
    int emptyY = 0;

    struct Tile {
        sf::Sprite sprite;
        int gridX;
        int gridY;

        Tile(const sf::Texture& texture, int x, int y)
            : sprite(texture), gridX(x), gridY(y) {
        }
    };

    std::vector<Tile> tiles;

    void createTiles();
    void shuffleTiles();
    void handleClick();

    void processEvents();
    void update();
    void render();

public:
    Game();
    void run();
};