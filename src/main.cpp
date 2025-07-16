#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>

#include "ui/frame.h"
#include "ui/layout.h"

int main()
{
    

    sf::RenderWindow window(sf::VideoMode({800, 800}), "UI Framework Test");

    Frame frame(&window);
    frame.setSize({500.f, 300.f});
    frame.setOutline(Outline(10.f, sf::Color::Blue, {500.f, 300.f}));
    frame.setAlignment(Layout::Alignment::CENTER);

    GridLayout nLayout(3, 3, frame.getPosition(), frame.getSize());

    std::vector<sf::RectangleShape> rects;
    
    for (GridLayout::Cell& cell : *nLayout.getCells())
    {   
        sf::Vector2f pos = cell.getPosition();
        sf::Vector2f size = cell.getSize();

        sf::RectangleShape cellRect(size);
        cellRect.setPosition(pos);
        cellRect.setFillColor(sf::Color::Transparent);
        cellRect.setOutlineColor(sf::Color::Blue);
        cellRect.setOutlineThickness(1.f);

        rects.push_back(cellRect);
    }

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
            {
                nLayout.setBounds(frame.getPosition(), frame.getSize()/2.f);

                rects = {};
                for (GridLayout::Cell& cell : *nLayout.getCells())
                {   
                    sf::Vector2f pos = cell.getPosition();
                    sf::Vector2f size = cell.getSize();

                    sf::RectangleShape cellRect(size);
                    cellRect.setPosition(pos);
                    cellRect.setFillColor(sf::Color::Transparent);
                    cellRect.setOutlineColor(sf::Color::Blue);
                    cellRect.setOutlineThickness(1.f);

                    rects.push_back(cellRect);
                }
            }
        }

        window.clear();

        frame.draw(window);
        
        for (sf::RectangleShape& rect: rects)
        {
            window.draw(rect);
        }

        window.display();
    }

    

    return 0;
}