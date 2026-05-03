#include "Game.h"
#include <iostream>
#include <algorithm>
#include <random>

Game::Game()
    : window(sf::VideoMode({ 800, 600 }), "Slide Puzzle")
{   
	// Load resources image and font
    if (!texture.loadFromFile("image.jpg")) {
        std::cout << "Failed to load image\n";
    }

    if (!font.openFromFile("Helvetica.ttf")) {
        std::cout << "Failed to load font\n";
        fontLoaded = false;
    }
    else {
        fontLoaded = true;

		// setup score text
        scoreText.emplace(font);
        scoreText->setCharacterSize(24);
        scoreText->setFillColor(sf::Color::White);
        scoreText->setPosition({ 10.f, 10.f });

		// setup message text
        messageText.emplace(font);
        messageText->setCharacterSize(36);
        messageText->setFillColor(sf::Color::Yellow);
        messageText->setString("");

		// initialize score text
        updateScoreText();

		// setup timer text
        timerText.emplace(font);
        timerText->setCharacterSize(24);
        timerText->setFillColor(sf::Color::White);
        timerText->setPosition({ 10.f, 40.f });

		// setup leaderboard text
        leaderboardText.emplace(font);
        leaderboardText->setCharacterSize(20);
        leaderboardText->setFillColor(sf::Color::White);
        leaderboardText->setPosition({ 10.f, 70.f });

        // load leaderboard data
        loadLeaderboard();
		// update leaderboard text
        updateLeaderboardText();
    }
	// initialize game state
    createTiles();
	// shuffle tiles to start the game
    shuffleTiles();
}

// Main game loop
void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

// Create tile sprites and set their initial positions based on the original image
void Game::createTiles() {
    tiles.clear();

    tileSize = texture.getSize().x / GRID_SIZE;
    scaleFactor = puzzleDisplaySize / texture.getSize().x;

    float offsetX = (800 - puzzleDisplaySize) / 2.0f;
    float offsetY = (600 - puzzleDisplaySize) / 2.0f;

	// Set empty tile position to bottom right
    emptyX = GRID_SIZE - 1;
    emptyY = GRID_SIZE - 1;

	// Create tiles for all grid positions except the bottom right (empty space)
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {

            // Skip bottom right tile (empty space)
            if (x == emptyX && y == emptyY)
                continue;

            Tile tile(texture, x, y);

            tile.sprite = sf::Sprite(texture);

            tile.sprite.setTextureRect(sf::IntRect({
                x * tileSize,
                y * tileSize
                }, {
                    tileSize,
                    tileSize
                }));

                tile.sprite.setScale({ scaleFactor, scaleFactor });

                tile.gridX = x;
                tile.gridY = y;

                tile.sprite.setPosition({
                    offsetX + x * tileSize * scaleFactor,
                    offsetY + y * tileSize * scaleFactor
                    });
				// Store tile in game state
                tiles.push_back(tile);
        }
    }
}
// Shuffle tiles randomly and update their grid positions and visual positions accordingly
void Game::shuffleTiles() {
    std::shuffle(tiles.begin(), tiles.end(), std::mt19937(std::random_device()()));

    float offsetX = (800 - puzzleDisplaySize) / 2.0f;
    float offsetY = (600 - puzzleDisplaySize) / 2.0f;

    for (int i = 0; i < static_cast<int>(tiles.size()); i++) {
        int x = i % GRID_SIZE;
        int y = i / GRID_SIZE;

        tiles[i].gridX = x;
        tiles[i].gridY = y;

        tiles[i].sprite.setPosition({
            offsetX + x * tileSize * scaleFactor,
            offsetY + y * tileSize * scaleFactor
            });
    }

    // reset empty tile to last position
    emptyX = GRID_SIZE - 1;
    emptyY = GRID_SIZE - 1;
}

// Handle mouse click events to move tiles if they are adjacent to the empty space
void Game::handleClick() {
    auto mousePos = sf::Mouse::getPosition(window);

	// Check if click is on a tile
    for (auto& tile : tiles) {
        if (tile.sprite.getGlobalBounds().contains((sf::Vector2f)mousePos)) {

            int dx = abs(tile.gridX - emptyX);
            int dy = abs(tile.gridY - emptyY);

            // Check if adjacent
            if (dx + dy == 1) {

                // Move tile into empty space
                int oldX = tile.gridX;
                int oldY = tile.gridY;

                tile.gridX = emptyX;
                tile.gridY = emptyY;

                emptyX = oldX;
                emptyY = oldY;

                // Update visual position
                float offsetX = (800 - puzzleDisplaySize) / 2.0f;
                float offsetY = (600 - puzzleDisplaySize) / 2.0f;

                tile.sprite.setPosition({
                    offsetX + tile.gridX * tileSize * scaleFactor,
                    offsetY + tile.gridY * tileSize * scaleFactor
                    });
            }
			// Check for win condition after move
            if (checkWin()) {
                onPuzzleSolved();
            }

            break;
        }
    }
}
// Handle actions when the puzzle is solved: update score, leaderboard, and display message before reshuffling
void Game::onPuzzleSolved() {
    score++;
    updateScoreText();

    addScoreToLeaderboard();

    std::cout << "Puzzle solved!\n";
    std::cout << "Score: " << score << "\n";

    pendingReshuffle = true;
    reshuffleClock.restart();

    if (fontLoaded && messageText) {
        messageText->setString("Puzzle solved!");

        sf::FloatRect bounds = messageText->getLocalBounds();

        messageText->setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
            });

        messageText->setPosition({ 800.f / 2.f, 600.f / 2.f });
    }
}
// Move all tiles to their correct positions to show the solved puzzle before reshuffling
void Game::solvePuzzle() {
    float offsetX = (800 - puzzleDisplaySize) / 2.0f;
    float offsetY = (600 - puzzleDisplaySize) / 2.0f;

    for (auto& tile : tiles) {
        tile.gridX = tile.correctX;
        tile.gridY = tile.correctY;

        tile.sprite.setPosition({
            offsetX + tile.gridX * tileSize * scaleFactor,
            offsetY + tile.gridY * tileSize * scaleFactor
            });
    }

    emptyX = GRID_SIZE - 1;
    emptyY = GRID_SIZE - 1;
}

