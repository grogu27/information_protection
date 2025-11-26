#include <iostream>
#include <SFML/Graphics.hpp>
#include "game.hpp"
#include "deck.hpp"

void drawCard(sf::RenderWindow& win, sf::Font& font, float x, float y, const CardInfo& ci) {
    // Конфигурируемые параметры
    const float CARD_WIDTH = 70.0f;
    const float CARD_HEIGHT = 100.0f;
    const float PADDING = 4.0f;
    const float CORNER_FONT_SIZE = 14.0f;
    const float CENTER_FONT_SIZE = 36.0f;
    
    // Карта: фон
    sf::RectangleShape card(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
    card.setOutlineColor(sf::Color::Black);
    card.setOutlineThickness(1);
    card.setPosition(x, y);
    win.draw(card);

    // ---- Верхний левый угол ----
    sf::Text topLeft(ci.rank + ci.suitSymbol, font, CORNER_FONT_SIZE);
    topLeft.setFillColor(ci.suitColor);
    topLeft.setPosition(x + PADDING, y + PADDING);
    win.draw(topLeft);

    // ---- Нижний правый угол ----
    sf::Text bottomRight(ci.rank + ci.suitSymbol, font, CORNER_FONT_SIZE);
    bottomRight.setFillColor(ci.suitColor);
    
    // Правильное позиционирование без вылезания за границы
    sf::FloatRect bounds = bottomRight.getLocalBounds();
    bottomRight.setOrigin(bounds.width, bounds.height);
    bottomRight.setRotation(180);
    bottomRight.setPosition(x + CARD_WIDTH - PADDING - (ci.rank.size() + 1) * CORNER_FONT_SIZE / 1.5, y + CARD_HEIGHT - PADDING - CORNER_FONT_SIZE / 1.5);
    win.draw(bottomRight);  

    // ---- Центр — символ масти ----
    sf::Text centerSuit(ci.suitSymbol, font, CENTER_FONT_SIZE);
    centerSuit.setFillColor(ci.suitColor);
    bounds = centerSuit.getLocalBounds();
    centerSuit.setOrigin(bounds.width / 2, bounds.height / 2);
    centerSuit.setPosition(x + CARD_WIDTH / 2 - 2, y + CARD_HEIGHT / 2.5);
    win.draw(centerSuit);
}

int main() {
    cpp_int p = 1019;
    int nPlayers;

    // Ввод количества игроков
    std::cout << "Enter number of players (2-6): ";
    std::cin >> nPlayers;
    
    if (nPlayers < 2 || nPlayers > 6) {
        std::cerr << "Invalid number of players. Using default: 2\n";
        nPlayers = 2;
    }

    std::cout << "Mental Poker (p = " << p << ")\n";
    Game game(p, nPlayers);
    game.deal();
    game.reveal();
    game.verify();

    sf::RenderWindow window(sf::VideoMode(1024, 900), "Mental Poker — Texas Hold'em");
    sf::Font font;
    
    if (!font.loadFromFile("segoe-ui-bold.ttf")) {
        // Fallback на системные шрифты
        std::vector<std::string> fontPaths = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
            "/usr/share/fonts/TTF/DejaVuSans-Bold.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
            "/usr/share/fonts/truetype/ubuntu/Ubuntu-Bold.ttf"
        };

        bool fontLoaded = false;
        for (auto& path : fontPaths) {
            if (font.loadFromFile(path)) {
                fontLoaded = true;
                break;
            }
        }
        
        if (!fontLoaded) {
            std::cerr << "Error: No suitable Unicode font found.\n";
            return 1;
        }
    }

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
        }

        window.clear(sf::Color(0, 100, 0));

        // --- Игроки (по 2 карты) ---
        for (size_t i = 0; i < game.players.size(); ++i) {
            for (size_t j = 0; j < game.players[i].hand_plain.size(); ++j) {
                auto info = Deck::getCardInfo(game.players[i].hand_plain[j]);
                drawCard(window, font, 100 + j * 85, 80 + i * 130, info);
            }
            // Подпись игрока
            sf::Text label("pl " + std::to_string(i + 1), font, 18);
            label.setFillColor(sf::Color::White);
            label.setPosition(50, 80 + i * 130 + 40);
            window.draw(label);
        }

        // --- Общие карты (5) ---
        for (size_t i = 0; i < game.community_plain.size(); ++i) {
            auto info = Deck::getCardInfo(game.community_plain[i]);
            drawCard(window, font, 380 + i * 85, 620, info);
        }

        // Заголовок
        sf::Text title("Texas Hold'em - Mental Poker", font, 24);
        title.setFillColor(sf::Color::White);
        title.setPosition(300, 20);
        window.draw(title);

        window.display();
    }

    return 0;
}