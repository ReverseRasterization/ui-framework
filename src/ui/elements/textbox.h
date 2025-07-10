#pragma once

#include "element.h"

class Textbox: public Element
{
    public:
        enum Rule
        {
            NUMBERS_ONLY,
            LETTERS_ONLY,
            ANY
        };

        void draw(sf::RenderWindow& window)
        {
            window.draw(m_background);
            window.draw(m_text);
        }

        void setSize(sf::Vector2f new_size);
        sf::Vector2f getSize() {return m_background.getSize();}

        void setPosition(sf::Vector2f new_position);
        sf::FloatRect getGlobalBounds() {return m_background.getGlobalBounds();}

        void setBackgroundColor(sf::Color color) {m_background.setFillColor(color); m_background_color = color;}

        void setOutline(Outline outline);
        float getOutlineSize(){return m_outline.o_thickness;};

        // setAlignment and getAlignment are here from the Elements class

        void toggleMutability(bool toggle) {isMutable = toggle;}
        bool isInteractive(){return isMutable;}

        void setString(std::string new_string);
        std::string getString(){return m_text_contents;}

        void setRule(Rule new_rule) {m_rule = new_rule;}

        void handleClick();
        void clickOff();
        void handleKey(char32_t character);
        
        Textbox(
            std::string text, 
            sf::Font* font, 
            sf::Vector2f size = {100.f, 25.f},
            bool is_mutable = false,
            Rule rule = ANY,
            sf::Color backgroundColor = sf::Color::White, 
            unsigned int padding = 4, 
            sf::Color fill_color = sf::Color::Black, 
            sf::Color outline_color = sf::Color::Black,
            float outline_thickness = 0.f
        );

    private:

        // m_alignment is here from the Elements class
        Outline m_outline{0.f, sf::Color::Black};

        sf::RectangleShape m_background;
        sf::Color m_background_color;
        sf::Text m_text;


        std::string m_text_contents;

        bool isMutable = false;
        bool selected = false;

        Rule m_rule = ANY;

        float m_padding;

        void adjustText(bool overrideFitting = false);
};