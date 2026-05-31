#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <string>
#include "Global.h"

// Состояния самого экрана Match-3
enum class Match3State { Playing, LevelComplete };

class Match3Screen : public BaseScreen {
private:
    static const int ROWS = 8;
    static const int COLS = 8;
    static const int TILE_SIZE = 64;
    static const int OFFSET_X = 244;
    static const int OFFSET_Y = 244;

    int grid[ROWS][COLS];
    std::vector<sf::Texture> gemTextures;
    sf::RectangleShape selectionRect;

    int selectedRow = -1;
    int selectedCol = -1;

    int score = 0;
    int rewardCoins = 50000;
    int& playerCoins;

    // --- Новые поля для целей и состояний ---
    Match3State currentState = Match3State::Playing;

    int targetGems[6];    // Сколько нужно собрать (индексы 1-5 для типов кубиков)
    int collectedGems[6]; // Сколько уже собрали

    sf::Text uiText;
    sf::Text goalText;

    // Элементы экрана победы
    sf::RectangleShape overlay;
    sf::RectangleShape panel;
    sf::RectangleShape backButton;
    sf::Text victoryText;
    sf::Text backBtnText;

    bool checkMatches();
    void applyGravity();
    void fillEmpty();
    void resolveBoard();
    bool isLevelComplete() const; // Проверка достижения целей

public:
    Match3Screen(sf::Font& font, int& coins);
    void resetLevel(); // Инициализация уровня и сброс целей
    void handleInput(sf::Vector2i mousePos, AppState& globalState);
    void draw(sf::RenderWindow& window) const;
};