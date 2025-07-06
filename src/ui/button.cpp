#include "ui.h"

void Button::setAlignment(Alignment alignment)
{
    m_alignment = alignment;

    m_rect.setPosition(
        getPosInSpace( 
            m_parent.getSize(),
            m_rect.getSize(), 
            m_parent.getPosition(),
            alignment,
            m_outline.o_thickness
        )
    );
}

void Button::setOutline(Outline outline)

{
    m_rect.setOutlineThickness(outline.o_thickness);
    m_rect.setOutlineColor(outline.o_color);

    m_outline = outline;
}