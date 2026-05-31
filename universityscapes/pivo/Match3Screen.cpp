#include "Match3Screen.h"
#include <iostream>
#include <cmath>

std::wstring gemNames[6] = { L"", L"Калькулятор", L"Процессор", L"Сервер", L"Прибор", L"Инструмент" };

Match3Screen::Match3Screen(sf::Font& font, int& coins) : playerCoins(coins) {
    for (int i = 1; i <= 5; ++i) {
        sf::Texture tex;
        if (!tex.loadFromFile("assets/gem_" + std::to_string(i) + ".png")) {
            std::cerr << "Error: Could not load gem_" << i << ".png\n";
        }
        gemTextures.push_back(tex);
    }

    selectionRect.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    selectionRect.setFillColor(sf::Color::Transparent);
    selectionRect.setOutlineColor(sf::Color::Yellow);
    selectionRect.setOutlineThickness(3.f);

    // Текст интерфейса
    uiText.setFont(font);
    uiText.setCharacterSize(22);
    uiText.setFillColor(sf::Color::White);
    uiText.setPosition(20.f, 20.f);

    goalText.setFont(font);
    goalText.setCharacterSize(22);
    goalText.setFillColor(sf::Color::Yellow);
    goalText.setPosition(20.f, 100.f);

    // Настройка экрана победы
    overlay.setSize({ 1000.f, 1000.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 180));

    panel.setSize({ 400.f, 250.f });
    panel.setPosition(300.f, 350.f);
    panel.setFillColor(sf::Color(30, 30, 80));
    panel.setOutlineThickness(4.f);
    panel.setOutlineColor(sf::Color::White);

    victoryText.setFont(font);
    victoryText.setCharacterSize(28);
    victoryText.setFillColor(sf::Color::White);
    victoryText.setPosition(330.f, 380.f);

    // Кнопка возврата в меню на панели победы
    backButton.setSize({ 200.f, 60.f });
    backButton.setPosition(400.f, 500.f);
    backButton.setFillColor(sf::Color(0, 150, 0));

    backBtnText.setFont(font);
    backBtnText.setCharacterSize(24);
    backBtnText.setFillColor(sf::Color::White);
    backBtnText.setString(L"В меню");

    sf::FloatRect textBounds = backBtnText.getLocalBounds();
    backBtnText.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
    backBtnText.setPosition(400.f + 100.f, 500.f + 30.f);

    resetLevel();
}

void Match3Screen::resetLevel() {
    for (int i = 0; i < 6; ++i) {
        targetGems[i] = 0;
        collectedGems[i] = 0;
    }

    int numQuests = (rand() % 3) + 1;
    int questsCreated = 0;
    while (questsCreated < numQuests) {
        int randomGemType = (rand() % 5) + 1;
        if (targetGems[randomGemType] == 0) {
            targetGems[randomGemType] = (rand() % 16) + 10;
            questsCreated++;
        }
    }

    currentState = Match3State::Playing;

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            grid[r][c] = (rand() % 5) + 1;
        }
    }

    resolveBoard();

    score = 0;
    for (int i = 0; i < 6; ++i) {
        collectedGems[i] = 0;
    }
}

bool Match3Screen::checkMatches() {
    bool matchFound = false;
    bool toDelete[ROWS][COLS] = { false };

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS - 2; ++c) {
            int type = grid[r][c];
            if (type == 0) continue;
            if (grid[r][c + 1] == type && grid[r][c + 2] == type) {
                toDelete[r][c] = toDelete[r][c + 1] = toDelete[r][c + 2] = true;
                matchFound = true;
            }
        }
    }

    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r < ROWS - 2; ++r) {
            int type = grid[r][c];
            if (type == 0) continue;
            if (grid[r + 1][c] == type && grid[r + 2][c] == type) {
                toDelete[r][c] = toDelete[r + 1][c] = toDelete[r + 2][c] = true;
                matchFound = true;
            }
        }
    }

    if (matchFound) {
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                if (toDelete[r][c]) {
                    int type = grid[r][c];
                    collectedGems[type]++;
                    grid[r][c] = 0;
                    score += 10;
                }
            }
        }
    }
    return matchFound;
}

