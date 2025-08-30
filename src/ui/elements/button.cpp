#include "button.h"
#include <iostream>
#include <algorithm>

/*

    TEXT OBJECT

*/


Button::Text::Text(std::string text, sf::Font* font, unsigned int padding, sf::Color text_color, sf::Color outline_color, float outline_ratio):
    t_outlineRatio(outline_ratio)
{
    sf::Text nText(*font, text, 100); // character size is only set to 100 for now, the setSize() function should take good care of it >:)
    nText.setFillColor(text_color);

    if (outline_color != sf::Color::Transparent)
    {
       // outline thickness is not set yet, as the setSize function will take care of that
        nText.setOutlineColor(outline_color);
    }

    t_txt = std::make_unique<sf::Text>(nText);
    t_padding = padding;
}

void Button::Text::setCharSize(unsigned int new_size)
{
    t_txt->setCharacterSize(new_size);

    if (t_outlineRatio > 0.f)
    {
        t_txt->setOutlineThickness((float) new_size * t_outlineRatio);
    }
}

/*

    BUTTON OBJECT

*/

void Button::adjustText(bool overrideFitting)
{
    if (!btnText.has_value()){return;}

    sf::Vector2f buttonSize = m_rect.getSize();

    auto& text = *btnText->t_txt.get();

    if (!overrideFitting)
    {
        text.setCharacterSize(100); // not gonna use the setCharSize function because this is just to set a benchmark
        sf::FloatRect textBounds = text.getLocalBounds();

        float padding = btnText->t_padding;
        float scaleX = (buttonSize.x - padding * 2) / textBounds.size.x;
        float scaleY = (buttonSize.y - padding * 2) / textBounds.size.y;
        float scale = std::min(scaleX, scaleY);

        btnText->setCharSize(std::clamp(100 * scale, 1.f, 256.f));
    }

    sf::FloatRect newBounds = text.getLocalBounds();
    text.setOrigin(newBounds.getCenter());
    text.setPosition({m_rect.getPosition().x + (buttonSize.x/2), m_rect.getPosition().y + (buttonSize.y / 2)});   
}

Button::Button(sf::Vector2f size, Layout::Alignment alignment, std::optional<Text> button_text, std::optional<Texture> button_texture, sf::Color button_color)
{
    setSize(size);
    if (alignment != Layout::Alignment::NIL_ALIGNMENT) {setAlignment(alignment);}

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

    setType(T_BTN);
}

void Button::setSize(sf::Vector2f new_size)
{
    m_rect.setOutlineThickness(m_outline.adjust(new_size));
    m_rect.setSize(new_size);
    
    adjustText();
}

void Button::setPosition(sf::Vector2f new_position)
{
    m_rect.setPosition(new_position);
    adjustText(true); // just so that way the text can catch up with the button
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