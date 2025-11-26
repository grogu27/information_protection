#include <iostream>
#include <SFML/Graphics.hpp>
#include "game.hpp"
#include "deck.hpp"

int main() {
    // Для проверки — используем p=23, как в лекции (пример 5.1)
    cpp_int p = 1019;
    int nPlayers = 2;

    std::cout << "Mental Poker (p = " << p << ")\n";
    Game game(p, nPlayers);
    game.deal();
    game.reveal();
    game.verify();

    // --- GUI ---
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Mental Poker (p=23)");
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        std::cerr << "Font not found! Install DejaVu or adjust path.\n";
        return 1;
    }

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
        }

        window.clear(sf::Color(0, 100, 0));

        // Игроки
        for (size_t i = 0; i < game.players.size(); ++i) {
            for (size_t j = 0; j < game.players[i].hand_plain.size(); ++j) {
                sf::RectangleShape card(sf::Vector2f(70, 100));
                card.setFillColor(sf::Color::White);
                card.setOutlineColor(sf::Color::Black);
                card.setPosition(100 + j * 80, 50 + i * 120);
                window.draw(card);

                sf::Text text(Deck::cardToString(game.players[i].hand_plain[j]), font, 14);
                text.setFillColor(sf::Color::Black);
                text.setPosition(110 + j * 80, 55 + i * 120);
                window.draw(text);
            }
        }

        // Общие карты
        for (size_t i = 0; i < game.community_plain.size(); ++i) {
            sf::RectangleShape card(sf::Vector2f(70, 100));
            card.setFillColor(sf::Color(255, 255, 200));
            card.setPosition(200 + i * 80, 600);
            window.draw(card);

            sf::Text text(Deck::cardToString(game.community_plain[i]), font, 14);
            text.setPosition(205 + i * 80, 605);
            text.setFillColor(sf::Color::Black);
            window.draw(text);
        }

        window.display();
    }

    return 0;
}