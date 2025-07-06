#pragma once

#include "element.h"

class Textbox: public Element
{
    public:
        void draw(sf::RenderWindow& window)
        {
            window.draw(m_background);
            window.draw(m_text);
        }

        void setSize(sf::Vector2f new_size);
        sf::Vector2f getSize() {m_background.getSize();}

        void setPosition(sf::Vector2f new_position);

        void setOutline(Outline outline);
        float getOutlineSize(){return m_outline.o_thickness;};

        // setAlignment and getAlignment are here from the Elements class

        void toggleMutability(bool toggle) {isMutable = true;}

        Textbox(
            std::string text, 
            sf::Font* font, 
            bool is_mutable, 
            std::optional<sf::Color> backgroundColor = sf::Color::White, 
            unsigned int padding = 4, 
            sf::Color fill_color = sf::Color::Black, 
            sf::Color outline_color = sf::Color::Black
        );

    private:

        // m_alignment is here from the Elements class
        Outline m_outline;

        sf::RectangleShape m_background;
        sf::Text m_text;

        std::string m_text_contents;

        bool isMutable = false;

        void adjustText(bool overrideFitting = false);
};