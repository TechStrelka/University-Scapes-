#pragma once

enum class AppState { CityMap, GameMenu, MapSelect, Gameplay, Settings };

struct UpgradeData {
    int cost;
    bool isRestored;
};

class BaseScreen {
public:
    virtual ~BaseScreen() {} // ¬иртуальный деструктор об€зателен при наследовании

    // „исто виртуальный метод Ч делает класс абстрактным
    // ¬се наследники ќЅя«јЌџ иметь свою функцию draw
    virtual void draw(sf::RenderWindow& window) const = 0;
};