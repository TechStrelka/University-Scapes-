#pragma once
#include <SFML/Graphics.hpp>

class Button {
public:
    sf::RectangleShape rect;
    sf::Text text;
    bool hasTexture = false;

    void setup(sf::Vector2f pos, sf::Vector2f size, sf::Color color, const sf::String& str, sf::Font& font);
    bool isClicked(sf::Vector2i mousePos) const;
    void draw(sf::RenderWindow& window) const;
};