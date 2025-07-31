#include "textbox.h"
#include <algorithm>
#include <iostream>
#include <cmath>

sf::Color tuneColor(sf::Color color, float factor)
{
    auto adjust = [factor](u_int8_t c)
    {
        if (factor > 1 && c == 0) c = 50;
        return c * factor;
    };

    return sf::Color(
        adjust(color.r),
        adjust(color.g),
        adjust(color.b)
    );
}

void Textbox::togglePlaceholder(bool toggle)
{
    if (toggle)
    {
        m_text.setString(m_placeholderText);
        m_text.setFillColor(tuneColor(m_textColor, 1.8f));
    }else {
        m_text.setFillColor(m_textColor);
    }
    displayText();
}

void Textbox::centerText()
{
    sf::FloatRect newBounds = m_text.getLocalBounds();
    m_text.setOrigin({newBounds.position.x + newBounds.size.x / 2.f, newBounds.position.y + newBounds.size.y / 2.f});
    m_text.setPosition(m_background.getPosition() + (m_background.getSize() / 2.f));
}

void Textbox::setTail()
{
    sf::Vector2f bgSize = m_background.getSize();

    float height = std::clamp(m_text.getCharacterSize() * 1.25f, 0.f, bgSize.y - ((bgSize.y * m_paddingRatio)*2));
    sf::Vector2f charPos = m_text.findCharacterPos(focusPosition);

    tailRect.setSize({bgSize.x * 0.02f, height});
    tailRect.setPosition({charPos.x, m_background.getPosition().y + (bgSize.y - height) / 2.f});
}

void Textbox::displayText()
{    
    const sf::Vector2f backgroundSize = m_background.getSize();
    const float padding = backgroundSize.x * m_paddingRatio;
    float maxFontSize = backgroundSize.y - (padding * 2);

    m_text.setCharacterSize(maxFontSize);

    // Getting text contents

    std::string str;

    if (selected)
    {
        str = m_textContents;
    }else {
        if (m_textContents.empty() && isMutable)
        {
            str = m_placeholderText;
        }else {
            str = m_textContents;
        }
    }

    m_text.setString(str);
    centerText();

    // Resizing the text

    const float availableWidth = backgroundSize.x - (padding * 2);
    unsigned int low = 1;
    unsigned int high = maxFontSize;
    unsigned int bestFit = low;

    int steps = 0;

    while (low <= high)
    {
        unsigned int mid = (low + high)/2;
        m_text.setCharacterSize(mid);
        unsigned int new_width = m_text.getGlobalBounds().size.x;

        if (new_width <= availableWidth)
        {
            bestFit = mid;
            low = mid + 1; // shoot for higher baby, it's somewhere
        }else {
            high = mid - 1; // fuck we messed up, we gotta go bald (lower)
        }

        steps++;
    }

    m_text.setCharacterSize(bestFit);
    std::cout << "Took " << steps << " steps\n";

    centerText();

    if (highlighted){ highlight(highlight_start, highlight_end); }

    if (selected)
    {
        setTail();
    }
}

Textbox::Textbox
(
            std::string text, 
            sf::Font* font, 
            sf::Vector2f size,
            sf::Color backgroundColor, 
            float padding_ratio, 
            sf::Color fill_color, 
            sf::Color outline_color,
            float outline_ratio
):           
    m_text(*font, text, 0),
    m_font(font),
    m_textContents(text),
    m_background(size),
    m_backgroundColor(backgroundColor),
    m_textColor(fill_color),
    m_outlineRatio(outline_ratio),
    m_paddingRatio(padding_ratio),
    focusPosition(text.size())
{
    m_background.setFillColor(backgroundColor);
    m_text.setFillColor(fill_color);
    m_text.setOutlineColor(outline_color);
    tailRect.setFillColor(sf::Color::Black);

    // set outline thickness will be taken care of whenever the adjust text function is called
    
    setType(T_TXBX);

    if (!text.empty())
    {
        setString(text);
    }else {
        togglePlaceholder(true);
    }
}           

