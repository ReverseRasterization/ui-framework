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
        m_text.setFillColor(tuneColor(m_textColor, 1.8f));
        m_text.setString(m_placeholderText);
        adjustText();
    }else {
        m_text.setFillColor(m_textColor);
        m_text.setString(m_textContents);
        adjustText();
    }
}


void Textbox::adjustText(bool overrideFitting)
{
    /*
        Character size in SFML is the vertical space the glyphs are expected to occupy

        Glyph Properties:
        
            advance: how much to move the pen horizontally after drawing the glyph. ** Most accurate for calculating the bounds of the whole text
            bounds: the bounding box of the glyph.
            textureRect: the region in the font texture.
    */

    // sf::Vector2f backgroundSize = m_background.getSize();

    
    // sf::FloatRect textBounds = m_text.getGlobalBounds();

    // sf::Vector2f t_tl_corner = textBounds.position;
    // sf::Vector2f t_br_corner = textBounds.position + textBounds.size;

    // sf::Vector2f b_tl_corner = m_background.getPosition();
    // sf::Vector2f b_br_corner = m_background.getPosition() + m_background.getSize();

    // if (t_tl_corner.x < b_tl_corner.x ||
    //     t_br_corner.x > b_br_corner.x)
    // {
    //     debugRect.setFillColor(sf::Color::Red);
    // }else {
    //     debugRect.setFillColor(sf::Color::Green);
    // }

    // int totalAdvance = 0;
    // for (char c : m_text.getString())
    // {
    //     totalAdvance += m_font->getGlyph(c, m_text.getCharacterSize(), false, m_outline.getThickness()).advance;
    // }

    // float scale = m_background.getSize().x / totalAdvance;
    // m_text.setCharacterSize(scale * 100);


    // std::cout << "Width: " << totalAdvance << "\n     Bound Width: " << textBounds.size.x << "\n";
 

    

    // debugRect.setSize({textBounds.size.x, textBounds.size.y});
    // debugRect.setPosition(textBounds.position);

    sf::Vector2f backgroundSize = m_background.getSize();

    if (!overrideFitting)
    {
        // Set base size and outline FIRST
        m_text.setCharacterSize(100);
        m_text.setOutlineThickness(100 * m_outlineRatio);

        sf::FloatRect textBounds = m_text.getLocalBounds();

        float scaleX = (backgroundSize.x - m_padding * 2) / textBounds.size.x;
        float scaleY = (backgroundSize.y - m_padding * 2) / textBounds.size.y;
        float scale = std::min(scaleX, scaleY);

        float nSize = std::clamp(100 * scale, 1.1f, 256.f);

        m_text.setCharacterSize(nSize);
        m_text.setOutlineThickness(nSize * m_outlineRatio);
    }

    // Centering
    sf::FloatRect newBounds = m_text.getLocalBounds();
    m_text.setOrigin({newBounds.position.x + newBounds.size.x / 2.f, newBounds.position.y + newBounds.size.y / 2.f});
    m_text.setPosition(m_background.getPosition() + (backgroundSize / 2.f));

    // Debug rect
    newBounds = m_text.getGlobalBounds();
    debugRect.setSize({newBounds.size.x, newBounds.size.y});
    debugRect.setPosition({newBounds.position.x, newBounds.position.y});
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
    m_padding(padding),
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
    adjustText();
}

void Textbox::setPosition(sf::Vector2f new_position)
{
    m_background.setPosition(new_position);
    adjustText(true);
}

void Textbox::setString(std::string new_string)
{
    m_textContents = new_string;

    selected ? m_text.setString(new_string + tailChar) : m_text.setString(new_string);
    

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
    m_background.setFillColor(tuneColor(m_backgroundColor, .8f));

    m_text.setString(m_textContents + tailChar);
    adjustText();
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

    std::cout << m_font->getGlyph(character, m_text.getCharacterSize(), false, m_outline.getThickness()).advance << '\n';

    setString(m_textContents);
}

void Textbox::setPlaceholderText(std::string placeholder_text)
{
    m_placeholderText = placeholder_text;

    if (m_textContents.empty())
        togglePlaceholder(true);
}