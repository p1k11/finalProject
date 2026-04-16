#include "Game.h"
#include <iostream>
#include <algorithm>
#include <random>

Game::Game()
    : window(sf::VideoMode({ 800, 600 }), "Puzzle Game")
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

    // Size of one tile in the original image
    tileSize = texture.getSize().x / GRID_SIZE;

    // How big the whole puzzle is on screen
    scaleFactor = puzzleDisplaySize / texture.getSize().x;

    float offsetX = (800 - puzzleDisplaySize) / 2.0f;
    float offsetY = (600 - puzzleDisplaySize) / 2.0f;

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (x == 0 && y == 0) {
				continue; // bottom right tile is empty
            }
            sf::Sprite tile(texture);

            // Select part of the image
            tile.setTextureRect(sf::IntRect({
                x * tileSize,
                y * tileSize
                }, {
                    tileSize,
                    tileSize
                }));

                // Scale tile
                tile.setScale({ scaleFactor, scaleFactor });

                // Position tile (scaled + centered)
                tile.setPosition({
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

        tiles[i].setPosition({
            offsetX + x * tileSize * scaleFactor,
            offsetY + y * tileSize * scaleFactor
            });
    }
}
void Game::processEvents() {
    while (auto event = window.pollEvent()) {
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
        window.draw(tile);
    }
    window.display();
}