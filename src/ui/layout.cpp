#include "layout.h"

sf::Vector2f Layout::getPosInSpace(sf::Vector2f space_size, sf::Vector2f object_size, sf::Vector2f tl_pos, Alignment alignment, float outline_size)
{
    sf::Vector2f new_pos = tl_pos;

    sf::Vector2f center = {((space_size.x - object_size.x)/2), ((space_size.y-object_size.y)/2)};

    float right = space_size.x - object_size.x - outline_size;
    float bottom = space_size.y - object_size.y - outline_size;

    switch (alignment)
    {
        case Alignment::TOP_LEFT:
            new_pos += {outline_size, outline_size};
            break;
        case Alignment::CENTER_LEFT:
            new_pos += {0, center.y};
            break;
        case Alignment::BOTTOM_LEFT:
            new_pos += {outline_size, bottom};
            break;

        case Alignment::TOP_CENTER:
            new_pos += {center.x, outline_size};
            break;
        case Alignment::CENTER:
            new_pos += center;
            break;
        case Alignment::BOTTOM_CENTER:
            new_pos += {center.x, bottom};
            break;
        
        case Alignment::TOP_RIGHT:
            new_pos += {right, outline_size};
            break;
        case Alignment::CENTER_RIGHT:
            new_pos += {right, center.y};
            break;
        case Alignment::BOTTOM_RIGHT:
            new_pos += {right, bottom};
    };

    return new_pos;
}