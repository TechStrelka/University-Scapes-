#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <map>

// Состояния приложения, добавлено Gameplay для будущей механики
enum class AppState { CityMap, GameMenu, MapSelect, Gameplay };

// Универсальный класс кнопки
class Button {
public:
    sf::RectangleShape rect;
    sf::Text text;
    bool hasTexture = false;

    void setup(sf::Vector2f pos, sf::Vector2f size, sf::Color color, const sf::String& str, sf::Font& font) {
        rect.setPosition(pos);
        rect.setSize(size);
        rect.setFillColor(color);
        rect.setOutlineThickness(2.f);
        rect.setOutlineColor(sf::Color::Black);

        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(22);
        text.setFillColor(sf::Color::White);

        sf::FloatRect bounds = text.getLocalBounds();
        text.setPosition(pos.x + (size.x - bounds.width) / 2.f, pos.y + (size.y - bounds.height) / 2.f - 5.f);
    }

    bool isClicked(sf::Vector2i mousePos) const {
        return rect.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(rect);
        if (!hasTexture) window.draw(text);
    }
};

// --- ЭКРАН УЛУЧШЕНИЙ ЗДАНИЙ ---
struct UpgradeNode {
    sf::RectangleShape button, pricePlate;
    sf::Text priceText;
    sf::Sprite coinIcon;
    bool isRestored = false;
    int cost = 250000;
};

class CityUpgradeScreen {
private:
    std::vector<bool>* currentBuildingState = nullptr;
    int currentBuildingId = -1;
    sf::Texture texBgRuined, texBgRestored, texBtnRuined, texBtnRestored, texCoin;
    sf::RectangleShape background, globalCoinPlate;
    sf::Text globalCoinText;
    sf::Sprite globalCoinSprite;
    std::vector<UpgradeNode> upgrades;
    sf::Font* mainFont;
    int& playerCoins;

public:
    CityUpgradeScreen(sf::Font& font, int& coins) : mainFont(&font), playerCoins(coins) {
        texBtnRuined.loadFromFile("assets/icon_mini_upgrade_destroyed.png");
        texBtnRestored.loadFromFile("assets/icon_mini_upgrade.png");
        texCoin.loadFromFile("assets/icon_coin.png");

        background.setSize({ 1000.f, 1000.f });

        globalCoinPlate.setSize({ 350.f, 55.f });
        globalCoinPlate.setPosition(600.f, 20.f);
        globalCoinPlate.setFillColor(sf::Color(0, 51, 153, 220));

        globalCoinSprite.setTexture(texCoin);
        globalCoinSprite.setScale(0.08f, 0.08f);

        globalCoinText.setFont(font);
        globalCoinText.setCharacterSize(24);
        globalCoinText.setFillColor(sf::Color::Yellow);

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

    void loadBuildingData(int buildingId, std::vector<bool>& state) {
        currentBuildingState = &state;
        currentBuildingId = buildingId;
        std::string prefix = "assets/building_" + std::to_string(buildingId);
        texBgRuined.loadFromFile(prefix + "_destroyed.png");
        texBgRestored.loadFromFile(prefix + ".png");
        updateVisuals();
    }

    void updateVisuals() {
        bool allDone = true;
        globalCoinText.setString(L"Монеты: " + std::to_wstring(playerCoins));
        globalCoinText.setPosition(670.f, 32.f);
        globalCoinSprite.setPosition(620.f, 28.f);

        for (size_t i = 0; i < upgrades.size(); ++i) {
            upgrades[i].isRestored = (*currentBuildingState)[i];
            // Логика зависимости: доступно ли текущее улучшение
            bool isAvailable = (i == 0 || upgrades[i - 1].isRestored);

            if (upgrades[i].isRestored) {
                upgrades[i].button.setTexture(&texBtnRestored);
                upgrades[i].button.setFillColor(sf::Color::White);
                upgrades[i].pricePlate.setFillColor(sf::Color(0, 150, 0, 220));
                upgrades[i].priceText.setString(L"Куплено");
                upgrades[i].priceText.setPosition(upgrades[i].pricePlate.getPosition().x + 50.f, 848.f);
            }
            else if (isAvailable) {
                allDone = false;
                upgrades[i].button.setTexture(&texBtnRuined);
                upgrades[i].button.setFillColor(sf::Color::White);
                upgrades[i].pricePlate.setFillColor(sf::Color(40, 40, 40, 200));
                upgrades[i].priceText.setString(std::to_string(upgrades[i].cost));
                upgrades[i].priceText.setPosition(upgrades[i].pricePlate.getPosition().x + 60.f, 848.f);
            }
            else {
                allDone = false;
                upgrades[i].button.setTexture(&texBtnRuined);
                upgrades[i].button.setFillColor(sf::Color(100, 100, 100)); // Затеняем недоступное
                upgrades[i].pricePlate.setFillColor(sf::Color(100, 0, 0, 200)); // Красная плашка
                upgrades[i].priceText.setString(L"Блок");
                upgrades[i].priceText.setPosition(upgrades[i].pricePlate.getPosition().x + 65.f, 848.f);
            }
        }
        background.setTexture(allDone ? &texBgRestored : &texBgRuined, true);
    }

    void handleInput(sf::Vector2i mousePos) {
        for (size_t i = 0; i < upgrades.size(); ++i) {
            if (!upgrades[i].isRestored && upgrades[i].button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                // Проверка зависимости: нельзя купить, если предыдущее не восстановлено
                if (i > 0 && !upgrades[i - 1].isRestored) continue;

                if (playerCoins >= upgrades[i].cost) {
                    playerCoins -= upgrades[i].cost;
                    (*currentBuildingState)[i] = true;
                    updateVisuals();
                }
            }
        }
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(background);
        window.draw(globalCoinPlate);
        window.draw(globalCoinSprite);
        window.draw(globalCoinText);
        for (size_t i = 0; i < upgrades.size(); ++i) {
            window.draw(upgrades[i].button);
            window.draw(upgrades[i].pricePlate);
            window.draw(upgrades[i].priceText);
            if (!upgrades[i].isRestored && (i == 0 || upgrades[i - 1].isRestored)) {
                window.draw(upgrades[i].coinIcon); // Монетка только на доступных для покупки
            }
        }
    }
};

// --- ЭКРАН КАРТЫ (УПРОЩЕННЫЙ) ---
struct MapMarker {
    int id;
    sf::Sprite sprite;
    bool isExpanded = false;

