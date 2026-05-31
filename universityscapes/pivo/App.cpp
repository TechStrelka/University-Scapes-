#include "App.h"
#include <fstream> // Для работы с файлами (исправляет ошибки ofstream/ifstream)
#include <iostream>

App::App() {
    // 1. Получаем разрешение монитора пользователя
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    // 2. Вычисляем безопасный размер для квадратного окна.
    // Берем 80% от ВЫСОТЫ экрана, чтобы окно точно поместилось на маленьких экранах ноутбуков.
    unsigned int windowSize = static_cast<unsigned int>(desktop.height * 0.8f);

    // Если монитор огромный (например, 4K), ограничиваем окно размером 1000x1000,
    // чтобы текстуры не растягивались слишком сильно.
    if (windowSize > 1000) {
        windowSize = 1000;
    }

    // 3. Создаем обычное окно (с рамкой и кнопкой закрытия), но строго квадратное
    window.create(sf::VideoMode(windowSize, windowSize), "University scapes", sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60);

    // 4. Закрепляем виртуальное разрешение 1000x1000.
    // Теперь для всего твоего кода мир ВСЕГДА равен 1000x1000.
    sf::View view(sf::FloatRect(0.f, 0.f, 1000.f, 1000.f));
    window.setView(view);

    if (!mainFont.loadFromFile("assets/font.ttf")) {
        std::cerr << "Font not found!" << std::endl;
    }

    // Инициализация данных по умолчанию
    globalBuildingStates[1] = { {50000, false}, {100000, false}, {150000, false}, {200000, false} };
    globalBuildingStates[6] = { {250000, false}, {500000, false}, {750000, false}, {1000000, false} };

    // Загружаем сохраненный прогресс
    loadProgress();

    cityScreen = new CityUpgradeScreen(mainFont, globalPlayerCoins);
    gameScreen = new GameScreen(mainFont, globalPlayerCoins);
    match3Screen = new Match3Screen(mainFont, globalPlayerCoins);

    cityScreen->loadBuildingData(selectedBuildingIndex, globalBuildingStates[selectedBuildingIndex]);
    mapScreen.highlightedBuildingId = selectedBuildingIndex;

    // Настройка табов и кнопок
    tabCity.setup({ 0.f, 900.f }, { 334.f, 100.f }, sf::Color(40, 40, 40), L"Улучшения", mainFont);
    tabGame.setup({ 334.f, 900.f }, { 333.f, 100.f }, sf::Color(40, 40, 40), L"Меню", mainFont);
    tabMap.setup({ 667.f, 900.f }, { 333.f, 100.f }, sf::Color(40, 40, 40), L"Карта", mainFont);

    btnSettings.setup({ 930.f, 20.f }, { 50.f, 50.f }, sf::Color(100, 100, 100), "S", mainFont);
    btnSaveExit.setup({ 350.f, 350.f }, { 300.f, 60.f }, sf::Color(70, 70, 70), L"Сохранить и выйти", mainFont);
    btnReset.setup({ 350.f, 450.f }, { 300.f, 60.f }, sf::Color(150, 50, 50), L"Сбросить", mainFont);
    btnBackSettings.setup({ 350.f, 550.f }, { 300.f, 60.f }, sf::Color(100, 100, 100), L"Назад", mainFont);

    activeTabIndicator.setSize({ 333.f, 5.f });
    activeTabIndicator.setFillColor(sf::Color::Yellow);
    updateTabs();
}

App::~App() {
    // Освобождаем память, которую выделяли через new
    delete cityScreen;
    delete gameScreen;
    delete match3Screen;
}

void App::saveProgress() {
    std::ofstream file("save.txt");
    if (file.is_open()) {
        file << globalPlayerCoins << "\n";

        // Переписываем цикл без использования [id, upgrades] для совместимости стандартов C++
        for (auto it = globalBuildingStates.begin(); it != globalBuildingStates.end(); ++it) {
            int id = it->first;
            const std::vector<UpgradeData>& upgrades = it->second;

            file << id << " " << upgrades.size() << " ";
            for (size_t i = 0; i < upgrades.size(); ++i) {
                file << upgrades[i].isRestored << " ";
            }
            file << "\n";
        }
        file.close();
    }
}

