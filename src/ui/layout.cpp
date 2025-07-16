#include "layout.h"


/*

                    CELL STRUCT

*/

GridLayout::Cell::Cell(sf::Vector2f position, sf::Vector2f size, sf::Vector2f space_position, sf::Vector2f space_size):
    c_tlPos(position),
    c_size(size),
    c_spaceSize(space_size)
{
    tlPosRatio = {position.x/space_size.x, position.y/space_size.y};
    sizeRatio = {size.x/space_size.x, size.y/space_size.y};
}

void GridLayout::Cell::resize(sf::Vector2f new_size)
{
    c_tlPos = {new_size.x * tlPosRatio.x, new_size.y * tlPosRatio.y};
    c_size = {new_size.x * sizeRatio.x, new_size.y * sizeRatio.y};
}

void GridLayout::Cell::setPosition(sf::Vector2f new_position)
{
    c_tlPos = new_position;
    tlPosRatio = {new_position.x/c_spaceSize.x, new_position.y/c_spaceSize.y};
}

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
                space_position, 
                space_size
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

    int count = 0;

    for (int i = 0; i < m_rows; i++)
    {
        for (int j = 0; j < m_columns; j++)
        {
            Cell& tCell = grid[i + j];

            tCell.setSpaceSize(new_size);
            tCell.resize(cellSize),
            tCell.setPosition(
                {new_tl_pos.x + (cellSize.x * j), new_tl_pos.y + (cellSize.y * i)}
            );
            count++;
        }
    }

    std::cout << "Hit a total of " << count << " cells\n";
}

sf::Vector2f Layout::getPosInSpace(sf::Vector2f space_size, sf::Vector2f object_size, sf::Vector2f tl_pos, Layout::Alignment alignment, float outline_size)
{
    sf::Vector2f new_pos = tl_pos;

    sf::Vector2f center = {((space_size.x - object_size.x)/2), ((space_size.y-object_size.y)/2)};

    float right = space_size.x - object_size.x - outline_size;
    float bottom = space_size.y - object_size.y - outline_size;

    switch (alignment)
    {
        case Layout::Alignment::TOP_LEFT:
            new_pos += {outline_size, outline_size};
            break;
        case Layout::Alignment::CENTER_LEFT:
            new_pos += {0, center.y};
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