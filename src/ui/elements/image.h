#pragma once

#include "element.h"

class Image : public Element
{
    public:

        struct TileTexture
        {
            sf::Texture* t_texture;

            sf::Vector2i t_tile_position;
            sf::Vector2i t_tile_size;

            TileTexture(sf::Texture* texture, sf::Vector2i tile_position, sf::Vector2i tile_size): t_texture(texture), t_tile_position(tile_position), t_tile_size(tile_size) {}
        };

        void draw(sf::RenderWindow& window)
        {
            window.draw(i_rect);
        }

        void setSize(sf::Vector2f new_size);
        sf::Vector2f getSize() {return i_rect.getSize();}

        void setPosition(sf::Vector2f new_position) {i_rect.setPosition(new_position);}
        sf::FloatRect getGlobalBounds() {return i_rect.getGlobalBounds();}

        void setOutline(Outline outline);
        float getOutlineSize() {return m_outline.getThickness();}

        void setImage(sf::Texture* new_texture);
        void setImage(TileTexture new_texture);

        Image(sf::Vector2f size, Layout::Alignment alignment);
    private:
        sf::RectangleShape i_rect;

        Outline m_outline{0.f, sf::Color::Black, {0.f, 0.f}};

};