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
            sf::Vector2f c_size;
            
            void setPosition(sf::Vector2f new_position) {c_tlPos = new_position;}
            void setSize(sf::Vector2f new_size) {c_size = new_size;}
            
            sf::Vector2f getPosition(){return c_tlPos;}
            sf::Vector2f getSize(){return c_size;}

            Cell(sf::Vector2f position, sf::Vector2f size, sf::Vector2f space_position):
                c_tlPos(position),
                c_size(size) {}
        };

        std::vector<Cell>* getCells() {return &grid;}
        Cell* getCellFromID(unsigned int id);

        void setBounds(sf::Vector2f new_tl_pos, sf::Vector2f new_size);

        GridLayout(unsigned int rows, unsigned int columns, sf::Vector2f space_position, sf::Vector2f space_size);

        void drawCells(sf::RenderWindow& window)
        {
            for (GridLayout::Cell& cell : grid)
            {   
                sf::Vector2f pos = cell.getPosition();
                sf::Vector2f size = cell.getSize();

                sf::RectangleShape cellRect(size);
                cellRect.setPosition(pos);
                cellRect.setFillColor(sf::Color::Transparent);
                cellRect.setOutlineColor(sf::Color::Red);
                cellRect.setOutlineThickness(1.f);

                window.draw(cellRect);
            }
        }

    private:
        std::vector<Cell> grid;

        unsigned int m_rows;
        unsigned int m_columns;

};