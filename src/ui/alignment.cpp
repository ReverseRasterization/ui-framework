#include "alignment.h"

sf::Vector2f getPosInSpace(sf::Vector2f space_size, sf::Vector2f object_size, sf::Vector2f tl_pos, Alignment alignment, float outline_size)
{
    sf::Vector2f new_pos;

    sf::Vector2f center = {(space_size.x- object_size.x)/2, (space_size.y-object_size.y)/2};
    float right = space_size.x - object_size.x - outline_size;
    float bottom = space_size.y - object_size.y - outline_size;

    switch (alignment)
    {
        case TOP_LEFT:
            new_pos = tl_pos + sf::Vector2f{outline_size, outline_size};
            break;
        case CENTER_LEFT:
            new_pos = {tl_pos.x, center.y};
            break;
        case BOTTOM_LEFT:
            new_pos = {tl_pos.x + outline_size, bottom};
            break;

        case TOP_CENTER:
            new_pos = {center.x, tl_pos.y + outline_size};
            break;
        case CENTER:
            new_pos = center;
            break;
        case BOTTOM_CENTER:
            new_pos = {center.x, bottom};
            break;
        
        case TOP_RIGHT:
            new_pos = {right, tl_pos.y + outline_size};
            break;
        case CENTER_RIGHT:
            new_pos = {right, center.y};
            break;
        case BOTTOM_RIGHT:
            new_pos = {right, bottom};
    };

    return new_pos;
}