void Match3Screen::applyGravity() {
    for (int c = 0; c < COLS; ++c) {
        for (int r = ROWS - 1; r >= 0; --r) {
            if (grid[r][c] == 0) {
                for (int k = r - 1; k >= 0; --k) {
                    if (grid[k][c] != 0) {
                        grid[r][c] = grid[k][c];
                        grid[k][c] = 0;
                        break;
                    }
                }
            }
        }
    }
}

void Match3Screen::fillEmpty() {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (grid[r][c] == 0) {
                grid[r][c] = (rand() % 5) + 1;
            }
        }
    }
}

void Match3Screen::resolveBoard() {
    while (checkMatches()) {
        applyGravity();
        fillEmpty();
    }
}

bool Match3Screen::isLevelComplete() const {
    for (int i = 1; i <= 5; ++i) {
        if (collectedGems[i] < targetGems[i]) {
            return false;
        }
    }
    return true;
}

void Match3Screen::handleInput(sf::Vector2i mousePos, AppState& globalState) {
    if (currentState == Match3State::LevelComplete) {
        if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            globalState = AppState::GameMenu;
            resetLevel();
        }
        return;
    }

    if (mousePos.x >= OFFSET_X && mousePos.x < OFFSET_X + COLS * TILE_SIZE &&
        mousePos.y >= OFFSET_Y && mousePos.y < OFFSET_Y + ROWS * TILE_SIZE) {

        int c = (mousePos.x - OFFSET_X) / TILE_SIZE;
        int r = (mousePos.y - OFFSET_Y) / TILE_SIZE;

        if (selectedRow == -1) {
            selectedRow = r;
            selectedCol = c;
        }
        else {
            if (std::abs(selectedRow - r) + std::abs(selectedCol - c) == 1) {
                std::swap(grid[selectedRow][selectedCol], grid[r][c]);

                if (!checkMatches()) {
                    std::swap(grid[selectedRow][selectedCol], grid[r][c]);
                }
                else {
                    applyGravity();
                    fillEmpty();
                    resolveBoard();

                    if (isLevelComplete()) {
                        currentState = Match3State::LevelComplete;
                        playerCoins += rewardCoins;
                        std::cout << "Victory! Added " << rewardCoins << " coins." << std::endl;
                    }
                }
            }
            selectedRow = -1;
            selectedCol = -1;
        }
    }
}

void Match3Screen::draw(sf::RenderWindow& window) const {
    // 1. Очки
    sf::Text txt = uiText;
    txt.setString(L"Очки: " + std::to_wstring(score));
    window.draw(txt);

    // 2. Цели уровня
    std::wstring goalsStr = L"НУЖНО СОБРАТЬ:\n";
    for (int i = 1; i <= 5; ++i) {
        if (targetGems[i] > 0) {
            int collected = std::min(collectedGems[i], targetGems[i]);
            goalsStr += gemNames[i] + L": " + std::to_wstring(collected) + L" / " + std::to_wstring(targetGems[i]) + L"\n";
        }
    }
    sf::Text gText = goalText;
    gText.setString(goalsStr);
    window.draw(gText);

    // 3. Игровое поле
    sf::Sprite sprite;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            int type = grid[r][c];
            if (type > 0 && type <= (int)gemTextures.size()) {
                sprite.setTexture(gemTextures[type - 1]);
                sprite.setScale(
                    (float)TILE_SIZE / sprite.getTexture()->getSize().x,
                    (float)TILE_SIZE / sprite.getTexture()->getSize().y
                );
                sprite.setPosition(OFFSET_X + c * TILE_SIZE, OFFSET_Y + r * TILE_SIZE);
                window.draw(sprite);
            }
        }
    }

    // Рамка выделения кубика
    if (selectedRow != -1 && selectedCol != -1) {
        sf::RectangleShape rect = selectionRect;
        rect.setPosition(OFFSET_X + selectedCol * TILE_SIZE, OFFSET_Y + selectedRow * TILE_SIZE);
        window.draw(rect);
    }

    // 4. Экран ПОБЕДЫ (Исправлен порядок отрисовки слоев и удалены дубли)
    if (currentState == Match3State::LevelComplete) {
        window.draw(overlay);
        window.draw(panel);

        sf::Text vText = victoryText;
        vText.setString(L"УРОВЕНЬ ПРОЙДЕН!\n\nВы заработали:\n" + std::to_wstring(rewardCoins) + L" монет");
        window.draw(vText);

        window.draw(backButton);
        window.draw(backBtnText);
    }
}