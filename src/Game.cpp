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

            // Skip bottom-right tile (empty space)
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

    for (int i = 0; i < tiles.size(); i++) {
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

            break;
        }
    }
}

void Game::processEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::MouseButtonPressed>()) {
            handleClick();
        }
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
    }
}

void Game::update() {
    // nothing yet
}

void Game::render() {
    window.clear();

    for (auto& tile : tiles) {
        window.draw(tile.sprite);
    }

    window.display();
}