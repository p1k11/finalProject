#include "Tile.h"

Tile::Tile(const sf::Texture& texture, int x, int y)
    : sprite(texture),
    gridX(x),
    gridY(y),
    correctX(x),
    correctY(y)
{
}