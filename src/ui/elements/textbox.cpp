#include "textbox.h"
#include <algorithm>
#include <iostream>
#include <cmath>

sf::Color tuneColor(sf::Color color, float factor)
{
    auto adjust = [factor](u_int8_t c)
    {
        if (factor > 1 && c == 0) c = 50;
        return c * factor;
    };

    return sf::Color(
        adjust(color.r),
        adjust(color.g),
        adjust(color.b)
    );
}

void Textbox::togglePlaceholder(bool toggle)
{
    if (toggle)
    {
        m_text.setString(m_placeholderText);
        m_text.setFillColor(tuneColor(m_textColor, 1.8f));
    }else {
        m_text.setFillColor(m_textColor);
    }
    adjustTextDisplay();
}

void Textbox::centerText()
{
    sf::FloatRect newBounds = m_text.getLocalBounds();
    m_text.setOrigin({newBounds.position.x + newBounds.size.x / 2.f, newBounds.position.y + newBounds.size.y / 2.f});
    m_text.setPosition(m_background.getPosition() + (m_background.getSize() / 2.f));
}

void Textbox::adjustTextDisplay()
{    
    const sf::Vector2f backgroundSize = m_background.getSize();
    const float padding = backgroundSize.x * m_padding_ratio;
    const float maxFontSize = backgroundSize.y - (padding * 2);

    m_text.setCharacterSize(maxFontSize);

    /* TODO:
        1. Make text resize
        2. Make text wrap
    */



    centerText();
}

Textbox::Textbox
(
            std::string text, 
            sf::Font* font, 
            sf::Vector2f size,
            bool is_mutable, 
            Rule rule,
            sf::Color backgroundColor, 
            float padding_ratio, 
            sf::Color fill_color, 
            sf::Color outline_color,
            float outline_ratio
):           
    m_text(*font, text, 0),
    m_font(font),
    m_textContents(text),
    isMutable(is_mutable),
    m_background(size),
    m_backgroundColor(backgroundColor),
    m_textColor(fill_color),
    m_outlineRatio(outline_ratio),
    m_padding_ratio(padding_ratio),
    m_rule(rule)
{
    m_background.setFillColor(backgroundColor);
    m_text.setFillColor(fill_color);
    m_text.setOutlineColor(outline_color);
    // set outline thickness will be taken care of whenever the adjust text function is called
    
    setType(T_TXBX);

    if (!text.empty())
    {
        setString(text);
    }else {
        togglePlaceholder(true);
    }
}           

void Textbox::setOutline(Outline outline)
{
    m_background.setOutlineThickness(outline.o_thickness);
    m_background.setOutlineColor(outline.o_color);

    m_outline = outline;
}

void Textbox::setSize(sf::Vector2f new_size)
{
    m_background.setOutlineThickness(m_outline.adjust(new_size));
    m_background.setSize(new_size);
    adjustTextDisplay();
}

void Textbox::setPosition(sf::Vector2f new_position)
{
    m_background.setPosition(new_position);
    centerText();
}

void Textbox::setString(std::string new_string)
{
    m_textContents = new_string;

    selected ? m_text.setString(new_string + tailChar) : m_text.setString(new_string);
    
    if (!selected && new_string.empty())
    {
        togglePlaceholder(true);
    }
    
    adjustTextDisplay();
}

void Textbox::handleClick()
{
    if (!isMutable || selected){return;}

    togglePlaceholder(false);

    sf::Color currColor = m_background.getFillColor();
    
    selected = true;
    m_background.setFillColor(tuneColor(m_backgroundColor, .8f));

    m_text.setString(m_textContents + tailChar);
    adjustTextDisplay();
}

void Textbox::clickOff()
{
    if (!selected){return;}

    if (m_textContents.empty())
        togglePlaceholder(true);

    selected = false;
    m_background.setFillColor(m_backgroundColor);

    setString(m_textContents);
}

void Textbox::handleKey(char32_t character)
{
    if (character >= 128 || character == 13)
        return;

    if (character == 8) // backspace
    {
        if (!m_textContents.empty())
        {
            m_textContents.pop_back();
        }
    }else {
        if (m_rule == ANY)
        {
            m_textContents += character;
        }else if (m_rule == NUMBERS_ONLY)
        {
            if (character >= 48 && character <= 57)
            {
                m_textContents += character;
            }
        }else // letters only
        {
            if ((character >= 65 && character <= 90) || (character >= 97 && character <= 122))
            {
                m_textContents += character;
            }
        }
    }

    setString(m_textContents);
}

void Textbox::setPlaceholderText(std::string placeholder_text)
{
    m_placeholderText = placeholder_text;

    if (m_textContents.empty())
        togglePlaceholder(true);
}

// void Textbox::adjustTextDisplay()
// {    
//     const sf::Vector2f backgroundSize = m_background.getSize();

//     float padding = backgroundSize.x * m_padding_ratio;
    
//     const int charSize = backgroundSize.y - (padding * 2);
//     m_text.setCharacterSize(charSize);

//     const int maxWidth = backgroundSize.x - (padding * 2);
//     int currentWidth = 0;

//     std::string displayString;

//     auto calculateFittingSubstring = [&](const std::string source, bool reverse) -> std::string {
//         if (reverse) // means the textbox is selected
//         {
//             for (int i = source.size() - 1; i >= 0; --i)
//             {
//                 int advance = m_font->getGlyph(m_textContents[i], m_text.getCharacterSize(), false, m_text.getOutlineThickness()).advance;
            
//                 if (currentWidth + advance > maxWidth)
//                 {
//                     return source.substr(i + 1);
//                 }

//                 currentWidth += advance;
//             }

//             return source + tailChar;
//         }else { // the textbox is not selected
//             for (int i = 0; i < source.size(); i++)
//             {
//                 int advance = m_font->getGlyph(source[i], m_text.getCharacterSize(), false, m_text.getOutlineThickness()).advance;

//                 if (currentWidth + advance > maxWidth)
//                 {
//                     return source.substr(0, i);
//                 }

//                 currentWidth += advance;
//             }

//             return source;
//         }

//         return "";
//     };

//     if (selected)
//     {
//         displayString = calculateFittingSubstring(m_textContents, true);
//     }else {
//         const std::string& base = (m_textContents.empty() && !m_placeholderText.empty()) 
//                                   ? m_placeholderText 
//                                   : m_textContents;
//         displayString = calculateFittingSubstring(base, false);
//     }

//     m_text.setString(displayString);
//     centerText();
// }