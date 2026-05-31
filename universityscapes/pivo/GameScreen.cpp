#include "GameScreen.h"

GameScreen::GameScreen(sf::Font& font, int& coins) : playerCoins(coins) {
    // 1. Фон
    if (!backgroundTexture.loadFromFile("assets/icon_background1.png")) {
        std::cerr << "Error loading background" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(1000.f / backgroundTexture.getSize().x, 1000.f / backgroundTexture.getSize().y);

    // 2. Кнопка Играть
    playBtnTex.loadFromFile("assets/buttonPlay.png");
    playBtnSprite.setTexture(playBtnTex);
    playBtnSprite.setPosition(350.f, 650.f); // Чуть ниже центра

    // 3. Логотип University Scapes
    if (logoTex.loadFromFile("assets/logo.png")) {
        logoSprite.setTexture(logoTex);
        sf::FloatRect bounds = logoSprite.getLocalBounds();
        logoSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        logoSprite.setPosition(500.f, 350.f); // По центру верхней половины
        logoSprite.setScale(0.8f, 0.8f);
    }
}

void GameScreen::draw(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
    window.draw(logoSprite);
    window.draw(playBtnSprite);
}

void GameScreen::handleInput(sf::Vector2i mousePos, AppState& currentState) {
    if (playBtnSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        currentState = AppState::Gameplay;
    }
}