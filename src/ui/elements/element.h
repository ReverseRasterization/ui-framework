#pragma once

#include <SFML/Graphics.hpp>

#include <iostream>

#include "../alignment.h"
#include "../outline.h"

class Element
{
    public:

        virtual ~Element() = default;

        virtual void draw(sf::RenderWindow& window) = 0;

        virtual void setSize(sf::Vector2f new_size) = 0;
        virtual sf::Vector2f getSize() = 0;

        virtual void setPosition(sf::Vector2f new_position) = 0;

        virtual void setOutline(Outline outline) = 0;
        virtual float getOutlineSize() = 0;
        
        void setAlignment(Alignment alignment) {m_alignment = alignment;};
        Alignment getAlignment() {return m_alignment;}

    private:

        Alignment m_alignment = NIL_ALIGNMENT;
        Outline m_outline {0.f, sf::Color::Black};


};