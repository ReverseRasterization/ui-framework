#include "button.h"
#include <iostream>

void Button::adjustText(bool overrideFitting)
{
    if (!btnText.has_value()){return;}

    sf::Vector2f buttonSize = m_rect.getSize();

    auto& text = *btnText->t_txt.get();

    if (!overrideFitting)
    {
        text.setCharacterSize(100);
        sf::FloatRect textBounds = text.getLocalBounds();

        float padding = btnText->t_padding;
        float scaleX = (buttonSize.x - padding * 2) / textBounds.size.x;
        float scaleY = (buttonSize.y - padding * 2) / textBounds.size.y;
        float scale = std::min(scaleX, scaleY);

        text.setCharacterSize(100 * scale);
    }

    sf::FloatRect newBounds = text.getLocalBounds();
    text.setOrigin(newBounds.getCenter());
    text.setPosition({m_rect.getPosition().x + (buttonSize.x/2), m_rect.getPosition().y + (buttonSize.y / 2)});   
}

Button::Button(sf::Vector2f size, Alignment alignment, std::optional<Text> button_text, std::optional<Texture> button_texture, sf::Color button_color)
{
    setSize(size);
    if (alignment != NIL_ALIGNMENT) {setAlignment(alignment);}

    if(button_text.has_value())
    {
        btnText = std::move(button_text);

        adjustText();
    }else if (button_texture.has_value())
    {
        Texture btnTex = button_texture.value();
        btnTexture = btnTex;

        m_rect.setTexture(btnTex.t_texture);
        m_rect.setTextureRect(sf::IntRect(button_texture->t_tile_position, button_texture->t_tile_size));
    }else {
        m_rect.setFillColor(button_color);
    }
}



void Button::setSize(sf::Vector2f new_size)
{
    m_rect.setSize(new_size);
    adjustText();
}

void Button::setPosition(sf::Vector2f new_position)
{
    m_rect.setPosition(new_position);
    adjustText(true);
}

void Button::setOutline(Outline outline)
{
    m_rect.setOutlineThickness(outline.o_thickness);
    m_rect.setOutlineColor(outline.o_color);

    m_outline = outline;
}

void Button::setText(std::string new_text)
{
    if(!btnText.has_value()){return;}

    btnText->t_txt->setString(new_text);
    
    adjustText();
}

void Button::setTexture(Texture new_texture)
{
    btnTexture = new_texture;

    m_rect.setTexture(new_texture.t_texture);
    m_rect.setTextureRect(sf::IntRect(new_texture.t_tile_position, new_texture.t_tile_size));
}