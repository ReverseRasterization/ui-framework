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
            window.draw(debugRect);
            window.draw(m_text);
        }

        void setSize(sf::Vector2f new_size);
        sf::Vector2f getSize() {return m_background.getSize();}

        void setPosition(sf::Vector2f new_position);
        sf::FloatRect getGlobalBounds() {return m_background.getGlobalBounds();}

        void setBackgroundColor(sf::Color color) {m_background.setFillColor(color); m_backgroundColor = color;}

        void setPlaceholderText(std::string placeholder_text);
        
        void setOutline(Outline outline);
        float getOutlineSize(){return m_outline.getThickness();};

        // setAlignment and getAlignment are here from the Elements class

        void toggleMutability(bool toggle) {isMutable = toggle;}
        bool isInteractive(){return isMutable;}

        void setString(std::string new_string);
        std::string getString(){return m_textContents;}

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
            float padding_ratio = 0.04, 
            sf::Color fill_color = sf::Color::Black, 
            sf::Color outline_color = sf::Color::Black,
            float outline_ratio = 0.f // relative to the character size
        );

    private:

        

        // m_alignment is here from the Elements class
        Outline m_outline{0.f, sf::Color::Black, {0.f, 0.f}};
        float m_outlineRatio = 0.f;

        sf::RectangleShape m_background;
        sf::RectangleShape debugRect;
        sf::Color m_backgroundColor;

        sf::Color m_textColor;
        sf::Text m_text;
        sf::Font* m_font;

        std::string m_textContents;
        std::string m_placeholderText;

        char tailChar = ']';
        int tailOffset = 0;

        bool isMutable = false;
        bool selected = false;

        Rule m_rule = ANY;

        float m_padding_ratio;

        void adjustTextDisplay();
        void centerText();
        void togglePlaceholder(bool toggle);
};