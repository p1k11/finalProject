#include "MainMenu.h"
#include "algorithm"

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
    playButton.setFillColor(sf::Color(90, 60, 150));

    leaderboardButton.setSize({ 250.f, 60.f });
    leaderboardButton.setPosition({ 375.f, 250.f });
    leaderboardButton.setFillColor(sf::Color(90, 60, 150));

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

    imageSelectText.emplace(font);
    imageSelectText->setString("Choose Photo");
    imageSelectText->setCharacterSize(22);
    imageSelectText->setFillColor(sf::Color::White);
    imageSelectText->setPosition({ 420.f, 310.f });

    difficultySelectText.emplace(font);
    difficultySelectText->setString("Choose Difficulty");
    difficultySelectText->setCharacterSize(22);
    difficultySelectText->setFillColor(sf::Color::White);
    difficultySelectText->setPosition({ 400.f, 490.f });

    imageButtons.clear();
    

    if (backgroundTexture.loadFromFile("image.jpg")) {
        backgroundSprite.emplace(backgroundTexture);

        float scaleX = 1000.f / backgroundTexture.getSize().x;
        float scaleY = 600.f / backgroundTexture.getSize().y;

        backgroundSprite->setScale({ scaleX, scaleY });
    }
    imageButtons.clear();
    imagePreviewTextures.clear();
    imagePreviewSprites.clear();

    imageButtons.reserve(imageFiles.size());
    imagePreviewTextures.reserve(imageFiles.size());
    imagePreviewSprites.reserve(imageFiles.size());

    float thumbnailSize = 120.f;
    float startX = 280.f;
    float startY = 350.f;
    float spacing = 180.f;

    for (int i = 0; i < static_cast<int>(imageFiles.size()); i++) {
        imagePreviewTextures.emplace_back();

        if (imagePreviewTextures.back().loadFromFile(imageFiles[i])) {
            sf::Sprite sprite(imagePreviewTextures.back());

            auto size = imagePreviewTextures.back().getSize();

            int cropSize = static_cast<int>(std::min(size.x, size.y));
            int cropX = (static_cast<int>(size.x) - cropSize) / 2;
            int cropY = (static_cast<int>(size.y) - cropSize) / 2;

            sprite.setTextureRect(sf::IntRect(
                { cropX, cropY },
                { cropSize, cropSize }
            ));

            float scale = thumbnailSize / cropSize;
            sprite.setScale({ scale, scale });

            sprite.setPosition({
                startX + i * spacing,
                startY
                });

            imagePreviewSprites.push_back(sprite);

            sf::RectangleShape border;
            border.setSize({ thumbnailSize, thumbnailSize });
            border.setPosition({
                startX + i * spacing,
                startY
                });

            border.setFillColor(sf::Color::Transparent);
            border.setOutlineThickness(4.f);
            border.setOutlineColor(sf::Color(90, 60, 150));

            imageButtons.push_back(border);
        }
    }

    difficultyButtons.clear();
    difficultyButtonTexts.clear();
    difficultyGridPreviews.clear();

    std::vector<std::string> labels = {
    "Easy 3x3",
    "Medium 4x4",
    "Hard 5x5"
    };

    std::vector<int> gridSizes = { 3, 4, 5 };

    for (int i = 0; i < 3; i++) {
        sf::RectangleShape button;
        button.setSize({ 170.f, 70.f });
        button.setPosition({ 240.f + i * 190.f, 520.f });
        button.setFillColor(sf::Color(90, 60, 150));
        difficultyButtons.push_back(button);

        difficultyButtonTexts.emplace_back(font);
        difficultyButtonTexts[i]->setString(labels[i]);
        difficultyButtonTexts[i]->setCharacterSize(18);
        difficultyButtonTexts[i]->setFillColor(sf::Color::White);
        difficultyButtonTexts[i]->setPosition({ 300.f + i * 190.f, 545.f });

        std::vector<sf::RectangleShape> gridPreview;

        int previewGridSize = gridSizes[i];
        float previewSize = 42.f;
        float cellSize = previewSize / previewGridSize;

        float gridStartX = 255.f + i * 190.f;
        float gridStartY = 535.f;

        for (int y = 0; y < previewGridSize; y++) {
            for (int x = 0; x < previewGridSize; x++) {
                sf::RectangleShape cell;
                cell.setSize({ cellSize, cellSize });
                cell.setPosition({
                    gridStartX + x * cellSize,
                    gridStartY + y * cellSize
                    });

                cell.setFillColor(sf::Color::Transparent);
                cell.setOutlineColor(sf::Color::White);
                cell.setOutlineThickness(1.f);

                gridPreview.push_back(cell);
            }
        }

        difficultyGridPreviews.push_back(gridPreview);
    }
}

void MainMenu::render(sf::RenderWindow& window, int selectedImageIndex, int gridSize) {
    if (backgroundSprite) {
        window.draw(*backgroundSprite);
    }

    sf::RectangleShape overlay;
    overlay.setSize({ 1000.f, 600.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(overlay);

    if (titleText) window.draw(*titleText);

    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

    sf::Color normalViolet(90, 60, 150);
    sf::Color hoverViolet(130, 90, 200);
    sf::Color selectedViolet(170, 120, 230);

    playButton.setFillColor(
        playButton.getGlobalBounds().contains(mousePos) ? hoverViolet : normalViolet
    );

    leaderboardButton.setFillColor(
        leaderboardButton.getGlobalBounds().contains(mousePos) ? hoverViolet : normalViolet
    );

    window.draw(playButton);
    window.draw(leaderboardButton);

    if (playButtonText) window.draw(*playButtonText);
    if (leaderboardButtonText) window.draw(*leaderboardButtonText);

    for (int i = 0; i < static_cast<int>(imageButtons.size()); i++) {

        bool isSelected = i == selectedImageIndex;
        bool isHovered = imageButtons[i].getGlobalBounds().contains(mousePos);

        imageButtons[i].setOutlineColor(
            isSelected ? sf::Color(170, 120, 230) :
            isHovered ? sf::Color(130, 90, 200) :
            sf::Color(90, 60, 150)
        );

        window.draw(imageButtons[i]);

        if (i < imagePreviewSprites.size()) {
            window.draw(imagePreviewSprites[i]);
        }
    }

    for (int i = 0; i < static_cast<int>(difficultyButtons.size()); i++) {
        int buttonGridSize = 3 + i;

        bool isSelected = buttonGridSize == gridSize;
        bool isHovered = difficultyButtons[i].getGlobalBounds().contains(mousePos);

        difficultyButtons[i].setFillColor(
            isSelected ? selectedViolet :
            isHovered ? hoverViolet :
            normalViolet
        );

        window.draw(difficultyButtons[i]);

        if (i < static_cast<int>(difficultyGridPreviews.size())) {
            for (auto& cell : difficultyGridPreviews[i]) {
                window.draw(cell);
            }
        }

        if (difficultyButtonTexts[i]) {
            window.draw(*difficultyButtonTexts[i]);
        }

        if (imageSelectText) window.draw(*imageSelectText);
        if (difficultySelectText) window.draw(*difficultySelectText);
    }
}