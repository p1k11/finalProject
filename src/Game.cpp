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
        timerText->setPosition({ 10.f, 100.f });

		// setup leaderboard text
        leaderboardText.emplace(font);
        leaderboardText->setCharacterSize(24);
        leaderboardText->setFillColor(sf::Color::White);
        leaderboardText->setPosition({ 360.f, 190.f });

        leaderboardPanel.setSize({ 460.f, 360.f });
        leaderboardPanel.setPosition({ 270.f, 120.f });
        leaderboardPanel.setFillColor(sf::Color(40, 25, 70, 230));
        leaderboardPanel.setOutlineColor(sf::Color(170, 120, 230));
        leaderboardPanel.setOutlineThickness(3.f);

        leaderboardTitleText.emplace(font);
        leaderboardTitleText->setString("Leaderboard");
        leaderboardTitleText->setCharacterSize(42);
        leaderboardTitleText->setFillColor(sf::Color::White);
        leaderboardTitleText->setPosition({ 370.f, 135.f });

        backButton.setSize({ 180.f, 50.f });
        backButton.setPosition({ 410.f, 500.f });
        backButton.setFillColor(sf::Color(90, 60, 150));

        backButtonText.emplace(font);
        backButtonText->setString("Back");
        backButtonText->setCharacterSize(24);
        backButtonText->setFillColor(sf::Color::White);
        backButtonText->setPosition({ 465.f, 510.f });

        pauseButton.setSize({ 120.f, 40.f });
        pauseButton.setPosition({ 860.f, 10.f });
        pauseButton.setFillColor(sf::Color(90, 60, 150));

        pauseButtonText.emplace(font);
        pauseButtonText->setString("Pause");
        pauseButtonText->setCharacterSize(22);
        pauseButtonText->setFillColor(sf::Color::White);
        pauseButtonText->setPosition({ 890.f, 15.f });

		//back button for leaderboard
        backButton.setSize({ 180.f, 50.f });
        backButton.setPosition({ 20.f, 520.f });
        backButton.setFillColor(sf::Color(90, 60, 150));

        backButtonText.emplace(font);
        backButtonText->setString("Back");
        backButtonText->setCharacterSize(24);
        backButtonText->setFillColor(sf::Color::White);
        backButtonText->setPosition({ 80.f, 530.f });

        sf::Color normalViolet(90, 60, 150);
        sf::Color hoverViolet(130, 90, 200);
        sf::Color selectedViolet(170, 120, 230);

        leaderboardManager.load();
        updateLeaderboardText();

		// initialize the separated main menu and other separate screens
        mainMenu.emplace(font);
        mainMenu->setup(imageFiles);
        endScreen.emplace(font);
        endScreen->setup();
        pauseMenu.emplace(font);
        pauseMenu->setup();
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
                moveTileToEmpty(tile);

                moveCount++;

                if (puzzleScore > 0) {
                    puzzleScore--;
                }

                updateScoreText();
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
    totalScore += puzzleScore;
    updateScoreText();

    leaderboardManager.addScore(totalScore, elapsedTime);
    updateLeaderboardText();

    if (endScreen) {
        endScreen->updateText(totalScore, elapsedTime);
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

    // image buttons 
    for (int i = 0; i < static_cast<int>(mainMenu->imageButtons.size()); i++) {
        if (mainMenu->imageButtons[i].getGlobalBounds().contains(mousePos)) {
            selectedImageIndex = i;
            std::cout << "Selected image: " << imageFiles[i] << "\n";
        }
    }

    // difficulty buttons 
    for (int i = 0; i < static_cast<int>(mainMenu->difficultyButtons.size()); i++) {
        if (mainMenu->difficultyButtons[i].getGlobalBounds().contains(mousePos)) {
            if (i == 0) gridSize = 3;
            if (i == 1) gridSize = 4;
            if (i == 2) gridSize = 5;

            std::cout << "Selected grid size: " << gridSize << "x" << gridSize << "\n";
        }
    }

    // play / leaderboard buttons use MainMenu's shapes
    if (mainMenu->playButton.getGlobalBounds().contains(mousePos)) {
        puzzleScore = 100;
        totalScore = 0;
        moveCount = 0;
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
void Game::drawTileBorder(const Tile& tile) {
    sf::RectangleShape border;

    border.setSize({
        tileSize * scaleFactor,
        tileSize * scaleFactor
        });

    border.setPosition(tile.sprite.getPosition());

    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::White);
    border.setOutlineThickness(2.f);

    window.draw(border);
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
                if (pauseMenu && pauseMenu->resumeButton.getGlobalBounds().contains(mousePos)) {
                    pausedTimeTotal += pauseClock.getElapsedTime().asSeconds();
                    state = GameState::Playing;
                }

                if (pauseMenu && pauseMenu->mainMenuButton.getGlobalBounds().contains(mousePos)) {
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
}

// Update the timer text to show elapsed time in seconds
void Game::updateTimerText() {
    if (timerText) {
        timerText->setString("Time: " + std::to_string(static_cast<int>(elapsedTime)) + "s");
    }
}
//using leaderboard manager to load and save leaderboard data and update the leaderboard text to show top scores and times
void Game::updateLeaderboardText() {
    if (!leaderboardText) return;

    std::string text = "Top Scores:\n";

    const auto& entries = leaderboardManager.getEntries();

    for (int i = 0; i < static_cast<int>(entries.size()); i++) {
        text += std::to_string(i + 1) + ". Score: " +
            std::to_string(entries[i].score) +
            " Time: " +
            std::to_string(static_cast<int>(entries[i].time)) +
            "s\n";
    }

    leaderboardText->setString(text);
}

void Game::renderLeaderboardScreen() {
    if (!fontLoaded) return;

    window.draw(leaderboardPanel);

    if (leaderboardTitleText) {
        window.draw(*leaderboardTitleText);
    }

    if (leaderboardText) {
        window.draw(*leaderboardText);
    }
    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

    sf::Color normalViolet(90, 60, 150);
    sf::Color hoverViolet(130, 90, 200);

    backButton.setFillColor(
        backButton.getGlobalBounds().contains(mousePos) ? hoverViolet : normalViolet
    );
    window.draw(backButton);

    if (backButtonText) {
        window.draw(*backButtonText);
    }
}

void Game::renderPauseScreen() {
    for (auto& tile : tiles) {
        window.draw(tile.sprite);
    }

    if (fontLoaded && scoreText) window.draw(*scoreText);
    if (fontLoaded && timerText) window.draw(*timerText);

    if (pauseMenu) {
        pauseMenu->render(window);
    }
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
        sf::RectangleShape emptySpace;
        emptySpace.setSize({
            tileSize * scaleFactor,
            tileSize * scaleFactor
            });

        emptySpace.setPosition({
            250.f + emptyX * tileSize * scaleFactor,
            (600 - puzzleDisplaySize) / 2.0f + emptyY * tileSize * scaleFactor
            });

        emptySpace.setFillColor(sf::Color(90, 60, 150));
        window.draw(emptySpace);
        for (auto& tile : tiles) {
            window.draw(tile.sprite);
            drawTileBorder(tile);
        }

        if (fontLoaded && scoreText) window.draw(*scoreText);
        if (fontLoaded && timerText) window.draw(*timerText);

        sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

        pauseButton.setFillColor(
            pauseButton.getGlobalBounds().contains(mousePos)
            ? sf::Color(130, 90, 200)
            : sf::Color(90, 60, 150)
        );

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
        scoreText->setString(
            "Puzzle Score: " + std::to_string(puzzleScore) +
            "\nTotal Score: " + std::to_string(totalScore) +
            "\nMoves: " + std::to_string(moveCount)
        );
    }
}