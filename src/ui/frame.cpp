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

    for (Element* child : children)
    {
        child->draw(window);
    }
}

void Frame::setSize(sf::Vector2f new_size)
{
    m_rect.setSize(new_size);

    sf::Vector2f windowSize = (sf::Vector2f) m_window->getSize();

    sizeScale = {
        new_size.x/windowSize.x,
        new_size.y/windowSize.y
    };
    aspectRatio = new_size.x/new_size.y;
}

void Frame::setPosition(sf::Vector2f new_pos)
{
    m_rect.setPosition(new_pos);
}

void Frame::setAlignment(Alignment alignment)
{
    m_alignment = alignment;

    m_rect.setPosition(
        getPosInSpace( 
            (sf::Vector2f) m_window->getSize(), 
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

    m_rect.setSize(rectSize*scale);
    m_rect.setOutlineThickness(m_outline.o_thickness *= scale);

    if(m_alignment != NIL_ALIGNMENT) {setAlignment(m_alignment);}

    // Then, children

    for (Element* child : children)
    {
        child->setSize({
            (rectSize.x * scale) * (child->getSize().x/rectSize.x),
            (rectSize.y * scale) * (child->getSize().y/rectSize.y)
        });

        child->setPosition(
            
            getPosInSpace(rectSize * scale, child->getSize(), m_rect.getPosition(), child->getAlignment(), child->getOutlineSize())
        );
    };

}

void Frame::addChild(Element* child)
{
    children.push_back(child);
    child->setPosition(
        getPosInSpace(m_rect.getSize(), child->getSize(), m_rect.getPosition(), child->getAlignment(), child->getOutlineSize())
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