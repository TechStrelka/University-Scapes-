#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include "Global.h"
#include "Button.h"
#include "CityUpgradeScreen.h"
#include "MapScreen.h"
#include "GameScreen.h"
#include "Match3Screen.h"

class App {
private:
    sf::RenderWindow window;
    sf::Font mainFont;
    AppState currentState = AppState::GameMenu;

    // Ёти переменные должны быть объ€влены здесь!
    int globalPlayerCoins = 3000000;
    int selectedBuildingIndex = 6;
    std::map<int, std::vector<UpgradeData>> globalBuildingStates;

    Button tabCity, tabGame, tabMap;
    sf::RectangleShape activeTabIndicator;

    //  нопки настроек
    Button btnSettings;
    Button btnSaveExit;
    Button btnReset;
    Button btnBackSettings;

    CityUpgradeScreen* cityScreen;
    GameScreen* gameScreen;
    MapScreen mapScreen;
    Match3Screen* match3Screen;

    void handleInput(sf::Vector2i mPos);
    void updateTabs();
    void render();

    // ћетоды дл€ работы с файлами
    void saveProgress();
    void loadProgress();

public:
    App();
    ~App();
    void run();
};