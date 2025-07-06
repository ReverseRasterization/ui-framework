#include <iostream>

#include <SFML/Graphics.hpp>

#include "ui/frame.h"
#include "ui/button.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 800}), "UI Framework Test");

    sf::Font font;
    if (!font.openFromFile("assets/font.ttf"))
    {
        std::cerr << "Failed to load font!\n";
        return -1;
    }

    sf::Texture buttonSet;
    if(!buttonSet.loadFromFile("assets/uicons.png"))
    {
        std::cerr << "Failed to load uicons tileset!";
        return -1;
    }

    Frame frame(&window);
    frame.setSize({500.f, 300.f});
    frame.setOutline(Outline(10.f, sf::Color::Blue));
    frame.setAlignment(CENTER);

    Button* button = new Button({100.f, 100.f}, NIL_ALIGNMENT, std::nullopt, Button::Texture(&buttonSet, {64, 0}, {32, 32}));
    button->setOutline(Outline(5.f, sf::Color::Black));
    button->setAlignment(CENTER);

    frame.addChild(button);

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

    delete button;

    return 0;
}