void Textbox::setOutline(Outline outline)
{
    m_background.setOutlineThickness(outline.o_thickness);
    m_background.setOutlineColor(outline.o_color);

    m_outline = outline;
}

void Textbox::setSize(sf::Vector2f new_size)
{
    m_background.setOutlineThickness(m_outline.adjust(new_size));
    m_background.setSize(new_size);

    displayText(); // this'll set the tail too
}

void Textbox::setPosition(sf::Vector2f new_position)
{
    m_background.setPosition(new_position);
    centerText();

    if(highlighted) 
        highlight(highlight_start, highlight_end);

    if(selected)
        setTail();
}

void Textbox::enableMutability(int max_characters, sf::Color highlight_color)
{
    if (isMutable)
        return;

    isMutable = true;
    maxCharacters = max_characters;

    highlightRect.setFillColor(highlight_color);
}

void Textbox::disableMutability()
{
    if (!isMutable)
        return;

    isMutable = false;

    if (selected)
    {
        clickOff();
    }
}

void Textbox::setString(std::string new_string)
{
    m_textContents = new_string;
    highlighted = false;
    
    if (!selected && new_string.empty())
    {
        togglePlaceholder(true);
    }
    
    displayText(); // this'll set the tail
}

void Textbox::handleClick(sf::Vector2f mousePos)
{
    if (!isMutable){return;}

    if (highlighted)
        highlighted = false;

    if (!selected)
    {
        togglePlaceholder(false);
        
        selected = true;
        m_background.setFillColor(tuneColor(m_backgroundColor, .8f));
    }

    focusPosition = std::ceil(getCharIndexFromPosition(mousePos));
    
    displayText(); // this'll set the tail
    
}

void Textbox::clickOff()
{
    if (!selected){return;}

    if (m_textContents.empty())
        togglePlaceholder(true);

    selected = false;
    m_background.setFillColor(m_backgroundColor);

    setString(m_textContents);
}

bool Textbox::containsRestriction(Restriction restriction)
{
    for (Restriction r : m_restrictions)
    {
        if (r == restriction)
        {
            return true;
        }
    }

    return false;
}

void Textbox::handleKey(char32_t character)
{
    /*
        CTRL + A = 1
        Backspace = 8
        Space = 20
        Numbers = 48-57
        Letters = 65-90 (uppercase), 97-122 (lowercase)
    */

    constexpr char32_t BACKSPACE = 8;
    constexpr char32_t SPACE = 20;
    constexpr char32_t ENTER = 13;
    constexpr char32_t SELECT_ALL = 1;

    if (character >= 128 || character == ENTER)
        return;

    if (character == SELECT_ALL && !highlighted)
    {
        highlighted = true;
        
        highlight(0, m_textContents.size()-1);
        
        return;
    }

    if (character == BACKSPACE) // backspace
    {
        if (highlighted)
        {
            m_textContents.erase(highlight_start, highlight_end-highlight_start + 1);
            focusPosition = highlight_start;

            highlighted = false;
        }else if (focusPosition > 0)
        {
            m_textContents.erase(focusPosition-1, 1);
            focusPosition--;
        }

        setString(m_textContents);

        return;
    }

    bool isSpace = character == SPACE;
    bool isNumber = character >= U'0' && character <= U'9';
    bool isUpper = character >= U'A' && character <= U'Z';
    bool isLower = character >= U'a' && character <= U'z';
    bool isLetter = isUpper || isLower;
    bool isSpecial = !isSpace && !isNumber && !isLetter;

    bool insert = false;

    if (isSpace)
    {
        insert = !containsRestriction(NO_SPACE);
    }
    else if (isNumber)
    {
        insert = !containsRestriction(NO_NUMBERS);
    }
    else if (isUpper)
    {
        insert = !containsRestriction(NO_LETTERS) && !containsRestriction(NO_UPPER_CASE);
    }
    else if (isLower)
    {
        insert = !containsRestriction(NO_LETTERS) && !containsRestriction(NO_LOWER_CASE);
    }
    else if (isSpecial)
    {
        insert = !containsRestriction(NO_SPECIAL_CHARACTERS);
    }

    if (m_textContents.size() >= maxCharacters && !highlighted)
        return;

    if (insert)
    {

        if (highlighted)
        {
            m_textContents.erase(highlight_start, highlight_end-highlight_start + 1);
            focusPosition = highlight_start;

            highlighted = false;
        }

        m_textContents.insert(focusPosition, 1, character);
        focusPosition++;
    } 

    setString(m_textContents);
}

