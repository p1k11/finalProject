#include "Game.h"
#include <iostream>

Game::Game()
    : window(sf::VideoMode({ 800, 600 }), "Puzzle Game")
{
    if (!texture.loadFromFile("image.jpg")) {
        std::cout << "Failed to load image\n";
    }
    else {
        sprite = sf::Sprite(texture);
    }
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
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
    if (sprite.has_value()) {
        window.draw(*sprite);
    }
    window.display();
}