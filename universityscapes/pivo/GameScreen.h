#pragma once
#include <SFML/Graphics.hpp>
#include "Global.h"
#include <iostream>

class GameScreen {
private:
    int& playerCoins;
    sf::Text coinsText;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Texture playBtnTex;
    sf::Sprite playBtnSprite;

    // Логотип игры
    sf::Texture logoTex;
    sf::Sprite logoSprite;

public:
    GameScreen(sf::Font& font, int& coins);
    // Теперь это обычный метод, не виртуальный
    void draw(sf::RenderWindow& window);
    void handleInput(sf::Vector2i mousePos, AppState& currentState);
};