#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <optional>
#include <fstream>
#include <sstream>

class Game {
private:
    sf::RenderWindow window;
    sf::Texture texture;

    float puzzleDisplaySize = 600.0f; // size of the whole puzzle on screen
	float scaleFactor;// how much to scale the original tile size to fit the display size

	// 3x3 grid means the game has 8 tiles + 1 empty space
	const int GRID_SIZE = 3;// 3x3 grid
	int tileSize;// size of each tile in the original image
	int emptyX = 0;// position of the empty tile
    int emptyY = 0;// position of the empty tile
	int score = 0;// player's score

	// font and text
	sf::Font font;// flag to check if font loaded successfully
    bool fontLoaded = false;
	std::optional<sf::Text> scoreText;// text to display score
	std::optional<sf::Text> messageText;// text to display messages (like "Puzzle solved!")
    // delay before reshuffling so user can see solved board
    bool pendingReshuffle = false;
    sf::Clock reshuffleClock;
    float reshuffleDelay = 1.0f; // seconds

	// tile struct to hold sprite and grid position
    struct Tile {
        sf::Sprite sprite;

        int gridX;
        int gridY;

        int correctX;
        int correctY;
		// constructor to initialize tile with texture and grid position
        Tile(const sf::Texture& texture, int x, int y)
            : sprite(texture),
            gridX(x),
            gridY(y),
            correctX(x),
            correctY(y) {
        }
    };
	// timer
    sf::Clock gameClock;
    float elapsedTime = 0.0f;

	// text to display timer and leaderboard
    std::optional<sf::Text> timerText;
    std::optional<sf::Text> leaderboardText;

	// struct to hold leaderboard entries
    struct LeaderboardEntry {
        int score;
        float time;
    };

	// leaderboard data
    std::vector<LeaderboardEntry> leaderboard;

    //winning condition: all tiles in correct position
    bool checkWin();
	// game state
    std::vector<Tile> tiles;

	// helper functions
    void createTiles();
    void shuffleTiles();
    void handleClick();

	// main game loop functions
    void processEvents();
    void solvePuzzle();
    void update();
    void render();

	// score related functions
    void updateScoreText();

	// timer related functions
    void updateTimerText();
    void loadLeaderboard();
    void saveLeaderboard();
    void addScoreToLeaderboard();
    void updateLeaderboardText();
    void onPuzzleSolved();

public:
	// constructor and main loop
    Game();
    void run();
};