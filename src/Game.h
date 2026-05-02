#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <optional>

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
    int score = 0;

    sf::Font font;
    bool fontLoaded = false;
    std::optional<sf::Text> scoreText;
    std::optional<sf::Text> messageText;
    // delay before reshuffling so user can see solved board
    bool pendingReshuffle = false;
    sf::Clock reshuffleClock;
    float reshuffleDelay = 1.0f; // seconds

    struct Tile {
        sf::Sprite sprite;

        int gridX;
        int gridY;

        int correctX;
        int correctY;

        Tile(const sf::Texture& texture, int x, int y)
            : sprite(texture),
            gridX(x),
            gridY(y),
            correctX(x),
            correctY(y) {
        }
    };

    //winning condition: all tiles in correct position
    bool checkWin();

    std::vector<Tile> tiles;

    void createTiles();
    void shuffleTiles();
    void handleClick();

    void processEvents();
    void solvePuzzle();
    void update();
    void render();

    void updateScoreText();

public:
    Game();
    void run();
};