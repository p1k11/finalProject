#include "Game.h"
#include <iostream>
#include <algorithm>
#include <random>

Game::Game()
    : window(sf::VideoMode({ 800, 600 }), "Slide Puzzle")
{
    if (!texture.loadFromFile("image.jpg")) {
        std::cout << "Failed to load image\n";
    }

    if (!font.openFromFile("Helvetica.ttf")) {
        std::cout << "Failed to load font\n";
        fontLoaded = false;
    }
    else {
        fontLoaded = true;

        scoreText.emplace(font);
        scoreText->setCharacterSize(24);
        scoreText->setFillColor(sf::Color::White);
        scoreText->setPosition({ 10.f, 10.f });

        messageText.emplace(font);
        messageText->setCharacterSize(36);
        messageText->setFillColor(sf::Color::Yellow);
        messageText->setString("");

        updateScoreText();
    }

    createTiles();
    shuffleTiles();
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::createTiles() {
    tiles.clear();

    tileSize = texture.getSize().x / GRID_SIZE;
    scaleFactor = puzzleDisplaySize / texture.getSize().x;

    float offsetX = (800 - puzzleDisplaySize) / 2.0f;
    float offsetY = (600 - puzzleDisplaySize) / 2.0f;

    emptyX = GRID_SIZE - 1;
    emptyY = GRID_SIZE - 1;

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

                tiles.push_back(tile);
        }
    }
}
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

void Game::handleClick() {
    auto mousePos = sf::Mouse::getPosition(window);

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

            if (checkWin()) {
                // increment score and show message. delay reshuffle so user can see solved board
                score++;
                updateScoreText();

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

            break;
        }
    }
}
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
                        score++;
                        updateScoreText();

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
void Game::update() {
    // handle delayed reshuffle after win
    if (pendingReshuffle) {
        if (reshuffleClock.getElapsedTime().asSeconds() >= reshuffleDelay) {
            shuffleTiles();
            pendingReshuffle = false;
            if (fontLoaded) {
                messageText->setString("");
            }
        }
    }
}

void Game::render() {
    window.clear();

    for (auto& tile : tiles) {
        window.draw(tile.sprite);
    }

    // draw score
    if (fontLoaded) {
        window.draw(*scoreText);
    }

    // draw win message while waiting to reshuffle
    if (pendingReshuffle && fontLoaded) {
        window.draw(*messageText);
    }

    window.display();
}

void Game::updateScoreText() {
    if (scoreText) {
        scoreText->setString("Score: " + std::to_string(score));
    }
}