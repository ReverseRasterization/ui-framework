#pragma once

#include <SFML/Graphics.hpp>

#include <iostream>

#include "../outline.h"
#include "../layout.h"

class Element
{
    public:

        enum Type
        {
            T_BTN,
            T_TXBX,
            T_IMG
        };

        virtual ~Element() = default;

        // Expected to be overwritten (should be manually written in every element class)
        virtual void draw(sf::RenderWindow& window) = 0;

        virtual void setSize(sf::Vector2f new_size) = 0;
        virtual sf::Vector2f getSize() = 0;

        virtual void setPosition(sf::Vector2f new_position) = 0;
        virtual sf::FloatRect getGlobalBounds() = 0;

        virtual void setOutline(Outline outline) = 0;
        virtual float getOutlineSize() = 0;

        // Just comes with every element class, no need to worry about this

        void setAlignment(Layout::Alignment alignment) {m_alignment = alignment;};
        Layout::Alignment getAlignment() {return m_alignment;}

        void setOffset(sf::Vector2f new_offset) {m_offset = new_offset;} // offset is a percentage relative to the space size
        sf::Vector2f getOffset() {return m_offset;}

        void setCellOccupancy(unsigned int cell) {m_parentCell = cell;}
        unsigned int getCellOccupancy() {return m_parentCell;}

        void setType(Type new_type) {this->type=new_type;}
        Type getType(){return type;}

    private:

        Layout::Alignment m_alignment = Layout::Alignment::NIL_ALIGNMENT;
        Outline m_outline {0.f, sf::Color::Black, {0.f, 0.f}};

        unsigned int m_parentCell = 0;

        sf::Vector2f m_offset = {0.f, 0.f};

        Type type;
};