// Process all window events: handle mouse clicks, key presses, and window close events
void Game::processEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::MouseButtonPressed>()) {
            handleClick();
        }
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        if (event->is<sf::Event::KeyPressed>()) {

            if (auto keyEvent = event->getIf<sf::Event::KeyPressed>()) {

                if (keyEvent->code == sf::Keyboard::Key::W) {
                    solvePuzzle();

                    if (checkWin()) {
                        onPuzzleSolved();
                    }
                }
            }
        }
    }
}

//winning condition: all tiles in correct position
bool Game::checkWin() {
    for (auto& tile : tiles) {

        if (tile.gridX != tile.correctX ||
            tile.gridY != tile.correctY) {

            return false;
        }
    }

    return true;
}
// Update game state: update timer, handle pending reshuffle after winning, and update timer text
void Game::update() {
    elapsedTime = gameClock.getElapsedTime().asSeconds();
    updateTimerText();

    if (pendingReshuffle) {
        if (reshuffleClock.getElapsedTime().asSeconds() >= reshuffleDelay) {
            shuffleTiles();
            pendingReshuffle = false;

            if (fontLoaded && messageText) {
                messageText->setString("");
            }
        }
    }
}

// Update the timer text to show elapsed time in seconds
void Game::updateTimerText() {
    if (timerText) {
        timerText->setString("Time: " + std::to_string(static_cast<int>(elapsedTime)) + "s");
    }
}

// Load leaderboard data from a file and populate the leaderboard vector
void Game::loadLeaderboard() {
    leaderboard.clear();

    std::ifstream file("leaderboard.txt");

    if (!file.is_open()) {
        return;
    }

    int savedScore;
    float savedTime;

    while (file >> savedScore >> savedTime) {
        leaderboard.push_back({ savedScore, savedTime });
    }

    file.close();
}
// Save the current leaderboard data to a file this can be viewed when the game is opeened again
void Game::saveLeaderboard() {
    std::ofstream file("leaderboard.txt");

    for (auto& entry : leaderboard) {
        file << entry.score << " " << entry.time << "\n";
    }

    file.close();
}
// Add the current score and time to the leaderboard, sort it, keep only the top 5 entries, and update the displayed leaderboard text
void Game::addScoreToLeaderboard() {
    leaderboard.push_back({ score, elapsedTime });

    std::sort(leaderboard.begin(), leaderboard.end(),
        [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
            if (a.score == b.score) {
                return a.time < b.time;
            }
            return a.score > b.score;
        });

    if (leaderboard.size() > 5) {
        leaderboard.resize(5);
    }

    saveLeaderboard();
    updateLeaderboardText();
}
// Update the leaderboard text to show the top scores and times in a formatted string
void Game::updateLeaderboardText() {
    if (!leaderboardText) return;

    std::string text = "Top Scores:\n";

    for (int i = 0; i < static_cast<int>(leaderboard.size()); i++) {
        text += std::to_string(i + 1) + ". Score: " +
            std::to_string(leaderboard[i].score) +
            " Time: " +
            std::to_string(static_cast<int>(leaderboard[i].time)) +
            "s\n";
    }

    leaderboardText->setString(text);
}
// Render the game: clear the window, draw all tiles, score, timer, leaderboard, and win message if applicable, then display the updated window
void Game::render() {
    window.clear();

    for (auto& tile : tiles) {
        window.draw(tile.sprite);
    }

    // draw score
    if (fontLoaded) {
        window.draw(*scoreText);
    }
    if (fontLoaded && timerText) {
        window.draw(*timerText);
    }

    if (fontLoaded && leaderboardText) {
        window.draw(*leaderboardText);
    }
    // draw win message while waiting to reshuffle
    if (pendingReshuffle && fontLoaded) {
        window.draw(*messageText);
    }

    window.display();
}
// Update the score text to reflect the current score after a puzzle is solved
void Game::updateScoreText() {
    if (scoreText) {
        scoreText->setString("Score: " + std::to_string(score));
    }
}