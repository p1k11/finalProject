#include "Game.h"
#include <iostream>
#include <algorithm>
#include <random>

Game::Game()
    : window(sf::VideoMode({ 1000, 600 }), "Slide Puzzle", sf::Style::Titlebar | sf::Style::Close)
{   
	// Load resources image and font
    if (!texture.loadFromFile(imageFiles[selectedImageIndex])) {
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

		// setup pause text
        pauseText.emplace(font);
        pauseText->setString("Paused\nPress P to Resume\nPress M for Menu");
        pauseText->setCharacterSize(36);
        pauseText->setFillColor(sf::Color::White);
        pauseText->setPosition({ 230.f, 220.f });

        // load leaderboard data
        loadLeaderboard();
		// update leaderboard text
        updateLeaderboardText();
    }
    setupMenu();
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
//buttons for selecting images in the main menu
void Game::setupImageButtons() {
    if (!fontLoaded) return;

    imageButtons.clear();
    imageButtonTexts.clear();

    for (int i = 0; i < static_cast<int>(imageFiles.size()); i++) {
        sf::RectangleShape button;
        button.setSize({ 140.f, 45.f });
        button.setPosition({ 280.f + i * 160.f, 420.f });
        button.setFillColor(sf::Color(80, 80, 80));

        imageButtons.push_back(button);

        imageButtonTexts.emplace_back(font);
        imageButtonTexts[i]->setString("Photo " + std::to_string(i + 1));
        imageButtonTexts[i]->setCharacterSize(20);
        imageButtonTexts[i]->setFillColor(sf::Color::White);
        imageButtonTexts[i]->setPosition({ 310.f + i * 160.f, 430.f });
    }
}
//setting up menu anf title
void Game::setupMenu() {
    if (!fontLoaded) return;

    titleText.emplace(font);
    titleText->setString("Slide Puzzle");
    titleText->setCharacterSize(48);
    titleText->setFillColor(sf::Color::White);
    titleText->setPosition({ 360.f, 100.f });

    playButton.setSize({ 250.f, 60.f });
    playButton.setPosition({ 375.f, 230.f });
    playButton.setFillColor(sf::Color(80, 80, 80));

    leaderboardButton.setSize({ 250.f, 60.f });
    leaderboardButton.setPosition({ 375.f, 320.f });
    leaderboardButton.setFillColor(sf::Color(80, 80, 80));

    backButton.setSize({ 180.f, 50.f });
    backButton.setPosition({ 20.f, 520.f });
    backButton.setFillColor(sf::Color(80, 80, 80));

    playButtonText.emplace(font);
    playButtonText->setString("Play Game");
    playButtonText->setCharacterSize(28);
    playButtonText->setFillColor(sf::Color::White);
    playButtonText->setPosition({ 430.f, 242.f });

    leaderboardButtonText.emplace(font);
    leaderboardButtonText->setString("Leaderboard");
    leaderboardButtonText->setCharacterSize(28);
    leaderboardButtonText->setFillColor(sf::Color::White);
    leaderboardButtonText->setPosition({ 415.f, 332.f });

    backButtonText.emplace(font);
    backButtonText->setString("Back");
    backButtonText->setCharacterSize(24);
    backButtonText->setFillColor(sf::Color::White);
    backButtonText->setPosition({ 80.f, 530.f });

    pauseButton.setSize({ 120.f, 40.f });
    pauseButton.setPosition({ 860.f, 10.f });
    pauseButton.setFillColor(sf::Color(80, 80, 80));

    pauseButtonText.emplace(font);
    pauseButtonText->setString("Pause");
    pauseButtonText->setCharacterSize(22);
    pauseButtonText->setFillColor(sf::Color::White);
    pauseButtonText->setPosition({ 890.f, 15.f });

    resumeButton.setSize({ 250.f, 60.f });
    resumeButton.setPosition({ 275.f, 240.f });
    resumeButton.setFillColor(sf::Color(80, 80, 80));

    pauseMenuButton.setSize({ 250.f, 60.f });
    pauseMenuButton.setPosition({ 275.f, 330.f });
    pauseMenuButton.setFillColor(sf::Color(80, 80, 80));

    resumeButtonText.emplace(font);
    resumeButtonText->setString("Resume");
    resumeButtonText->setCharacterSize(28);
    resumeButtonText->setFillColor(sf::Color::White);
    resumeButtonText->setPosition({ 345.f, 252.f });

    pauseMenuButtonText.emplace(font);
    pauseMenuButtonText->setString("Main Menu");
    pauseMenuButtonText->setCharacterSize(28);
    pauseMenuButtonText->setFillColor(sf::Color::White);
    pauseMenuButtonText->setPosition({ 320.f, 342.f });

    setupImageButtons();
}
// Load the selected image and create tiles based on it, then shuffle the tiles to start the game
void Game::loadSelectedImage() {
    if (!texture.loadFromFile(imageFiles[selectedImageIndex])) {
        std::cout << "Failed to load selected image\n";
    }

    createTiles();
    shuffleTiles();
}
// Create tile sprites and set their initial positions based on the original image
void Game::createTiles() {
    tiles.clear();

    auto textureSize = texture.getSize();

    int cropSize = std::min(textureSize.x, textureSize.y);

    int cropX = (textureSize.x - cropSize) / 2;
    int cropY = (textureSize.y - cropSize) / 2;

    tileSize = cropSize / GRID_SIZE;
    scaleFactor = puzzleDisplaySize / cropSize;

    float offsetX = 250.f;
    float offsetY = (600 - puzzleDisplaySize) / 2.0f;

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

    float offsetX = 250.f;
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
                float offsetX = 250.f;
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
    float offsetX = 250.f;
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
void Game::handleMenuClick(sf::Vector2f mousePos) {

    for (int i = 0; i < static_cast<int>(imageButtons.size()); i++) {
        if (imageButtons[i].getGlobalBounds().contains(mousePos)) {
            selectedImageIndex = i;
            std::cout << "Selected image: " << imageFiles[i] << "\n";
        }
    }

    if (playButton.getGlobalBounds().contains(mousePos)) {
        score = 0;
        updateScoreText();

        pausedTimeTotal = 0.0f;
        gameClock.restart();

        loadSelectedImage();

        state = GameState::Playing;
    }

    if (leaderboardButton.getGlobalBounds().contains(mousePos)) {
        updateLeaderboardText();
        state = GameState::Leaderboard;
    }

    
}
// Process all window events handle mouse clicks, key presses, and window close events
void Game::processEvents() {
    while (auto event = window.pollEvent()) {

        if (event->is<sf::Event::MouseButtonPressed>()) {
            auto mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

            if (state == GameState::MainMenu) {
                handleMenuClick(mousePos);
            }
            else if (state == GameState::Playing) {
                if (pauseButton.getGlobalBounds().contains(mousePos)) {
                    state = GameState::Paused;
                    pauseClock.restart();
                }
                else {
                    handleClick();
                }
            }
            else if (state == GameState::Paused) {
                if (resumeButton.getGlobalBounds().contains(mousePos)) {
                    pausedTimeTotal += pauseClock.getElapsedTime().asSeconds();
                    state = GameState::Playing;
                }

                if (pauseMenuButton.getGlobalBounds().contains(mousePos)) {
                    state = GameState::MainMenu;
                }
            }
            else if (state == GameState::Leaderboard) {
                if (backButton.getGlobalBounds().contains(mousePos)) {
                    state = GameState::MainMenu;
                }
            }
        }

        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (event->is<sf::Event::KeyPressed>()) {
            if (auto keyEvent = event->getIf<sf::Event::KeyPressed>()) {

                if (state == GameState::Playing && keyEvent->code == sf::Keyboard::Key::W) {
                    solvePuzzle();

                    if (checkWin()) {
                        onPuzzleSolved();
                    }
                }

                if (keyEvent->code == sf::Keyboard::Key::P) {
                    if (state == GameState::Playing) {
                        state = GameState::Paused;
                        pauseClock.restart();
                    }
                    else if (state == GameState::Paused) {
                        pausedTimeTotal += pauseClock.getElapsedTime().asSeconds();
                        state = GameState::Playing;
                    }
                }

                if (state == GameState::Paused && keyEvent->code == sf::Keyboard::Key::M) {
                    state = GameState::MainMenu;
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
    if (state == GameState::Playing) {
        elapsedTime = gameClock.getElapsedTime().asSeconds() - pausedTimeTotal;

        if (elapsedTime < 0) {
            elapsedTime = 0;
        }

        updateTimerText();
    }

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

void Game::renderMainMenu() {
    if (!fontLoaded) return;

    if (titleText) window.draw(*titleText);

    window.draw(playButton);
    window.draw(leaderboardButton);

    if (playButtonText) window.draw(*playButtonText);
    if (leaderboardButtonText) window.draw(*leaderboardButtonText);
    for (int i = 0; i < static_cast<int>(imageButtons.size()); i++) {
        if (i == selectedImageIndex) {
            imageButtons[i].setFillColor(sf::Color(120, 120, 120));
        }
        else {
            imageButtons[i].setFillColor(sf::Color(80, 80, 80));
        }

        window.draw(imageButtons[i]);

        if (imageButtonTexts[i]) {
            window.draw(*imageButtonTexts[i]);
        }
    }
}

void Game::renderLeaderboardScreen() {
    if (!fontLoaded) return;

    if (leaderboardText) window.draw(*leaderboardText);

    window.draw(backButton);

    if (backButtonText) window.draw(*backButtonText);
}

void Game::renderPauseScreen() {

    for (auto& tile : tiles) {
        window.draw(tile.sprite);
    }

    if (fontLoaded && scoreText)
        window.draw(*scoreText);

    if (fontLoaded && timerText)
        window.draw(*timerText);

    // dark overlay effect
    sf::RectangleShape overlay;
    overlay.setSize({ 1000.f, 600.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    window.draw(overlay);

    window.draw(resumeButton);
    window.draw(pauseMenuButton);

    if (resumeButtonText)
        window.draw(*resumeButtonText);

    if (pauseMenuButtonText)
        window.draw(*pauseMenuButtonText);
}

// Render the game clear the window, draw all tiles, score, timer, leaderboard, and win message if applicable, then display the updated window
void Game::render() {
    window.clear();

    if (state == GameState::MainMenu) {
        renderMainMenu();
    }
    else if (state == GameState::Playing) {
        for (auto& tile : tiles) {
            window.draw(tile.sprite);
        }

        if (fontLoaded && scoreText) window.draw(*scoreText);
        if (fontLoaded && timerText) window.draw(*timerText);

        if (pendingReshuffle && fontLoaded && messageText) {
            window.draw(*messageText);
        }

        window.draw(pauseButton);

        if (pauseButtonText) {
            window.draw(*pauseButtonText);
        }
    }
    else if (state == GameState::Leaderboard) {
        renderLeaderboardScreen();
    }
    else if (state == GameState::Paused) {
        renderPauseScreen();
    }
    window.display();
}
// Update the score text to reflect the current score after a puzzle is solved
void Game::updateScoreText() {
    if (scoreText) {
        scoreText->setString("Score: " + std::to_string(score));
    }
}