#include "image.h"

Image::Image(sf::Vector2f size, Layout::Alignment alignment)
{
    setSize(size);
    if (alignment != Layout::Alignment::NIL_ALIGNMENT) {setAlignment(alignment);}

    setType(Element::T_IMG);
}

void Image::setSize(sf::Vector2f new_size)
{
    i_rect.setOutlineThickness(m_outline.adjust(new_size));
    i_rect.setSize(new_size);
}

void Image::setOutline(Outline new_outline)
{
    i_rect.setOutlineColor(new_outline.getColor());
    i_rect.setOutlineThickness(new_outline.getThickness());
}

void Image::setImage(sf::Texture* new_texture)
{
    i_rect.setTexture(new_texture);
}

void Image::setImage(TileTexture new_texture)
{
    i_rect.setTexture(new_texture.t_texture);
    i_rect.setTextureRect(sf::IntRect(new_texture.t_tile_position, new_texture.t_tile_size));
}