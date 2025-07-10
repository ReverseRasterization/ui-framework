#pragma once

#include "element.h"

#include <functional>

class Button : public Element
{
    public:
        struct Text
        {
            std::unique_ptr<sf::Text> t_txt;
            unsigned int t_padding;

            Text(std::string text, sf::Font* font, unsigned int padding=4, sf::Color text_color=sf::Color::Black, sf::Color outline_color=sf::Color::Black)
            {
                sf::Text nText(*font, text, 100); // character size is only set to 100 for now, the setSize() function should take good care of it >:)
                nText.setFillColor(text_color);
                nText.setOutlineColor(outline_color);

                t_txt = std::make_unique<sf::Text>(nText);
                t_padding = padding;
            }
        };

        struct Texture
        {
            sf::Texture* t_texture;

            sf::Vector2i t_tile_position;
            sf::Vector2i t_tile_size;

            Texture(sf::Texture* texture, sf::Vector2i tile_position, sf::Vector2i tile_size): t_texture(texture), t_tile_position(tile_position), t_tile_size(tile_size) {}
        };

        void draw(sf::RenderWindow& window)
        {
            window.draw(m_rect);
        
            if (btnText.has_value())
            {
                window.draw(*btnText->t_txt.get());
            }
            
        }

        Button(
            sf::Vector2f size = {50.f, 50.f}, 
            Alignment alignment = NIL_ALIGNMENT, 
            std::optional<Text> button_text = std::nullopt, 
            std::optional<Texture> button_texture = std::nullopt, 
            sf::Color button_color = sf::Color::White
        );

        void setPosition(sf::Vector2f new_position);
        sf::FloatRect getGlobalBounds(){return m_rect.getGlobalBounds();}

        void setSize(sf::Vector2f new_size);
        sf::Vector2f getSize(){return m_rect.getSize();}

        void setOutline(Outline outline);
        float getOutlineSize() {return m_outline.o_thickness;}

        void setText(std::string new_text);
        void setTexture(Texture new_texture);
        
        void toggle(bool enabled) {active = enabled;}
        bool isActive() {return active;} 

        std::function<void()> onClick;

        Alignment getAlignment() {return m_alignment;}

    private:
        sf::RectangleShape m_rect;
        std::optional<Text> btnText;
        sf::Font* font;

        std::optional<Texture> btnTexture;

        Alignment m_alignment = NIL_ALIGNMENT;
        Outline m_outline {0.f, sf::Color::Black};

        void adjustText(bool overrideFitting = false);

        bool active = true;
};
