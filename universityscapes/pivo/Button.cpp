
#include "Button.h"

void Button::setup(sf::Vector2f pos, sf::Vector2f size, sf::Color color, const sf::String& str, sf::Font& font) {
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

bool Button::isClicked(sf::Vector2i mousePos) const {
    return rect.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(rect);
    if (!hasTexture) window.draw(text);
}