void App::loadProgress() {
    std::ifstream file("save.txt");
    if (file.is_open()) {
        if (!(file >> globalPlayerCoins)) return;

        int id, size;
        while (file >> id >> size) {
            if (globalBuildingStates.count(id)) {
                for (int i = 0; i < size; ++i) {
                    bool restored;
                    if (file >> restored) {
                        if (i < (int)globalBuildingStates[id].size()) {
                            globalBuildingStates[id][i].isRestored = restored;
                        }
                    }
                }
            }
        }
        file.close();
    }
}

void App::updateTabs() {
    float posX = 0.f;
    if (currentState == AppState::GameMenu) posX = 334.f;
    else if (currentState == AppState::MapSelect) posX = 667.f;

    activeTabIndicator.setPosition(posX, 900.f);
}

void App::handleInput(sf::Vector2i mPos) {
    if (currentState == AppState::Settings) {
        if (btnSaveExit.isClicked(mPos)) { saveProgress(); window.close(); }
        if (btnReset.isClicked(mPos)) {
            globalPlayerCoins = 3000000;
            for (auto& pair : globalBuildingStates) {
                for (auto& upg : pair.second) upg.isRestored = false;
            }
            saveProgress();
            cityScreen->loadBuildingData(selectedBuildingIndex, globalBuildingStates[selectedBuildingIndex]);
            currentState = AppState::GameMenu;
        }
        if (btnBackSettings.isClicked(mPos)) currentState = AppState::GameMenu;
        return;
    }

    if (currentState != AppState::Gameplay && btnSettings.isClicked(mPos)) {
        currentState = AppState::Settings;
        return;
    }

    if (currentState != AppState::Gameplay) {
        if (tabCity.isClicked(mPos)) { currentState = AppState::CityMap; updateTabs(); }
        else if (tabGame.isClicked(mPos)) { currentState = AppState::GameMenu; updateTabs(); }
        else if (tabMap.isClicked(mPos)) { currentState = AppState::MapSelect; updateTabs(); }
    }

    if (currentState == AppState::CityMap) cityScreen->handleInput(mPos);
    else if (currentState == AppState::GameMenu) gameScreen->handleInput(mPos, currentState);
    else if (currentState == AppState::MapSelect) {
        int id = mapScreen.handleInput(mPos);
        if (id != -1) {
            selectedBuildingIndex = id;
            cityScreen->loadBuildingData(id, globalBuildingStates[id]);
            currentState = AppState::CityMap;
            updateTabs();
        }
    }
    else if (currentState == AppState::Gameplay) match3Screen->handleInput(mPos, currentState);
}

void App::render() {
    window.clear(sf::Color(20, 20, 20));

    if (currentState == AppState::CityMap) cityScreen->draw(window);
    else if (currentState == AppState::GameMenu) gameScreen->draw(window);
    else if (currentState == AppState::MapSelect) mapScreen.draw(window);
    else if (currentState == AppState::Gameplay) match3Screen->draw(window);

    if (currentState == AppState::Settings) {
        sf::RectangleShape overlay({ 1000.f, 1000.f });
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(overlay);
        btnSaveExit.draw(window);
        btnReset.draw(window);
        btnBackSettings.draw(window);
    }
    else if (currentState != AppState::Gameplay) {
        tabCity.draw(window);
        tabGame.draw(window);
        tabMap.draw(window);
        window.draw(activeTabIndicator);
        btnSettings.draw(window);
    }
    window.display();
}

void App::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { saveProgress(); window.close(); }

            if (event.type == sf::Event::MouseButtonPressed) {
                // Переводим реальные пиксели окна в координаты виртуального экрана 1000x1000
                sf::Vector2f virtualMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::Vector2i mPos(static_cast<int>(virtualMousePos.x), static_cast<int>(virtualMousePos.y));

                handleInput(mPos);
            }
        }
        render();
    }
}