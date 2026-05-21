#include "EndScreen.h"

EndScreen::EndScreen(sf::Font& font)
    : font(font)
{
}

void EndScreen::setup() {
    titleText.emplace(font);
    titleText->setString("Puzzle Complete!");
    titleText->setCharacterSize(48);
    titleText->setFillColor(sf::Color::White);
    titleText->setPosition({ 310.f, 120.f });

    scoreText.emplace(font);
    scoreText->setCharacterSize(28);
    scoreText->setFillColor(sf::Color::White);
    scoreText->setPosition({ 390.f, 220.f });

    timeText.emplace(font);
    timeText->setCharacterSize(28);
    timeText->setFillColor(sf::Color::White);
    timeText->setPosition({ 390.f, 260.f });

    playAgainButton.setSize({ 250.f, 60.f });
    playAgainButton.setPosition({ 375.f, 350.f });
    playAgainButton.setFillColor(sf::Color(80, 80, 80));

    mainMenuButton.setSize({ 250.f, 60.f });
    mainMenuButton.setPosition({ 375.f, 430.f });
    mainMenuButton.setFillColor(sf::Color(80, 80, 80));

    playAgainButtonText.emplace(font);
    playAgainButtonText->setString("Play Again");
    playAgainButtonText->setCharacterSize(28);
    playAgainButtonText->setFillColor(sf::Color::White);
    playAgainButtonText->setPosition({ 430.f, 362.f });

    mainMenuButtonText.emplace(font);
    mainMenuButtonText->setString("Main Menu");
    mainMenuButtonText->setCharacterSize(28);
    mainMenuButtonText->setFillColor(sf::Color::White);
    mainMenuButtonText->setPosition({ 430.f, 442.f });
}

void EndScreen::updateText(int score, float time) {
    if (scoreText) {
        scoreText->setString("Final Score: " + std::to_string(score));
    }

    if (timeText) {
        timeText->setString("Final Time: " + std::to_string(static_cast<int>(time)) + "s");
    }
}

void EndScreen::render(sf::RenderWindow& window) {
    if (titleText) window.draw(*titleText);
    if (scoreText) window.draw(*scoreText);
    if (timeText) window.draw(*timeText);

    window.draw(playAgainButton);
    window.draw(mainMenuButton);

    if (playAgainButtonText) window.draw(*playAgainButtonText);
    if (mainMenuButtonText) window.draw(*mainMenuButtonText);
}