#pragma once

#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>

class Frame;

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

    sf::Vector2f getPosInSpace(sf::Vector2f space_size, sf::Vector2f object_size, sf::Vector2f tl_pos, Layout::Alignment alignment, float outline_size=0.f);
}


class GridLayout
{
    public:
        struct Cell
        {
            sf::Vector2f c_tlPos;
            sf::Vector2f tlPosRatio;
            
            sf::Vector2f c_size;
            sf::Vector2f c_spaceSize;
            sf::Vector2f sizeRatio;
            
            void setPosition(sf::Vector2f new_position);
            void resize(sf::Vector2f new_size);

            void setSpaceSize(sf::Vector2f new_size) {c_spaceSize = new_size;}  // Note: this will not change the size or position
            
            sf::Vector2f getPosition(){return c_tlPos;} // warning, position is relative to the grid's base position
            sf::Vector2f getSize(){return c_size;}

            Cell(sf::Vector2f position, sf::Vector2f size, sf::Vector2f space_position, sf::Vector2f space_size);
        };

        std::vector<Cell>* getCells() {return &grid;}

        void setBounds(sf::Vector2f new_tl_pos, sf::Vector2f new_size);

        GridLayout(unsigned int rows, unsigned int columns, sf::Vector2f space_position, sf::Vector2f space_size);

    private:

        std::vector<Cell> grid;

        unsigned int m_rows;
        unsigned int m_columns;

};