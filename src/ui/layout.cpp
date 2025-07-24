#include "layout.h"


/*

                    CELL STRUCT

*/

/*

                    GRID LAYOUT CLASS

*/

GridLayout::GridLayout(unsigned int rows, unsigned int columns, sf::Vector2f space_position, sf::Vector2f space_size):
    m_rows(rows),
    m_columns(columns)
{
    sf::Vector2f cellSize = {
        space_size.x/columns,
        space_size.y/rows
    };

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            Cell nCell(
                {space_position.x + (cellSize.x * j), space_position.y + (cellSize.y * i)}, 
                cellSize, 
                space_position
            );

            grid.push_back(nCell);
        }
    }
}

void GridLayout::setBounds(sf::Vector2f new_tl_pos, sf::Vector2f new_size)
{
    sf::Vector2f cellSize = {
        new_size.x / m_columns,
        new_size.y / m_rows
    };

    for (int i = 0; i < m_rows; i++)
    {
        for (int j = 0; j < m_columns; j++)
        {
            Cell& tCell = grid[i * m_columns + j];

            tCell.setSize(cellSize),
            tCell.setPosition(
                {new_tl_pos.x + (cellSize.x * j), new_tl_pos.y + (cellSize.y * i)}
            );
        }
    }
}

GridLayout::Cell* GridLayout::getCellFromID(unsigned int id)
{
    if (grid.size()-1 >= id)
    {
        return &grid[id];
    }else {
        std::cerr << "Cannot get cell " << id << ", out of bounds.\n";
        return nullptr;
    }
}

sf::Vector2f Layout::getPosInSpace(sf::Vector2f space_size, sf::Vector2f tl_space_pos, sf::Vector2f object_size, sf::Vector2f object_offset_ratio, Layout::Alignment alignment, float outline_size)
{
    sf::Vector2f new_pos = tl_space_pos + sf::Vector2f{space_size.x * object_offset_ratio.x, space_size.y * object_offset_ratio.y};

    sf::Vector2f center = {((space_size.x - object_size.x)/2), ((space_size.y-object_size.y)/2)};

    float right = space_size.x - object_size.x - outline_size;
    float bottom = space_size.y - object_size.y - outline_size;

    switch (alignment)
    {
        case Layout::Alignment::TOP_LEFT:
            new_pos += {outline_size, outline_size};
            break;
        case Layout::Alignment::CENTER_LEFT:
            new_pos += {outline_size, center.y};
            break;
        case Layout::Alignment::BOTTOM_LEFT:
            new_pos += {outline_size, bottom};
            break;

        case Layout::Alignment::TOP_CENTER:
            new_pos += {center.x, outline_size};
            break;
        case Layout::Alignment::CENTER:
            new_pos += center;
            break;
        case Layout::Alignment::BOTTOM_CENTER:
            new_pos += {center.x, bottom};
            break;
        case Layout::Alignment::TOP_RIGHT:
            new_pos += {right, outline_size};
            break;
        case Layout::Alignment::CENTER_RIGHT:
            new_pos += {right, center.y};
            break;
        case Layout::Alignment::BOTTOM_RIGHT:
            new_pos += {right, bottom};
    };

    return new_pos;
}