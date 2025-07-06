#pragma once

#include <iostream>
#include <vector>
#include <variant>

#include <SFML/Graphics.hpp>
#include "alignment.h"

struct Outline
{
    float o_thickness;
    sf::Color o_color;

    Outline(float thickness, sf::Color color): o_thickness(thickness), o_color(color){}
};




