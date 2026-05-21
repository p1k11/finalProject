#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

class EndScreen {
private:
    sf::Font& font;

public:
    std::optional<sf::Text> titleText;
    std::optional<sf::Text> scoreText;
    std::optional<sf::Text> timeText;

    sf::RectangleShape playAgainButton;
    sf::RectangleShape mainMenuButton;

    std::optional<sf::Text> playAgainButtonText;
    std::optional<sf::Text> mainMenuButtonText;

    EndScreen(sf::Font& font);

    void setup();
    void updateText(int score, float time);
    void render(sf::RenderWindow& window);
};