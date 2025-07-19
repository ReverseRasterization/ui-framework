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

            float t_outlineRatio;

            Text(std::string text, sf::Font* font, unsigned int padding=4, sf::Color text_color=sf::Color::Black, sf::Color outline_color=sf::Color::Transparent, float outline_ratio = 0.f);

            void setCharSize(unsigned int new_size);
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
            Layout::Alignment alignment = Layout::Alignment::NIL_ALIGNMENT, 
            std::optional<Text> button_text = std::nullopt, 
            std::optional<Texture> button_texture = std::nullopt, 
            sf::Color button_color = sf::Color::White
        );

        void setPosition(sf::Vector2f new_position);
        sf::FloatRect getGlobalBounds(){return m_rect.getGlobalBounds();}

        void setSize(sf::Vector2f new_size);
        sf::Vector2f getSize(){return m_rect.getSize();}

        void setOutline(Outline outline);
        float getOutlineSize() {return m_outline.getThickness();}

        void setText(std::string new_text);
        void setTexture(Texture new_texture);
        void setBackgroundColor(sf::Color color) {m_rect.setFillColor(color);}
        
        void toggle(bool enabled) {active = enabled;}
        bool isActive() {return active;} 

        std::function<void()> onClick;

        Layout::Alignment getAlignment() {return m_alignment;}

    private:
        sf::RectangleShape m_rect;
        std::optional<Text> btnText;
        sf::Font* font;

        std::optional<Texture> btnTexture;

        Layout::Alignment m_alignment = Layout::Alignment::NIL_ALIGNMENT;
        Outline m_outline {0.f, sf::Color::Black, {0.f, 0.f}};

        void adjustText(bool overrideFitting = false);

        bool active = true;
};
