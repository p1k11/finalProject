#include "PauseMenu.h"

PauseMenu::PauseMenu(sf::Font& font)
    : font(font)
{
}

void PauseMenu::setup() {
    overlay.setSize({ 1000.f, 600.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    pauseTitleText.emplace(font);
    pauseTitleText->setString("Paused");
    pauseTitleText->setCharacterSize(48);
    pauseTitleText->setFillColor(sf::Color::White);
    pauseTitleText->setPosition({ 410.f, 140.f });

    resumeButton.setSize({ 250.f, 60.f });
    resumeButton.setPosition({ 375.f, 250.f });
    resumeButton.setFillColor(sf::Color(80, 80, 80));

    mainMenuButton.setSize({ 250.f, 60.f });
    mainMenuButton.setPosition({ 375.f, 340.f });
    mainMenuButton.setFillColor(sf::Color(80, 80, 80));

    resumeButtonText.emplace(font);
    resumeButtonText->setString("Resume");
    resumeButtonText->setCharacterSize(28);
    resumeButtonText->setFillColor(sf::Color::White);
    resumeButtonText->setPosition({ 445.f, 262.f });

    mainMenuButtonText.emplace(font);
    mainMenuButtonText->setString("Main Menu");
    mainMenuButtonText->setCharacterSize(28);
    mainMenuButtonText->setFillColor(sf::Color::White);
    mainMenuButtonText->setPosition({ 430.f, 352.f });
}

void PauseMenu::render(sf::RenderWindow& window) {
    window.draw(overlay);

    if (pauseTitleText) window.draw(*pauseTitleText);

    window.draw(resumeButton);
    window.draw(mainMenuButton);

    if (resumeButtonText) window.draw(*resumeButtonText);
    if (mainMenuButtonText) window.draw(*mainMenuButtonText);
}