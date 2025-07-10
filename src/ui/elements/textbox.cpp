#include "textbox.h"
#include <algorithm>
#include <iostream>

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

        m_text.setCharacterSize(100 * scale);
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
            sf::Color backgroundColor, 
            unsigned int padding, 
            sf::Color fill_color, 
            sf::Color outline_color,
            float outline_thickness
):           
    m_text(*font, text, 0),
    isMutable(is_mutable),
    m_background(size),
    m_padding(padding)
{
    adjustText();

    m_background.setFillColor(backgroundColor);
    m_text.setFillColor(fill_color);
    m_text.setOutlineColor(outline_color);
    m_text.setOutlineThickness(outline_thickness);

    setType(T_TXBX);
}           

void Textbox::setOutline(Outline outline)
{
    m_background.setOutlineThickness(outline.o_thickness);
    m_background.setOutlineColor(outline.o_color);

    m_outline = outline;
}

void Textbox::setSize(sf::Vector2f new_size)
{
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

    adjustText();
}

void Textbox::handleClick()
{
    if (!isMutable || selected){return;}

    sf::Color currColor = m_background.getFillColor();
    
    selected = true;

    std::cout << "Textbox Active\n";
}

void Textbox::clickOff()
{
    if (!selected){return;}

    selected = false;

    std::cout << "Textbox not active\n";
}