#include "CityUpgradeScreen.h"
#include <iostream>
#include <algorithm>

CityUpgradeScreen::CityUpgradeScreen(sf::Font& font, int& coins) : playerCoins(coins) {
    // Загрузка общих ресурсов интерфейса
    texBtnRuined.loadFromFile("assets/icon_mini_upgrade_destroyed.png");
    texBtnRestored.loadFromFile("assets/icon_mini_upgrade.png");
    texCoin.loadFromFile("assets/icon_coin.png");

    background.setSize({ 1000.f, 1000.f });

    // Настройка плашки с монетами
    globalCoinPlate.setSize({ 350.f, 55.f });
    globalCoinPlate.setPosition(600.f, 20.f);
    globalCoinPlate.setFillColor(sf::Color(0, 51, 153, 220));

    globalCoinSprite.setTexture(texCoin);
    globalCoinSprite.setScale(0.08f, 0.08f);

    globalCoinText.setFont(font);
    globalCoinText.setCharacterSize(24);
    globalCoinText.setFillColor(sf::Color::Yellow);

    // Инициализация 4-х кнопок улучшений
    for (int i = 0; i < 4; ++i) {
        UpgradeNode node;
        node.button.setSize({ 180.f, 180.f });
        node.button.setPosition({ 60.f + i * 230.f, 650.f });
        node.button.setOutlineThickness(3.f);

        node.pricePlate.setSize({ 180.f, 40.f });
        node.pricePlate.setPosition(node.button.getPosition().x, 840.f);

        node.priceText.setFont(font);
        node.priceText.setCharacterSize(18);
        node.priceText.setFillColor(sf::Color::White);

        node.coinIcon.setTexture(texCoin);
        node.coinIcon.setScale(0.06f, 0.06f);
        node.coinIcon.setPosition(node.pricePlate.getPosition().x + 15.f, 845.f);
        upgrades.push_back(node);
    }
}

void CityUpgradeScreen::loadBuildingData(int buildingId, std::vector<UpgradeData>& data) {
    currentBuildingUpgrades = &data;
    currentBuildingId = buildingId;

    // Динамическая загрузка стадий для выбранного здания
    buildingStages.clear();
    buildingStages.resize(4);
    std::string prefix = "assets/building_" + std::to_string(buildingId);
    for (int i = 0; i < 4; ++i) {
        buildingStages[i].loadFromFile(prefix + "_stage_" + std::to_string(i) + ".png");
    }

    for (size_t i = 0; i < upgrades.size(); ++i) {
        if (i < currentBuildingUpgrades->size()) {
            upgrades[i].cost = (*currentBuildingUpgrades)[i].cost;
        }
    }
    updateVisuals();
}

void CityUpgradeScreen::updateVisuals() {
    if (!currentBuildingUpgrades) return;

    int restoredCount = 0;
    globalCoinText.setString(L"Монеты: " + std::to_wstring(playerCoins));
    globalCoinText.setPosition(670.f, 32.f);
    globalCoinSprite.setPosition(620.f, 28.f);

    for (size_t i = 0; i < upgrades.size(); ++i) {
        if (i >= currentBuildingUpgrades->size()) continue;

        upgrades[i].isRestored = (*currentBuildingUpgrades)[i].isRestored;
        if (upgrades[i].isRestored) restoredCount++;

        // Улучшение доступно, если оно первое или предыдущее уже куплено
        bool isAvailable = (i == 0 || (*currentBuildingUpgrades)[i - 1].isRestored);

        if (upgrades[i].isRestored) {
            upgrades[i].button.setTexture(&texBtnRestored);
            upgrades[i].button.setFillColor(sf::Color::White);
            upgrades[i].pricePlate.setFillColor(sf::Color(0, 150, 0, 220));
            upgrades[i].priceText.setString(L"Куплено");
            upgrades[i].priceText.setPosition(upgrades[i].pricePlate.getPosition().x + 50.f, 848.f);
        }
        else if (isAvailable) {
            upgrades[i].button.setTexture(&texBtnRuined);
            upgrades[i].button.setFillColor(sf::Color::White);
            upgrades[i].pricePlate.setFillColor(sf::Color(40, 40, 40, 200));
            upgrades[i].priceText.setString(std::to_string(upgrades[i].cost));
            upgrades[i].priceText.setPosition(upgrades[i].pricePlate.getPosition().x + 60.f, 848.f);
        }
        else {
            upgrades[i].button.setTexture(&texBtnRuined);
            upgrades[i].button.setFillColor(sf::Color(100, 100, 100));
            upgrades[i].pricePlate.setFillColor(sf::Color(100, 0, 0, 200));
            upgrades[i].priceText.setString(L"Блок");
            upgrades[i].priceText.setPosition(upgrades[i].pricePlate.getPosition().x + 65.f, 848.f);
        }
    }

    // Установка фона в зависимости от количества купленных улучшений
    int textureIndex = std::min(restoredCount, 3);
    background.setTexture(&buildingStages[textureIndex], true);
}

void CityUpgradeScreen::handleInput(sf::Vector2i mousePos) {
    if (!currentBuildingUpgrades) return;

    for (size_t i = 0; i < currentBuildingUpgrades->size(); ++i) {
        // Проверка нажатия на некупленное и доступное улучшение
        if (!(*currentBuildingUpgrades)[i].isRestored && upgrades[i].button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            if (i > 0 && !(*currentBuildingUpgrades)[i - 1].isRestored) continue;

            if (playerCoins >= upgrades[i].cost) {
                playerCoins -= upgrades[i].cost;
                (*currentBuildingUpgrades)[i].isRestored = true;
                updateVisuals();
            }
        }
    }
}

void CityUpgradeScreen::draw(sf::RenderWindow& window) const {
    auto* nonConstThis = const_cast<CityUpgradeScreen*>(this);
    nonConstThis->globalCoinText.setString(std::to_string(playerCoins));

    window.draw(background);
    window.draw(globalCoinPlate);
    window.draw(globalCoinSprite);
    window.draw(globalCoinText);
    for (size_t i = 0; i < upgrades.size(); ++i) {
        if (currentBuildingUpgrades && i < currentBuildingUpgrades->size()) {
            window.draw(upgrades[i].button);
            window.draw(upgrades[i].pricePlate);
            window.draw(upgrades[i].priceText);
            // Иконка монеты рисуется только если кнопка доступна для покупки
            if (!upgrades[i].isRestored && (i == 0 || (*currentBuildingUpgrades)[i - 1].isRestored)) {
                window.draw(upgrades[i].coinIcon);
            }
        }
    }
}