#include "MainMenu.h"

MainMenu::MainMenu(sf::Font& font)
    : font(font)
{
}

void MainMenu::setup(const std::vector<std::string>& imageFiles) {
    titleText.emplace(font);
    titleText->setString("Slide Puzzle");
    titleText->setCharacterSize(48);
    titleText->setFillColor(sf::Color::White);
    titleText->setPosition({ 360.f, 70.f });

    playButton.setSize({ 250.f, 60.f });
    playButton.setPosition({ 375.f, 170.f });
    playButton.setFillColor(sf::Color(80, 80, 80));

    leaderboardButton.setSize({ 250.f, 60.f });
    leaderboardButton.setPosition({ 375.f, 250.f });
    leaderboardButton.setFillColor(sf::Color(80, 80, 80));

    playButtonText.emplace(font);
    playButtonText->setString("Play Game");
    playButtonText->setCharacterSize(28);
    playButtonText->setFillColor(sf::Color::White);
    playButtonText->setPosition({ 430.f, 182.f });

    leaderboardButtonText.emplace(font);
    leaderboardButtonText->setString("Leaderboard");
    leaderboardButtonText->setCharacterSize(28);
    leaderboardButtonText->setFillColor(sf::Color::White);
    leaderboardButtonText->setPosition({ 415.f, 262.f });

    imageButtons.clear();
    imageButtonTexts.clear();

    for (int i = 0; i < static_cast<int>(imageFiles.size()); i++) {
        sf::RectangleShape button;
        button.setSize({ 140.f, 45.f });
        button.setPosition({ 280.f + i * 160.f, 350.f });
        button.setFillColor(sf::Color(80, 80, 80));
        imageButtons.push_back(button);

        imageButtonTexts.emplace_back(font);
        imageButtonTexts[i]->setString("Photo " + std::to_string(i + 1));
        imageButtonTexts[i]->setCharacterSize(20);
        imageButtonTexts[i]->setFillColor(sf::Color::White);
        imageButtonTexts[i]->setPosition({ 310.f + i * 160.f, 360.f });
    }

    difficultyButtons.clear();
    difficultyButtonTexts.clear();

    std::vector<std::string> labels = {
        "Easy 3x3",
        "Medium 4x4",
        "Hard 5x5"
    };

    for (int i = 0; i < 3; i++) {
        sf::RectangleShape button;
        button.setSize({ 170.f, 45.f });
        button.setPosition({ 240.f + i * 190.f, 430.f });
        button.setFillColor(sf::Color(80, 80, 80));
        difficultyButtons.push_back(button);

        difficultyButtonTexts.emplace_back(font);
        difficultyButtonTexts[i]->setString(labels[i]);
        difficultyButtonTexts[i]->setCharacterSize(18);
        difficultyButtonTexts[i]->setFillColor(sf::Color::White);
        difficultyButtonTexts[i]->setPosition({ 270.f + i * 190.f, 442.f });
    }
}

void MainMenu::render(sf::RenderWindow& window, int selectedImageIndex, int gridSize) {
    if (titleText) window.draw(*titleText);

    window.draw(playButton);
    window.draw(leaderboardButton);

    if (playButtonText) window.draw(*playButtonText);
    if (leaderboardButtonText) window.draw(*leaderboardButtonText);

    for (int i = 0; i < static_cast<int>(imageButtons.size()); i++) {
        imageButtons[i].setFillColor(
            i == selectedImageIndex ? sf::Color(120, 120, 120) : sf::Color(80, 80, 80)
        );

        window.draw(imageButtons[i]);

        if (imageButtonTexts[i]) {
            window.draw(*imageButtonTexts[i]);
        }
    }

    for (int i = 0; i < static_cast<int>(difficultyButtons.size()); i++) {
        int buttonGridSize = 3 + i;

        difficultyButtons[i].setFillColor(
            buttonGridSize == gridSize ? sf::Color(120, 120, 120) : sf::Color(80, 80, 80)
        );

        window.draw(difficultyButtons[i]);

        if (difficultyButtonTexts[i]) {
            window.draw(*difficultyButtonTexts[i]);
        }
    }
}