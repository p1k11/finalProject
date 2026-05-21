#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <string>

class MainMenu {
private:
    sf::Font& font;

public:
    sf::RectangleShape playButton;
    sf::RectangleShape leaderboardButton;

    std::vector<sf::RectangleShape> imageButtons;
    std::vector<std::optional<sf::Text>> imageButtonTexts;

    std::vector<sf::RectangleShape> difficultyButtons;
    std::vector<std::optional<sf::Text>> difficultyButtonTexts;

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> playButtonText;
    std::optional<sf::Text> leaderboardButtonText;

    MainMenu(sf::Font& font);

    void setup(const std::vector<std::string>& imageFiles);
    void render(sf::RenderWindow& window, int selectedImageIndex, int gridSize);
};