#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Global.h"

// Структура маркера на карте
struct MapMarker {
    int id;
    sf::Sprite sprite;

    MapMarker(int mId, sf::Vector2f pos, const sf::Texture& tex);
};

class MapScreen : public BaseScreen {
private:
    sf::Sprite background;
    sf::Texture bgTexture, markerTexture;
    std::vector<MapMarker> markers;
public:
    int highlightedBuildingId = -1;

    MapScreen();
    // Возвращает ID выбранного здания или -1
    int handleInput(sf::Vector2i mousePos);
    void draw(sf::RenderWindow& window) const;
};