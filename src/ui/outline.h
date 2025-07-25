#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>

struct Outline
{
    float o_thickness;
    sf::Color o_color;
    float ratio = 0.f;

    Outline(float thickness, sf::Color color, sf::Vector2f rectangle_size): o_thickness(thickness), o_color(color)
    {
        if (thickness > 0.f)
        {
            ratio = thickness / std::min(rectangle_size.x, rectangle_size.y);
        }
    }

    float adjust(sf::Vector2f new_size) {
        if (ratio <= 0.f ){return 0.f;}
        o_thickness = ratio * std::min(new_size.x, new_size.y);
        return o_thickness;
    }

    float getThickness(){return o_thickness;}
    sf::Color getColor(){return o_color;}
};


