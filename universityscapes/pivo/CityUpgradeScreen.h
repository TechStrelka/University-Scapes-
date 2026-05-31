#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Global.h" // Файл с AppState и UpgradeData

// Узел улучшения на экране (визуальная часть кнопки)
struct UpgradeNode {
    sf::RectangleShape button, pricePlate;
    sf::Text priceText;
    sf::Sprite coinIcon;
    bool isRestored = false;
    int cost = 0;
};

class CityUpgradeScreen : public BaseScreen {
private:
    std::vector<UpgradeData>* currentBuildingUpgrades = nullptr;
    int currentBuildingId = -1;

    std::vector<sf::Texture> buildingStages;
    sf::Texture texBtnRuined, texBtnRestored, texCoin;

    sf::RectangleShape background, globalCoinPlate;
    sf::Text globalCoinText;
    sf::Sprite globalCoinSprite;
    std::vector<UpgradeNode> upgrades;
    int& playerCoins;

public:
    CityUpgradeScreen(sf::Font& font, int& coins);

    // Загрузка данных конкретного здания
    void loadBuildingData(int buildingId, std::vector<UpgradeData>& data);

    // Обновление визуального состояния (текстуры, цены, доступность)
    void updateVisuals();

    // Обработка кликов по кнопкам улучшений
    void handleInput(sf::Vector2i mousePos);

    // Отрисовка всех элементов экрана
    void draw(sf::RenderWindow& window) const;
};