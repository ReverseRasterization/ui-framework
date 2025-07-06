#include <iostream>

#include <SFML/Graphics.hpp>
#include "ui/ui.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 800}), "UI Framework Test");

    Frame frame(&window);
    frame.setSize({500.f, 300.f});
    frame.setOutline(Outline(10.f, sf::Color::Blue));
    frame.setAlignment(CENTER);

    Button button(frame);
    button.setAlignment(CENTER);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
                window.setView(sf::View(visibleArea));

                frame.onWindowResized();
            }
        }

        window.clear();

        frame.draw(window);

        window.display();
    }

    return 0;
}