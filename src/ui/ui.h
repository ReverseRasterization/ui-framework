#pragma once

#include <iostream>
#include <vector>
#include <variant>

#include <SFML/Graphics.hpp>
#include "alignment.h"

struct Outline
{
    float o_thickness;
    sf::Color o_color;

    Outline(float thickness, sf::Color color): o_thickness(thickness), o_color(color){}
};

class Frame
{
    public:

        void draw(sf::RenderWindow& window)
        {
            window.draw(m_rect);
        }

        Frame(sf::RenderWindow* window, sf::Color color = sf::Color::White);

        void setSize(sf::Vector2f new_size);
        void setPosition(sf::Vector2f new_pos);
        void setColor(sf::Color new_color) { m_rect.setFillColor(new_color); }

        void setOutline(Outline outline);
        void setAlignment(Alignment alignment);

        sf::Vector2f getPosition(){return m_rect.getPosition();}
        sf::Vector2f getSize(){return m_rect.getSize();}

        void onWindowResized();

        sf::RectangleShape* getRect() { return &m_rect; }

    private:

        sf::RectangleShape m_rect;
        sf::Window* m_window;

        Alignment m_alignment = NIL_ALIGNMENT;
        Outline m_outline {0.f, sf::Color::Black};

        sf::Vector2f sizeScale;
        float outlineScale;
        float aspectRatio;
};

class Button
{
    public:

        void draw(sf::RenderWindow& window)
        {
            window.draw(m_rect);
        }

        Button(Frame& parent): m_parent(parent){}

        // TODO: setSize() function

        void setOutline(Outline outline);
        void setAlignment(Alignment alignment);

    private:
        sf::RectangleShape m_rect;

        Alignment m_alignment = NIL_ALIGNMENT;
        Outline m_outline {0.f, sf::Color::Black};

        Frame& m_parent;
};

