#include "textbox.h"
#include <algorithm>
#include <iostream>

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
        m_text.setFillColor(tuneColor(m_text_color, 1.8f));
        m_text.setString(m_placeholder_text);
        adjustText();
    }else {
        m_text.setFillColor(m_text_color);
        m_text.setString(m_text_contents);
        adjustText();
    }
}

void Textbox::adjustText(bool overrideFitting)
{
    sf::Vector2f backgroundSize = m_background.getSize();

    if (!overrideFitting)
    {
        m_text.setCharacterSize(100);
        sf::FloatRect textBounds = m_text.getLocalBounds();

        float scaleX = (backgroundSize.x - m_padding * 2) / textBounds.size.x;
        float scaleY = (backgroundSize.y - m_padding * 2) / textBounds.size.y;
        float scale = std::min(scaleX, scaleY);

        m_text.setCharacterSize(std::clamp(100 * scale, 1.1f, 256.f));
    }

    sf::FloatRect newBounds = m_text.getLocalBounds();
    m_text.setOrigin(newBounds.getCenter());
    m_text.setPosition({m_background.getPosition().x + (backgroundSize.x/2), m_background.getPosition().y + (backgroundSize.y / 2)});   
}

Textbox::Textbox
(
            std::string text, 
            sf::Font* font, 
            sf::Vector2f size,
            bool is_mutable, 
            Rule rule,
            sf::Color backgroundColor, 
            unsigned int padding, 
            sf::Color fill_color, 
            sf::Color outline_color,
            float outline_thickness
):           
    m_text(*font, text, 0),
    m_text_contents(text),
    isMutable(is_mutable),
    m_background(size),
    m_background_color(backgroundColor),
    m_text_color(fill_color),
    m_padding(padding),
    m_rule(rule)
{
    m_background.setFillColor(backgroundColor);
    m_text.setFillColor(fill_color);
    m_text.setOutlineColor(outline_color);
    m_text.setOutlineThickness(outline_thickness);
    
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
    adjustText();
}

void Textbox::setPosition(sf::Vector2f new_position)
{
    m_background.setPosition(new_position);
    adjustText(true);
}

void Textbox::setString(std::string new_string)
{
    m_text_contents = new_string;
    m_text.setString(new_string);

    if (!selected && new_string.empty())
    {
        togglePlaceholder(true);
    }
    
    adjustText();
}

void Textbox::handleClick()
{
    if (!isMutable || selected){return;}

    togglePlaceholder(false);

    sf::Color currColor = m_background.getFillColor();
    
    selected = true;
    m_background.setFillColor(tuneColor(m_background_color, .8f));
}

void Textbox::clickOff()
{
    if (!selected){return;}

    if (m_text_contents.empty())
        togglePlaceholder(true);

    selected = false;
    m_background.setFillColor(m_background_color);
}

void Textbox::handleKey(char32_t character)
{
    if (character >= 128 || character == 13)
        return;

    if (character == 8) // backspace
    {
        if (!m_text_contents.empty())
        {
            m_text_contents.pop_back();
        }
    }else {
        if (m_rule == ANY)
        {
            m_text_contents += character;
        }else if (m_rule == NUMBERS_ONLY)
        {
            if (character >= 48 && character <= 57)
            {
                m_text_contents += character;
            }
        }else // letters only
        {
            if ((character >= 65 && character <= 90) || (character >= 97 && character <= 122))
            {
                m_text_contents += character;
            }
        }
    }

    setString(m_text_contents);
}

void Textbox::setPlaceholderText(std::string placeholder_text)
{
    m_placeholder_text = placeholder_text;

    if (m_text_contents.empty())
        togglePlaceholder(true);
}