    MapMarker(int mId, sf::Vector2f pos, const sf::Texture& tex) : id(mId) {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(pos);
        sprite.setScale(0.1f, 0.1f);
    }
};

class MapScreen {
private:
    sf::Sprite background;
    sf::Texture bgTexture, markerTexture;
    std::vector<MapMarker> markers;
public:
    int highlightedBuildingId = -1;

    MapScreen() {
        bgTexture.loadFromFile("assets/map_empty.png");
        markerTexture.loadFromFile("assets/marker.png");
        background.setTexture(bgTexture);
        background.setScale(1000.f / bgTexture.getSize().x, 1000.f / bgTexture.getSize().y);

        markers.emplace_back(1, sf::Vector2f(377.f, 95.f), markerTexture);
        markers.emplace_back(6, sf::Vector2f(705.f, 553.f), markerTexture);
    }

    int handleInput(sf::Vector2i mousePos) {
        for (auto& marker : markers) {
            if (marker.sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                highlightedBuildingId = marker.id;
                return marker.id;
            }
        }
        return -1;
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(background);
        for (auto& marker : markers) {
            sf::Sprite spr = marker.sprite;
            spr.setColor(marker.id == highlightedBuildingId ? sf::Color::Green : sf::Color::White);
            window.draw(spr);
        }
    }
};

// --- ЭКРАН МЕНЮ ИГРЫ ---
class GameScreen {
private:
    int& playerCoins;
    Button battleButton;
    sf::Text coinsText;

public:
    GameScreen(sf::Font& font, int& coins) : playerCoins(coins) {
        battleButton.setup({ 350.f, 600.f }, { 300.f, 100.f }, sf::Color(255, 140, 0), L"Играть", font);
        coinsText.setFont(font);
        coinsText.setCharacterSize(24);
        coinsText.setPosition(20.f, 20.f);
    }

    void handleInput(sf::Vector2i mousePos, AppState& currentState) {
        // Кнопка просто переводит в состояние геймплея
        if (battleButton.isClicked(mousePos)) {
            currentState = AppState::Gameplay;
        }
    }

    void draw(sf::RenderWindow& window) {
        coinsText.setString(L"Монеты: " + std::to_wstring(playerCoins));
        window.draw(coinsText);
        battleButton.draw(window);
    }
};

// --- ГЛАВНЫЙ КЛАСС APP ---
class App {
private:
    sf::RenderWindow window;
    sf::Font mainFont;
    AppState currentState = AppState::GameMenu;
    int globalPlayerCoins = 3000000;
    int selectedBuildingIndex = 6;

    std::map<int, std::vector<bool>> globalBuildingStates;
    Button tabCity, tabGame, tabMap;
    sf::RectangleShape activeTabIndicator;

