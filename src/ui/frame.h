#pragma once

#include <memory>

#include <SFML/Graphics.hpp>
#include "layout.h"
#include "outline.h"

#include "elements/element.h"

class Frame
{
    public:

        void draw(sf::RenderWindow& window);

        Frame(sf::RenderWindow* window, sf::Color color = sf::Color::White);

        void setSize(sf::Vector2f new_size, bool adjustScale = true);
        void setPosition(sf::Vector2f new_pos);
        void setColor(sf::Color new_color) { m_rect.setFillColor(new_color); }

        void setOutline(Outline outline);
        void setAlignment(Layout::Alignment alignment);

        void addChild(Element* child);
        Element* getChildFromPosition(sf::Vector2f position);

        sf::Vector2f getPosition(){return m_rect.getPosition();}
        sf::FloatRect getGlobalBounds(){return m_rect.getGlobalBounds();}
        sf::Vector2f getSize(){return m_rect.getSize();}

        void onWindowResized();

        sf::RectangleShape* getRect() { return &m_rect; }

    private:

        sf::RectangleShape m_rect;
        sf::Window* m_window;

        Layout::Alignment m_alignment = Layout::Alignment::NIL_ALIGNMENT;
        Outline m_outline {0.f, sf::Color::Black, {0.f, 0.f}};
        GridLayout m_layout = GridLayout(3, 1, {0.f, 0.f}, {0.f, 0.f});

        sf::Vector2f sizeScale;
        float aspectRatio;

        std::vector<Element*> children;
};

