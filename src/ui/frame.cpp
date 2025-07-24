#include "frame.h"
#include <algorithm>
#include <iostream>

Frame::Frame(sf::RenderWindow* window, sf::Color color):
    m_rect({0.f, 0.f}),
    m_window(window)
{
    m_rect.setFillColor(color);
}

void Frame::draw(sf::RenderWindow& window)
{
    window.draw(m_rect);
    m_layout.drawCells(window);

    for (Element* child : children)
    {
        child->draw(window);
    }
}

void Frame::setSize(sf::Vector2f new_size, bool adjust_scale)
{
    m_rect.setSize(new_size);
    m_rect.setOutlineThickness(m_outline.adjust(new_size));

    m_layout.setBounds(getPosition(), getSize());

    if (adjust_scale)
    {
        sf::Vector2f windowSize = (sf::Vector2f) m_window->getSize();

        sizeScale = {
            new_size.x/windowSize.x,
            new_size.y/windowSize.y
        };
        aspectRatio = new_size.x/new_size.y;
    }
}

void Frame::setPosition(sf::Vector2f new_pos)
{
    m_rect.setPosition(new_pos);
    m_layout.setBounds(new_pos, getSize());
}

void Frame::setAlignment(Layout::Alignment alignment)
{
    m_alignment = alignment;

    setPosition(
        Layout::getPosInSpace( 
            (sf::Vector2f) m_window->getSize(), 
            {0.f, 0.f},
            m_rect.getSize(), 
            {0.f, 0.f},
            alignment,
            m_outline.o_thickness
        )
    );
}

void Frame::setOutline(Outline outline)

{
    m_rect.setOutlineThickness(outline.o_thickness);
    m_rect.setOutlineColor(outline.o_color);

    m_outline = outline;
}

void Frame::onWindowResized()
{
    // First, resize the frame

    sf::Vector2f nSize = (sf::Vector2f) m_window->getSize();

    /*
        H' = H * (W'/W)
        W' = W * (H'/H)

        Determine which is smaller: W'/W or H'/H
            S = min(W'/W, H'/H)

            Scaled Width = W * S
            Scaled Height = H * S
    */

    sf::Vector2f rectSize = m_rect.getSize(); // This is W or H
    sf::Vector2f projectedRectSize = {nSize.x * sizeScale.x, nSize.y * sizeScale.y}; // This is W' or H'

    float scale = std::min(projectedRectSize.x / rectSize.x, projectedRectSize.y / rectSize.y);

    setSize(rectSize*scale, false);
    m_rect.setOutlineThickness(m_outline.adjust(rectSize*scale));

    if(m_alignment != Layout::Alignment::NIL_ALIGNMENT) {setAlignment(m_alignment);}

    // Then, children

    for (Element* child : children)
    {
        GridLayout::Cell* parentCell = m_layout.getCellFromID(child->getCellOccupancy());
        sf::Vector2f spacePos = parentCell->getPosition();
        sf::Vector2f spaceSize = parentCell->getSize();

        child->setSize({
            (rectSize.x * scale) * (child->getSize().x/rectSize.x),
            (rectSize.y * scale) * (child->getSize().y/rectSize.y)
        });

        child->setPosition(
            
            Layout::getPosInSpace(spaceSize, spacePos, child->getSize(), child->getOffset(), child->getAlignment(), child->getOutlineSize())
        );
    };

}

void Frame::addChild(Element* child)
{
    children.push_back(child);

    GridLayout::Cell* parentCell = m_layout.getCellFromID(child->getCellOccupancy());
    sf::Vector2f spacePos = getPosition();
    sf::Vector2f spaceSize = getSize();

    if (parentCell)
    {
        spacePos = parentCell->getPosition();
        spaceSize = parentCell->getSize();
    }else {
        child->setCellOccupancy(0);
    }

    child->setPosition(
        Layout::getPosInSpace(spaceSize, spacePos, child->getSize(), child->getOffset(), child->getAlignment(), child->getOutlineSize())
    );
}

Element* Frame::getChildFromPosition(sf::Vector2f position)
{
    for (Element* child : children)
    {
        if (child->getGlobalBounds().contains(position))
        {
            return child;
        }
    }

    return nullptr;
}