    CityUpgradeScreen* cityScreen;
    GameScreen* gameScreen;
    MapScreen mapScreen;

    sf::Text gameplayPlaceholderText; // Заглушка для будущей игры

public:
    App() : window(sf::VideoMode(1000, 1000), "University scapes") {
        window.setFramerateLimit(60);
        mainFont.loadFromFile("assets/font.ttf");

        globalBuildingStates[1] = std::vector<bool>(4, false);
        globalBuildingStates[6] = std::vector<bool>(4, false);

        cityScreen = new CityUpgradeScreen(mainFont, globalPlayerCoins);
        gameScreen = new GameScreen(mainFont, globalPlayerCoins);
        cityScreen->loadBuildingData(selectedBuildingIndex, globalBuildingStates[selectedBuildingIndex]);
        mapScreen.highlightedBuildingId = selectedBuildingIndex;

        tabCity.setup({ 0.f, 900.f }, { 334.f, 100.f }, sf::Color(40, 40, 40), L"Улучшения", mainFont);
        tabGame.setup({ 334.f, 900.f }, { 333.f, 100.f }, sf::Color(40, 40, 40), L"Меню", mainFont);
        tabMap.setup({ 667.f, 900.f }, { 333.f, 100.f }, sf::Color(40, 40, 40), L"Карта", mainFont);

        activeTabIndicator.setSize({ 333.f, 5.f });
        activeTabIndicator.setFillColor(sf::Color::Yellow);

        gameplayPlaceholderText.setFont(mainFont);
        gameplayPlaceholderText.setString(L"Здесь будет игра...\nНажмите ESC для выхода в меню.");
        gameplayPlaceholderText.setPosition(300.f, 450.f);
        gameplayPlaceholderText.setCharacterSize(30);

        updateTabs();
    }

    ~App() {
        delete cityScreen;
        delete gameScreen;
    }

    void run() {
        while (window.isOpen()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed) window.close();
                if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                    handleInput(sf::Mouse::getPosition(window));
                }
                // Выход из режима игры обратно в меню
                if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape && currentState == AppState::Gameplay) {
                    currentState = AppState::GameMenu;
                    updateTabs();
                }
            }
            render();
        }
    }

private:
    void handleInput(sf::Vector2i mPos) {
        if (currentState != AppState::Gameplay) {
            if (tabCity.isClicked(mPos)) { currentState = AppState::CityMap; updateTabs(); }
            else if (tabGame.isClicked(mPos)) { currentState = AppState::GameMenu; updateTabs(); }
            else if (tabMap.isClicked(mPos)) { currentState = AppState::MapSelect; updateTabs(); }
        }

        if (currentState == AppState::CityMap) cityScreen->handleInput(mPos);
        else if (currentState == AppState::GameMenu) gameScreen->handleInput(mPos, currentState);
        else if (currentState == AppState::MapSelect) {
            int clickedId = mapScreen.handleInput(mPos);
            if (clickedId != -1) {
                selectedBuildingIndex = clickedId;
                currentState = AppState::CityMap;
                cityScreen->loadBuildingData(selectedBuildingIndex, globalBuildingStates[selectedBuildingIndex]);
                updateTabs();
            }
        }
    }

    void updateTabs() {
        tabCity.rect.setFillColor(currentState == AppState::CityMap ? sf::Color(60, 60, 60) : sf::Color(30, 30, 30));
        tabGame.rect.setFillColor(currentState == AppState::GameMenu ? sf::Color(60, 60, 60) : sf::Color(30, 30, 30));
        tabMap.rect.setFillColor(currentState == AppState::MapSelect ? sf::Color(60, 60, 60) : sf::Color(30, 30, 30));

        if (currentState == AppState::CityMap) activeTabIndicator.setPosition(0.f, 895.f);
        else if (currentState == AppState::GameMenu) activeTabIndicator.setPosition(334.f, 895.f);
        else if (currentState == AppState::MapSelect) activeTabIndicator.setPosition(667.f, 895.f);
    }

    void render() {
        window.clear(sf::Color(20, 20, 20));

        if (currentState == AppState::CityMap) cityScreen->draw(window);
        else if (currentState == AppState::GameMenu) gameScreen->draw(window);
        else if (currentState == AppState::MapSelect) mapScreen.draw(window);
        else if (currentState == AppState::Gameplay) window.draw(gameplayPlaceholderText); // Отрисовка будущей игры

        if (currentState != AppState::Gameplay) {
            tabCity.draw(window);
            tabGame.draw(window);
            tabMap.draw(window);
            window.draw(activeTabIndicator);
        }

        window.display();
    }
};

int main() {
    App game;
    game.run();
    return 0;
}