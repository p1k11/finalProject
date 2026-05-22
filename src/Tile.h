#pragma once
#include <SFML/Graphics.hpp>

struct Tile {
    sf::Sprite sprite;

    int gridX;
    int gridY;

    int correctX;
    int correctY;

    Tile(const sf::Texture& texture, int x, int y);
};