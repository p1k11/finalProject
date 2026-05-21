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

        // initialize the separated main menu
        mainMenu.emplace(font);
        mainMenu->setup(imageFiles);
        endScreen.emplace(font);
        endScreen->setup();
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
                moveTileToEmpty(tile);

            }
			// Check for win condition after move
            if (checkWin()) {
                onPuzzleSolved();
            }

            break;
        }
    }
}

// Handle actions when the puzzle is solved update score leaderboard and display message before reshuffling
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
    if (endScreen) {
        endScreen->updateText(score, elapsedTime);
    }

    state = GameState::EndScreen;
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

    emptyX = gridSize - 1;
    emptyY = gridSize - 1;
}
void Game::createTiles() {
    tiles.clear();

    auto textureSize = texture.getSize();

    int cropSize = std::min(textureSize.x, textureSize.y);
    int cropX = (textureSize.x - cropSize) / 2;
    int cropY = (textureSize.y - cropSize) / 2;

    tileSize = cropSize / gridSize;
    scaleFactor = puzzleDisplaySize / cropSize;

    float offsetX = 250.f;
    float offsetY = (600 - puzzleDisplaySize) / 2.0f;

    emptyX = gridSize - 1;
    emptyY = gridSize - 1;

    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            if (x == emptyX && y == emptyY)
                continue;

            Tile tile(texture, x, y);

            tile.sprite.setTextureRect(sf::IntRect(
                { cropX + x * tileSize, cropY + y * tileSize },
                { tileSize, tileSize }
            ));

            tile.sprite.setScale({ scaleFactor, scaleFactor });

            tile.sprite.setPosition({
                offsetX + x * tileSize * scaleFactor,
                offsetY + y * tileSize * scaleFactor
                });

            tiles.push_back(tile);
        }
    }
}

void Game::moveTileToEmpty(Tile& tile) {
    int oldX = tile.gridX;
    int oldY = tile.gridY;

    tile.gridX = emptyX;
    tile.gridY = emptyY;

    emptyX = oldX;
    emptyY = oldY;

    float offsetX = 250.f;
    float offsetY = (600 - puzzleDisplaySize) / 2.0f;

    tile.sprite.setPosition({
        offsetX + tile.gridX * tileSize * scaleFactor,
        offsetY + tile.gridY * tileSize * scaleFactor
        });
}
void Game::handleMenuClick(sf::Vector2f mousePos) {
    if (!mainMenu) return;

    // image buttons (use MainMenu's imageButtons)
    for (int i = 0; i < static_cast<int>(mainMenu->imageButtons.size()); i++) {
        if (mainMenu->imageButtons[i].getGlobalBounds().contains(mousePos)) {
            selectedImageIndex = i;
            std::cout << "Selected image: " << imageFiles[i] << "\n";
        }
    }

    // difficulty buttons (use MainMenu's difficultyButtons)
    for (int i = 0; i < static_cast<int>(mainMenu->difficultyButtons.size()); i++) {
        if (mainMenu->difficultyButtons[i].getGlobalBounds().contains(mousePos)) {
            if (i == 0) gridSize = 3;
            if (i == 1) gridSize = 4;
            if (i == 2) gridSize = 5;

            std::cout << "Selected grid size: " << gridSize << "x" << gridSize << "\n";
        }
    }

    // play / leaderboard buttons (use MainMenu's shapes)
    if (mainMenu->playButton.getGlobalBounds().contains(mousePos)) {
        score = 0;
        updateScoreText();

        pausedTimeTotal = 0.0f;
        gameClock.restart();

        loadSelectedImage();

        state = GameState::Playing;
    }

    if (mainMenu->leaderboardButton.getGlobalBounds().contains(mousePos)) {
        updateLeaderboardText();
        state = GameState::Leaderboard;
    }
}
void Game::loadSelectedImage() {

    if (!texture.loadFromFile(imageFiles[selectedImageIndex])) {
        std::cout << "Failed to load selected image\n";
        return;
    }

    createTiles();
    shuffleTiles();
}
void Game::shuffleTiles() {

    // Start from solved puzzle
    solvePuzzle();

    std::random_device rd;
    std::mt19937 gen(rd());

    int shuffleMoves = gridSize * gridSize * 30;

    for (int i = 0; i < shuffleMoves; i++) {

        std::vector<int> movableTileIndexes;

        for (int t = 0; t < static_cast<int>(tiles.size()); t++) {

            int dx = abs(tiles[t].gridX - emptyX);
            int dy = abs(tiles[t].gridY - emptyY);

            if (dx + dy == 1) {
                movableTileIndexes.push_back(t);
            }
        }

        std::uniform_int_distribution<> dist(
            0,
            static_cast<int>(movableTileIndexes.size()) - 1
        );

        int chosenIndex = movableTileIndexes[dist(gen)];

        moveTileToEmpty(tiles[chosenIndex]);
    }
}
// Process all window events handle mouse clicks key presses and window close events
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
            else if (state == GameState::EndScreen) {
                if (endScreen && endScreen->playAgainButton.getGlobalBounds().contains(mousePos)) {
                    pausedTimeTotal = 0.0f;
                    gameClock.restart();
                    loadSelectedImage();
                    state = GameState::Playing;
                }

                if (endScreen && endScreen->mainMenuButton.getGlobalBounds().contains(mousePos)) {
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
//winning condition all tiles in correct position
bool Game::checkWin() {
    for (auto& tile : tiles) {

        if (tile.gridX != tile.correctX ||
            tile.gridY != tile.correctY) {

            return false;
        }
    }

    return true;
}
// Update game state update timer handle pending reshuffle after winning and update timer text
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
        if (mainMenu) {
            mainMenu->render(window, selectedImageIndex, gridSize);
        }
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
    else if (state == GameState::EndScreen) {
        if (endScreen) {
            endScreen->render(window);
        }
    }
    window.display();
}
// Update the score text to reflect the current score after a puzzle is solved
void Game::updateScoreText() {
    if (scoreText) {
        scoreText->setString("Score: " + std::to_string(score));
    }
}