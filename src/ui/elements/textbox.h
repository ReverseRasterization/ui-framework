#pragma once

#include "element.h"
#include <vector>

class Textbox: public Element
{
    public:
        enum Restriction
        {
            NO_LETTERS,
            NO_UPPER_CASE,
            NO_LOWER_CASE,
            NO_NUMBERS,
            NO_SPACE,
            NO_SPECIAL_CHARACTERS
        };

        enum TextAlignment
        {
            LEFT,
            CENTER,
            RIGHT
        };

        struct Line
        {
            unsigned int startIndex;
            unsigned int endIndex;

            unsigned int getStartIndex() const {return startIndex;}
            void setStartIndex(unsigned int i) {startIndex = i;}

            unsigned int getEndIndex() const {return endIndex;}
            void setEndIndex(unsigned int i) {endIndex = i;}

            Line(unsigned int start_index, unsigned int end_index)
            :
                startIndex(start_index),
                endIndex(end_index)
            {}

            sf::FloatRect getBounds(const sf::Text& m_text);
        };

        void draw(sf::RenderWindow& window)
        {
            window.draw(m_background);

            if (highlighted)
            { 
                for (sf::RectangleShape rect : highlightRects)
                {
                    window.draw(rect);
                }
            }
                
            if (selected && !highlighted)
                window.draw(tailRect);

                for (sf::CircleShape cell : debugCells)
                {
                    window.draw(cell);
                }

            window.draw(m_text);
        }

        void setSize(sf::Vector2f new_size);
        sf::Vector2f getSize() {return m_background.getSize();}

        void setPosition(sf::Vector2f new_position);
        sf::FloatRect getGlobalBounds() {return m_background.getGlobalBounds();}

        void setBackgroundColor(sf::Color color) {m_background.setFillColor(color); m_backgroundColor = color;}
        void setTailColor(sf::Color color) {tailRect.setFillColor(color);}

        void setPlaceholderText(std::string placeholder_text);
        
        void setOutline(Outline outline);
        float getOutlineSize(){return m_outline.getThickness();};

        // setAlignment and getAlignment are here from the Elements class

        void enableMutability(int max_characters = 1000, sf::Color highlight_color = sf::Color::Cyan);
        void disableMutability();
        bool getMutable(){return isMutable;}

        void setString(std::string new_string);
        void appendString(std::string string);
        std::string getString(){return m_textContents;}

        void addRestriction(Restriction restriction) {m_restrictions.push_back(restriction);}
        void setRestrictions(std::vector<Restriction> restrictions) {m_restrictions = restrictions;}

        void handleClick(sf::Vector2f mousePos);
        void clickOff();
        void handleKey(char32_t character);

        void highlight(sf::Vector2f start_position, sf::Vector2f end_position);
        void highlight(unsigned int start_index, unsigned int end_index);

        void shiftFocus(int horizontal_direction, int vertical_direction = 0); // 1 is forward / upward, -1 is backwards/downward, 0 is neither
        
        Textbox(
            std::string text, 
            sf::Font* font, 
            sf::Vector2f size = {100.f, 25.f},
            sf::Color backgroundColor = sf::Color::White, 
            float padding_ratio = 0.04, 
            TextAlignment text_alignment = TextAlignment::CENTER,
            sf::Color fill_color = sf::Color::Black, 
            sf::Color outline_color = sf::Color::Black,
            float outline_ratio = 0.f // relative to the character size
        );

    private:

        // m_alignment is here from the Elements class
        Outline m_outline{0.f, sf::Color::Black, {0.f, 0.f}};
        float m_outlineRatio = 0.f;

        sf::RectangleShape m_background;
        std::vector<sf::RectangleShape> highlightRects;
        sf::RectangleShape tailRect; // the width is 2% of the background width

        sf::Color m_backgroundColor;
        sf::Color m_textColor;
        sf::Color m_highlightColor = sf::Color::Cyan;
        sf::Text m_text;
        sf::Font* m_font;

        std::string m_textContents;
        std::string m_placeholderText;

        std::vector<Line> m_lines;
        void updateLines();
        std::vector<Line*> getLinesByIndexRange(unsigned int i_begin, unsigned int i_end); // returns nullptr by default

        int focusPosition = 0; // basically where the text is inserted upon a keystroke

        bool isMutable = false;
        bool selected = false;

        bool highlighted = false;
        unsigned int highlight_start = 0;
        unsigned int highlight_end = 0;

        std::vector<Restriction> m_restrictions;

        float m_paddingRatio;
        TextAlignment m_textAlignment;
        int maxCharacters;

        void displayText();
        void positionText();
        void togglePlaceholder(bool toggle);
        void setTail();

        float getCharIndexFromPosition(sf::Vector2f position); // if the outcome is .5 then that means place the cursor after, otherwise, before
                                                               // returns -1.f in failure
        
        bool containsRestriction(Restriction restriction);

        std::vector<std::pair<std::string, int>> history; // the character (or string in the event of a paste) and then the index in which the content was stored

        // temp
        std::vector<sf::CircleShape> debugCells;
};