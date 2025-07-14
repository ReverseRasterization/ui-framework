#pragma once

#include <SFML/Graphics.hpp>

namespace Layout
{
    enum Alignment
    {
        TOP_LEFT,
        CENTER_LEFT,
        BOTTOM_LEFT,

        TOP_CENTER,
        CENTER,
        BOTTOM_CENTER,

        TOP_RIGHT,
        CENTER_RIGHT,
        BOTTOM_RIGHT,

        NIL_ALIGNMENT
    };

    sf::Vector2f getPosInSpace(sf::Vector2f space_size, sf::Vector2f object_size, sf::Vector2f tl_pos, Alignment alignment, float outline_size=0.f);
}