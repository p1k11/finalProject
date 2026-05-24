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
    std::vector<sf::Texture> imagePreviewTextures;
    std::vector<sf::Sprite> imagePreviewSprites;
    

    std::vector<sf::RectangleShape> difficultyButtons;
    std::vector<std::optional<sf::Text>> difficultyButtonTexts;

    std::vector<std::vector<sf::RectangleShape>> difficultyGridPreviews;

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> playButtonText;
    std::optional<sf::Text> leaderboardButtonText;

    std::optional<sf::Text> imageSelectText;
    std::optional<sf::Text> difficultySelectText;

    sf::Texture backgroundTexture;
    std::optional<sf::Sprite> backgroundSprite;

    MainMenu(sf::Font& font);

    void setup(const std::vector<std::string>& imageFiles);
    void render(sf::RenderWindow& window, int selectedImageIndex, int gridSize);
};