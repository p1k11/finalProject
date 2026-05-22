#pragma once
#include "MainMenu.h"
#include "EndScreen.h"
#include "PauseMenu.h"
#include "Tile.h"
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
    int gridSize = 3;// 3x3 grid
	int tileSize;// size of each tile in the original image
	int emptyX = 0;// position of the empty tile
    int emptyY = 0;// position of the empty tile
	int score = 0;// player's score

    int puzzleScore = 100;
    int totalScore = 0;
    int moveCount = 0;

	// font and text
	sf::Font font;// flag to check if font loaded successfully
    bool fontLoaded = false;
	std::optional<sf::Text> scoreText;// text to display score
	// timer
    sf::Clock gameClock;
    float elapsedTime = 0.0f;

    float pausedTimeTotal = 0.0f;
    sf::Clock pauseClock;

	// text to display timer and leaderboard
    std::optional<sf::Text> timerText;
    std::optional<sf::Text> leaderboardText;
    std::optional<MainMenu> mainMenu;
    std::optional<EndScreen> endScreen;
    std::optional<PauseMenu> pauseMenu;

	// struct to hold leaderboard entries
    struct LeaderboardEntry {
        int score;
        float time;
    };

	// leaderboard data
    std::vector<LeaderboardEntry> leaderboard;

    //winning condition all tiles in correct position
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

    // game states
    enum class GameState {
        MainMenu,
        Playing,
        Paused,
        Leaderboard,
		EndScreen
    };

    GameState state = GameState::MainMenu;

    // Leaderboard/back UI (kept in Game)
    sf::RectangleShape backButton;

    std::optional<sf::Text> pauseText;
    void renderPauseScreen();
 
    sf::RectangleShape pauseButton;
    
    std::optional<sf::Text> pauseButtonText;
    std::optional<sf::Text> backButtonText;
    std::optional<sf::Text> messageText;

    // NOTE: menu setup/render functions moved into MainMenu
    void handleMenuClick(sf::Vector2f mousePos); // updated to use mainMenu

    //the game was creating unsolvable puzzles in the shuffle
    void moveTileToEmpty(Tile& tile);
	//load the image, create tiles from it, and render the tiles
    std::vector<std::string> imageFiles = {
    "image.jpg",
    "image2.jpg",
    "image3.jpg"
    };

    int selectedImageIndex = 0;

    void loadSelectedImage();

    void renderLeaderboardScreen();

public:
	// constructor and main loop
    Game();
    void run();
};