#include <SFML/Graphics.hpp>
#include <optional>

int main() {
    // En SFML 3, VideoMode ahora recibe un sf::Vector2u
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Beyota Engine Test");

    // Loop principal
    while (window.isOpen()) {
        // pollEvent() devuelve std::optional<sf::Event>
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Limpia la pantalla (azul oscuro)
        window.clear(sf::Color(30, 30, 80));
        window.display();
    }

    return 0;
}
