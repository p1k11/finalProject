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
    pauseTitleText->setPosition({ 410.f, 180.f });

    resumeButton.setSize({ 250.f, 60.f });
    resumeButton.setPosition({ 375.f, 250.f });
    resumeButton.setFillColor(sf::Color(90, 60, 150));

    mainMenuButton.setSize({ 250.f, 60.f });
    mainMenuButton.setPosition({ 375.f, 340.f });
    mainMenuButton.setFillColor(sf::Color(90, 60, 150));

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

    panel.setSize({ 360.f, 280.f });
    panel.setPosition({ 320.f, 160.f });
    panel.setFillColor(sf::Color(40, 25, 70, 230));
    panel.setOutlineColor(sf::Color(170, 120, 230));
    panel.setOutlineThickness(3.f);
}

void PauseMenu::render(sf::RenderWindow& window) {
    window.draw(overlay);
    window.draw(panel);


    if (pauseTitleText) window.draw(*pauseTitleText);

    window.draw(resumeButton);
    window.draw(mainMenuButton);

    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

    sf::Color normalViolet(90, 60, 150);
    sf::Color hoverViolet(130, 90, 200);
    resumeButton.setFillColor(
        resumeButton.getGlobalBounds().contains(mousePos) ? hoverViolet : normalViolet
    );

    mainMenuButton.setFillColor(
        mainMenuButton.getGlobalBounds().contains(mousePos) ? hoverViolet : normalViolet
    );

    if (resumeButtonText) window.draw(*resumeButtonText);
    if (mainMenuButtonText) window.draw(*mainMenuButtonText);


}