void Textbox::setPlaceholderText(std::string placeholder_text)
{
    m_placeholderText = placeholder_text;

    if (m_textContents.empty())
        togglePlaceholder(true);
}

void Textbox::shiftFocus(int direction)
{
    if (direction == 1)
    {
        if (focusPosition < m_textContents.size())
        {
            focusPosition++;
        }
    }else if (direction == -1)
    {
        if (focusPosition > 0)
        {
            focusPosition--;
        }
    }

    displayText(); // this'll set the tail
}   

float Textbox::getCharIndexFromPosition(sf::Vector2f position)
{
    if (m_textContents.empty())
        return 0;

    // Localize the position (relative to the top-right corner of the text itself)
    position -= m_text.getGlobalBounds().position;

    if (position.x < 0)
    {
        return 0;
    }

    float i = 0;
    int currx = 0;

    for (char c : m_textContents)
    {
        float advance = m_font->getGlyph(c, m_text.getCharacterSize(), false, m_text.getOutlineThickness()).advance;

        if (position.x >= currx && position.x <= currx + advance)
        {
            // Localize the positon relative to the character
            float localX = position.x - currx;

            if (localX >= advance / 2.f)
            {
                return i + .5;
            }

            return i;
        } 

        currx += advance;
        i+=1.f;
    }

    return i;
}

bool isHalf(float num)
{
    return std::floor(num) < num;
}

void Textbox::highlight(sf::Vector2f start_position, sf::Vector2f end_position, Textbox* debugTextbox)
{
    float rawStartIndex = getCharIndexFromPosition(start_position); // returns x.5 if the mouse is atleast halfway through the character
    float rawEndIndex = getCharIndexFromPosition(end_position);

    // the mouse position has to be atleast half of the character in order for it to highlight
    // if the mouse starts off more than halfway through a character, that character isn't included
    
    // if the start position starts at less than half of the character but then ends at more than halfway, the character is included

    debugTextbox->setString("S_RAW: " + std::to_string(rawStartIndex) + " | E_RAW: " + std::to_string(rawEndIndex));

    if (rawStartIndex == rawEndIndex)
    {
        highlighted = false;
        return;
    }

    if (rawEndIndex < rawStartIndex)
    {
        std::swap(rawStartIndex, rawEndIndex);
    }

    if (isHalf(rawStartIndex))
    {
        rawStartIndex = std::ceil(rawStartIndex);
    }

    if (isHalf(rawEndIndex))
    {
        rawEndIndex = std::floor(rawEndIndex);
    }else {
        rawEndIndex--;
    }

    highlight(rawStartIndex, rawEndIndex);
}

void Textbox::highlight(unsigned int start_index, unsigned int end_index)
{
    if (start_index >= m_textContents.size() || end_index >= m_textContents.size())
        return;

    highlighted = true;
    highlight_start = start_index;
    highlight_end = end_index;

    sf::Vector2f tlPos = m_text.findCharacterPos(start_index);
    sf::Vector2f brPos = m_text.findCharacterPos(end_index) + (sf::Vector2f) { (float) m_font->getGlyph(m_textContents[end_index], m_text.getCharacterSize(), false, m_text.getOutlineThickness()).advance, (float) m_text.getCharacterSize()};

    highlightRect.setSize(brPos - tlPos);
    highlightRect.setPosition(tlPos);
}