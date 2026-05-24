#pragma once
#include <SFML/Graphics.hpp>
#include <optional>

class PauseMenu {
private:
    sf::Font& font;

public:
    sf::RectangleShape overlay;

    sf::RectangleShape resumeButton;
    sf::RectangleShape mainMenuButton;

    std::optional<sf::Text> resumeButtonText;
    std::optional<sf::Text> mainMenuButtonText;
    std::optional<sf::Text> pauseTitleText;

    sf::RectangleShape panel;

    PauseMenu(sf::Font& font);

    void setup();
    void render(sf::RenderWindow& window);
};