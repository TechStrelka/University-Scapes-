#include "MapScreen.h"

MapMarker::MapMarker(int mId, sf::Vector2f pos, const sf::Texture& tex) : id(mId) {
    sprite.setTexture(tex);
    sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
    sprite.setPosition(pos);
    sprite.setScale(0.1f, 0.1f); // Масштаб маркера
}

MapScreen::MapScreen() {
    bgTexture.loadFromFile("assets/map_empty.png");
    markerTexture.loadFromFile("assets/marker.png");
    background.setTexture(bgTexture);
    background.setScale(1000.f / bgTexture.getSize().x, 1000.f / bgTexture.getSize().y);

    // Добавление маркеров зданий
    markers.emplace_back(1, sf::Vector2f(377.f, 95.f), markerTexture);
    markers.emplace_back(6, sf::Vector2f(705.f, 553.f), markerTexture);
}

int MapScreen::handleInput(sf::Vector2i mousePos) {
    for (auto& marker : markers) {
        if (marker.sprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            highlightedBuildingId = marker.id;
            return marker.id;
        }
    }
    return -1;
}

void MapScreen::draw(sf::RenderWindow& window) const {
    window.draw(background);
    for (auto& marker : markers) {
        sf::Sprite spr = marker.sprite;
        // Подсветка выбранного маркера зеленым цветом
        spr.setColor(marker.id == highlightedBuildingId ? sf::Color::Green : sf::Color::White);
        window.draw(spr);
    }
}
