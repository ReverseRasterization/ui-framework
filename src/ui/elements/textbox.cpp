#include "textbox.h"
#include "clip.h"
#include <algorithm>
#include <iostream>
#include <cmath>

#pragma region GENERAL_FUNCTIONS

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

#pragma endregion

#pragma region LINE_FUNCTIONS

sf::FloatRect Textbox::Line::getBounds(const sf::Text& m_text)
{
    sf::FloatRect retRect;

    if (startIndex > m_text.getString().getSize()-1 || endIndex > m_text.getString().getSize()-1)
    {
        std::cerr << "[ERROR] - ABORTING Line::getBounds DUE TO OUT OF BOUND INDICIES\n";
        std::cerr << "      m_text Size: " << m_text.getString().getSize() << '\n';
        std::cerr << "       m_text Contents: " << m_text.getString().toAnsiString() << "|\n";
        std::cerr << "      _startIndex: " << startIndex << '\n';
        std::cerr << "      _endIndex: " << endIndex << "\n\n";
        return sf::FloatRect({-1.f, -1.f}, {-1.f, -1.f});
    }
    
    sf::Vector2f startCharPos = m_text.findCharacterPos(startIndex);
    sf::Vector2f endCharPos = m_text.findCharacterPos(endIndex);

    retRect.position = startCharPos;
    retRect.size = (endCharPos - startCharPos) + (sf::Vector2f) {m_text.getFont().getGlyph(m_text.getString()[endIndex], m_text.getCharacterSize(), false, m_text.getOutlineThickness()).advance, (float) m_text.getCharacterSize()};

    return retRect;
}

#pragma endregion

#pragma region PRIVATE_FUNCTIONS

std::vector<Textbox::Line*> Textbox::getLinesByIndexRange(unsigned int i_begin, unsigned int i_end)
{
    if (i_begin > m_textContents.size()-1)
        return {nullptr};

    std::vector<Textbox::Line*> retVec;

    if (i_begin > i_end)
    {
        std::swap(i_begin, i_end);
    }    

    // first find where the beginning index resides then find where the end index resides
    int beginningResidence = -1;
    int endResidence = -1;

    unsigned int i = 0;
    for (Line& line : m_lines)
    {
        if (line.getStartIndex() <= i_begin && line.getEndIndex() >= i_begin)
        {
            beginningResidence = i;
        }
        
        if (line.getStartIndex() <= i_end && line.getEndIndex() >= i_end)
        {
            endResidence = i;
            retVec.push_back(&line);
            break;
        }

        if (beginningResidence > -1 && endResidence == -1) // if the beginning residence has been found but the end residence hasn't...
        {
            retVec.push_back(&line);
        }

        i++;
    }

    if (endResidence == -1) // if the end residence hasn't been found yet...
    {
        endResidence = m_lines.size()-1;
    }

    return retVec;
}

void Textbox::updateLines()
{
    m_lines.clear();

    int start_i = 0;
    int i = 0;

    for (int i = 0; i < m_textContents.size(); i++)
    {
        char c = m_textContents[i];

        if (c == '\n')
        {
            m_lines.push_back(Line(start_i, i));

            start_i = i + 1; // the next i    
        }
    }

    m_lines.push_back(Line(start_i, m_textContents.size()-1));
    
}

void Textbox::positionText()
{
    sf::FloatRect bounds = m_text.getLocalBounds();

    const sf::Vector2f bgPos = m_background.getPosition();
    const sf::Vector2f bgSize = m_background.getSize();
    const float horizPadding = bgSize.x * m_paddingRatio.x;
    const float vertPadding = bgSize.y * m_paddingRatio.y;

    if (m_textAlignment == TextAlignment::LEFT)
    {
        m_text.setOrigin(bounds.position + (sf::Vector2f) {0.f, bounds.size.y/2.f});
        m_text.setPosition(bgPos + (sf::Vector2f) {horizPadding, bgSize.y/2});
    }else if (m_textAlignment == TextAlignment::CENTER)
    {
        m_text.setOrigin(bounds.getCenter());
        m_text.setPosition(bgPos + (bgSize / 2.f));
    }else if (m_textAlignment == TextAlignment::RIGHT) {
        m_text.setOrigin({bounds.position.x + bounds.size.x, bounds.getCenter().y});
        m_text.setPosition({bgPos.x + bgSize.x - horizPadding, bgPos.y + bgSize.y/2});
    }
}

void Textbox::displayText()

{    
    const sf::Vector2f backgroundSize = m_background.getSize();
    const float xPadding = backgroundSize.x * m_paddingRatio.x;
    const float yPadding = backgroundSize.y * m_paddingRatio.y;
    float maxFontSize = backgroundSize.y - (yPadding * 2);

    m_text.setCharacterSize(maxFontSize);

    // Getting text contents

    std::string str;

    if (!selected && m_textContents.empty() && isMutable)
    {
        str = m_placeholderText;
    }else {
        if (masking)
        {
            str = std::string(m_textContents.length(), maskingChar);
        }else {
            str = m_textContents;
        }
        
    }

    m_text.setString(str);
    positionText();

    // Resizing the text

    const float availableWidth = backgroundSize.x - (xPadding * 2);
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

    positionText();

    std::cout << "Character Size: " << m_text.getCharacterSize() << "\n\n";

    if (highlighted){ highlight(highlight_start, highlight_end); }

    if (selected)
    {
        setTail();
    }
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

void Textbox::setTail()
{
    sf::Vector2f bgSize = m_background.getSize();
    sf::Vector2f bgPos = m_background.getPosition();

    float padding = bgSize.y * m_paddingRatio.y;
    float height = std::clamp(m_text.getCharacterSize() * 1.25f, 0.f, bgSize.y - (padding*2));
    tailRect.setSize({bgSize.x * 0.02f, height});

    sf::Vector2f charPos = m_text.findCharacterPos(focusPosition) - (sf::Vector2f) {m_text.getLetterSpacing(), 0.f};
    charPos = {std::clamp(charPos.x, bgPos.x + padding, bgPos.x + bgSize.x - padding), charPos.y};

    if (m_textContents.empty())
    {   
        tailRect.setPosition({charPos.x, m_background.getPosition().y + padding});
    }else {
        tailRect.setPosition({charPos.x, std::clamp(charPos.y, m_background.getPosition().y+padding, (m_background.getPosition().y + bgSize.y)-padding)});
    }
}

float Textbox::getCharIndexFromPosition(sf::Vector2f position)
{
    
    if (m_textContents.empty())
        return 0.f;

    debugCells = {};

    // Get which line corresponds with the y axis
    
    Line targetLine(0,m_textContents.size()-1);

    if (m_lines.size() > 0) // multi line
    {
        bool found = false;

        for (Line line : m_lines)
        {
            
            sf::FloatRect bounds = line.getBounds(m_text);

            if (bounds.size.x == -1)
            {
                std::cerr << "[ERROR] - ABORTING getCharIndexFromPosition, BOUNDS FOR LINE RETURNED FAILED\n";
                return -1.f;
            }

            float topy = bounds.position.y;
            float bottomy = bounds.position.y + bounds.size.y;

            if (topy <= position.y && bottomy >= position.y)
            {
                targetLine = line;
                found = true;
                break;
            }
        }

        // If none found, if the mouse is above or below the text choose the first or last line
        if (!found)
        {
            if (position.y <= m_text.getGlobalBounds().position.y) // above
            {
                targetLine = m_lines[0];
            }else { // below
                targetLine = m_lines[m_lines.size()-1];
            }
        }
    }
    

    
     
    sf::FloatRect b = targetLine.getBounds(m_text);
    if (b.size.x == -1)
    {
        std::cerr << "[ERROR] - ABORTING getCharIndexFromPosition, BOUNDS FOR LINE RETURNED FAILED\n";
        return -1.f;
    }

    // time to get to business baby

    unsigned int fontSize = m_text.getCharacterSize();
    float oThickness = m_text.getOutlineThickness();

    float best_i = 0;
    float minDistance = std::numeric_limits<float>::max();
    bool half = false;
    
    if (targetLine.getStartIndex() > m_textContents.size()-1 || targetLine.getEndIndex() > m_textContents.size()-1)
    {
        std::cerr << "[ERROR] - TARGET LINE CALCULATED IN getCharIndexFromPosition OUT OF BOUNDS." << '\n';
        std::cerr << "      m_textContents size: " << m_textContents.size() << '\n';
        std::cerr << "      Line start index: " << targetLine.getStartIndex() << '\n';
        std::cerr << "      Line end index: " << targetLine.getEndIndex() << "\n\n";
        return -1.f;
    }

    for (int i = targetLine.getStartIndex(); i <= targetLine.getEndIndex(); i++)
    {
        char c = m_textContents[i];
        
        if (c == '\n')
            continue;

        sf::Vector2f charPos = m_text.findCharacterPos(i) + (sf::Vector2f) {0.f, fontSize/2.f}; // the char pos is centered in the character
        float charAdvance = m_font->getGlyph(c, fontSize, false, oThickness).advance;

        sf::Vector2f delta = position - charPos;

        float dist = delta.x * delta.x + delta.y * delta.y;

        if (dist < minDistance)
        {
            minDistance = dist;
            best_i = i;

            if (position.x > charPos.x + charAdvance)
            {
                half = true;
            }else {
                half = false;
            }
        }
    }

    

    return half ? best_i + 0.5f : best_i;
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

void Textbox::appendHistory(Textbox::HistoryElement n_element)
{
    if (history.size() > HISTORY_LIMIT)
    {
        history.erase(history.begin(), history.begin() + 10);
    }

    undoHistory.clear();
    history.push_back(n_element);
}

void Textbox::undo()
{
    if (history.empty())
        return;

    HistoryElement elm = history[history.size()-1];

    history.pop_back();

    if (elm.type == CHAR_INSERTION) 
    {
        m_textContents.erase(elm.index);

        undoHistory.push_back(HistoryElement(SINGLE_DELETION, elm.index, elm.character));

        focusPosition = elm.index;
    }else if (elm.type == MASS_INSERTION)
    {
        m_textContents.erase(elm.index, elm.index + elm.string.size());
        
        undoHistory.push_back(HistoryElement(MASS_DELETION, elm.index, elm.string));

        focusPosition = elm.index;
    }else if (elm.type == SINGLE_DELETION)
    {
        m_textContents.insert(elm.index, 1, elm.character);

        undoHistory.push_back(HistoryElement(CHAR_INSERTION, elm.index, elm.character));
        focusPosition = elm.index + 1;
    }else if (elm.type == MASS_DELETION)
    {
        m_textContents.insert(elm.index, elm.string);

        undoHistory.push_back(HistoryElement(MASS_INSERTION, elm.index, elm.string));

        focusPosition = elm.index + elm.string.size();
    }
    

    setString(m_textContents);
}

void Textbox::redo()
{
    if (undoHistory.empty())
        return;

    HistoryElement elm = undoHistory[undoHistory.size()-1];

    undoHistory.pop_back();

    if (elm.type == CHAR_INSERTION)
    {
        m_textContents.erase(elm.index);
        history.push_back(HistoryElement(SINGLE_DELETION, elm.index, elm.character));

        focusPosition = elm.index + 1;
    }else if (elm.type == MASS_INSERTION)
    {
        m_textContents.erase(elm.index, elm.string.size());
        history.push_back(HistoryElement(MASS_DELETION, elm.index, elm.string));

        focusPosition = elm.index;
    }else if (elm.type == SINGLE_DELETION)
    {
        m_textContents.insert(elm.index, 1, elm.character);
        history.push_back(HistoryElement(CHAR_INSERTION, elm.index, elm.character));

        focusPosition = elm.index + 1;
    }else if (elm.type == MASS_DELETION)
    {
        focusPosition = elm.index;
        appendString(elm.string); // handles pushing back the history
    }

    setString(m_textContents);
}   

bool Textbox::isRestricted(char character)
{
    bool isSpace = character == U' ';
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

    return !insert;
}

#pragma endregion

#pragma region PUBLIC_FUNCTIONS

Textbox::Textbox
(
            std::string text, 
            sf::Font* font, 
            sf::Vector2f size,
            sf::Color backgroundColor, 
            sf::Vector2f padding_ratio, 
            TextAlignment text_alignment,
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
    m_textAlignment(text_alignment),
    focusPosition(text.size())
{
    // Set the colors of everything
    m_background.setFillColor(backgroundColor);
    m_text.setFillColor(fill_color);
    m_text.setOutlineColor(outline_color);
    tailRect.setFillColor(sf::Color::Black);

    // set outline thickness will be taken care of whenever the adjust text function is called
    
    // As per the elements class
    setType(T_TXBX);

    // Placeholder stufferoons and line setup
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
    positionText();

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

    m_highlightColor = highlight_color;
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

    updateLines();
    displayText(); // this'll set the tail
}

void Textbox::handleClick(sf::Vector2f mousePos)
{
    if (!isMutable){return;}

    float targetIndex = getCharIndexFromPosition(mousePos);
    if (targetIndex == -1.f)
    {
        std::cerr << "[ERROR] - handleClick FAILED, getCharIndexFromPosition RETURNED -1" << '\n';
        return;
    }

    focusPosition = std::ceil(targetIndex);

    

    if (highlighted)
        highlighted = false;

    if (!selected)
    {
        togglePlaceholder(false);
        
        selected = true;
        m_background.setFillColor(tuneColor(m_backgroundColor, .8f));
    }



    
    
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

void Textbox::handleKey(char32_t character, bool redo)
{
    /*
        CTRL + A = 1
        CTRL + C = 3
        CTRL + V = 22
        CTRL + X = 24
        CTRL + Z = 26

        Backspace = 8
        DEL = 127
        Space = 20
        Numbers = 48-57
        Letters = 65-90 (uppercase), 97-122 (lowercase)
    */

    constexpr char32_t BACKSPACE = 8;
    constexpr char32_t DELETE = 127;
    constexpr char32_t SPACE = 20;
    constexpr char32_t ENTER = 13;
    constexpr char32_t SELECT_ALL = 1;
    constexpr char32_t COPY = 3;
    constexpr char32_t PASTE = 22;
    constexpr char32_t CUT = 24;
    constexpr char32_t UNDO = 26;

    if (character == SELECT_ALL)
    {
        if (highlighted)
            return;

        highlighted = true;
        
        highlight(0, m_textContents.size()-1);
        
        return;
    }

    if (character == BACKSPACE)
    {
        if (highlighted)
        {
            appendHistory(HistoryElement(MASS_DELETION, highlight_start, m_textContents.substr(highlight_start, (highlight_end - highlight_start) + 1)));

            m_textContents.erase(highlight_start, highlight_end-highlight_start + 1);
            focusPosition = highlight_start;

            highlighted = false;
        }else if (focusPosition > 0)
        {
            appendHistory(HistoryElement(SINGLE_DELETION, focusPosition-1, m_textContents[focusPosition-1]));

            m_textContents.erase(focusPosition-1, 1);
            focusPosition--;
        }

        setString(m_textContents);

        return;
    }

    if (character == DELETE) 
    {
        if (highlighted)
        {
            appendHistory(HistoryElement(MASS_DELETION, highlight_start, m_textContents.substr(highlight_start, (highlight_end - highlight_start) + 1)));

            m_textContents.erase(highlight_start, (highlight_end-highlight_start) + 1);
            focusPosition = highlight_start;

            highlighted = false;
        }else if (focusPosition < m_textContents.size())
        {
            appendHistory(HistoryElement(SINGLE_DELETION, focusPosition, m_textContents[focusPosition]));
            m_textContents.erase(focusPosition, 1);
        }

        setString(m_textContents);

        return;
    }

    if (character == COPY)
    {
        if (!highlighted)
            return;
        
        clip::set_text(m_textContents.substr(highlight_start, (highlight_end-highlight_start)+1));

        return;
    }

    if (character == PASTE)
    {
        std::string nString;
        bool success = clip::get_text(nString);

        if (success)
            appendString(nString); // handles history
    }

    if (character == CUT)
    {
        if (!highlighted)
            return;

        bool success = clip::set_text(m_textContents.substr(highlight_start, (highlight_end-highlight_start)+1));

        if (success)
        {
            appendHistory(HistoryElement(HistoryType::MASS_DELETION, highlight_start, m_textContents.substr(highlight_start, (highlight_end-highlight_start)+1))); // todo: test this
            m_textContents.erase(highlight_start, highlight_end-highlight_start + 1);
            focusPosition = highlight_start;

            highlighted = false;
        }

        setString(m_textContents);

        return;
        
    }

    if (character == UNDO)
    {
        redo ? this->redo() : this->undo();
        return;
    }

    // Filter out unhandled control characters
    if ((character < 32 && character != BACKSPACE && character != DELETE && character != SELECT_ALL) || character == ENTER)
        return;

    if (isRestricted(character))
        return;

    if (m_textContents.size() >= maxCharacters && !highlighted)
        return;

    if (highlighted)
    {
        appendHistory(HistoryElement(MASS_DELETION, highlight_start, m_textContents.substr(highlight_start, (highlight_end-highlight_start) + 1))); // for content that was deleted
        appendHistory(HistoryElement(CHAR_INSERTION, highlight_start, character)); // for the character that'll be added

        m_textContents.erase(highlight_start, highlight_end-highlight_start + 1);
        m_textContents.insert(highlight_start, 1, character);

        focusPosition = highlight_start + 1;
        highlighted = false;
    }else {
        appendHistory(HistoryElement(CHAR_INSERTION, focusPosition, character));

        m_textContents.insert(focusPosition, 1, character);
        focusPosition++;
    }

    setString(m_textContents);
}

void Textbox::appendString(std::string string)
{
    togglePlaceholder(false);

    // Filter out characters on restrictions

    std::string str;

    for (char c : string)
    {
        if (!isRestricted(c))
        {
            str.append(1, c);
        }
    }

    // Slice string if necessary to meet max character requirement

    if (m_textContents.size() + str.size() > maxCharacters)
    {
        unsigned int availableSpace = maxCharacters - m_textContents.size();

        str = str.substr(0, availableSpace);
    }

    appendHistory(HistoryElement(MASS_INSERTION, focusPosition, str));
    m_textContents.insert(focusPosition, str);
    focusPosition += str.size();

    setString(m_textContents);
}

void Textbox::setPlaceholderText(std::string placeholder_text)
{
    m_placeholderText = placeholder_text;

    if (m_textContents.empty())
        togglePlaceholder(true);
}

void Textbox::maskCharacters(bool toggle, char mask_char)
{
    masking = toggle;

    if (toggle){
        maskingChar = mask_char;
    }
}

void Textbox::shiftFocus(int horizontal_direction, int vertical_direction)
{
    highlighted = false;

    if (m_lines.size() > 1 && vertical_direction != 0)
    {
        // Get the index of the current line

        unsigned int currLineIndex = 0;

        for (int i = 0; i < m_lines.size(); i++)
        {
            const Line& line = m_lines[i];

            if (line.getStartIndex() <= focusPosition && line.getEndIndex() + 1 >= focusPosition)
            {
                currLineIndex = i;
                break;
            }
        }

        // Move it baby 
        int newIndex = currLineIndex - vertical_direction;
        
        if (newIndex >= 0 && newIndex < m_lines.size())
        {
            const Line& currLine = m_lines[currLineIndex];
            const Line& newLine = m_lines[newIndex];

            unsigned int offset = focusPosition - currLine.getStartIndex();

            focusPosition = std::clamp(newLine.getStartIndex() + offset, newLine.getStartIndex(), newLine.getEndIndex());
        }
    }

    if (horizontal_direction == 1)
    {
        if (focusPosition < m_textContents.size())
        {
            focusPosition++;
        }
    }else if (horizontal_direction == -1)
    {
        if (focusPosition > 0)
        {
            focusPosition--;
        }
    }

    displayText(); // this'll set the tail
}   

bool isHalf(float num)
{
    return std::floor(num) < num;
}

void Textbox::highlight(sf::Vector2f start_position, sf::Vector2f end_position)
{ 
    float rawStartIndex = getCharIndexFromPosition(start_position); // returns x.5 if the mouse is atleast halfway through the character
    float rawEndIndex = getCharIndexFromPosition(end_position);


    if (rawStartIndex == -1.f || rawEndIndex == -1.f)
    {
        std::cerr << "[ERROR] - highlight (position override) FAILED, RAW START INDEX OR RAW END INDEX HAD -1\n";
        return;
    }

    // the mouse position has to be atleast half of the character in order for it to highlight
    // if the mouse starts off more than halfway through a character, that character isn't included
    
    // if the start position starts at less than half of the character but then ends at more than halfway, the character is included

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

    if (highlighted && rawStartIndex == highlight_start && rawEndIndex == highlight_end)
        return;

    highlight(rawStartIndex, rawEndIndex);
}

void Textbox::highlight(unsigned int start_index, unsigned int end_index)
{
    if (start_index >= m_textContents.size() || end_index >= m_textContents.size())
        return;

    highlightRects.clear();
    highlighted = true;
    highlight_start = start_index;
    highlight_end = end_index;

    std::vector<Line*> targetLines = getLinesByIndexRange(start_index, end_index);
    
    if (targetLines.size() > 1)
    {
        for (Line* line : targetLines)
        {
            unsigned int _startIndex = std::max(start_index, line->getStartIndex());
            unsigned int _endIndex = std::min(end_index, line->getEndIndex());

            if (_startIndex > m_textContents.size()-1 || _endIndex > m_textContents.size()-1)
            {
                std::cerr << "[ERROR] - ABORTING highlight (index variant) DUE TO OUT OF BOUND INDICIES\n";
                std::cerr << "      m_textContents Size: " << m_textContents.size() << '\n';
                std::cerr << "      _startIndex: " << _startIndex << '\n';
                std::cerr << "      _endIndex: " << _endIndex << "\n\n";
            }

            sf::Vector2f tlPos = m_text.findCharacterPos(_startIndex);
            sf::Vector2f brPos = m_text.findCharacterPos(_endIndex) + (sf::Vector2f) { (float) m_font->getGlyph(m_textContents[_endIndex], m_text.getCharacterSize(), false, m_text.getOutlineThickness()).advance, (float) m_text.getCharacterSize()};

            sf::RectangleShape nRect(brPos-tlPos);
            nRect.setPosition(tlPos);
            nRect.setFillColor(m_highlightColor);
            highlightRects.push_back(nRect);
        }
    }else {
        sf::Vector2f tlPos = m_text.findCharacterPos(start_index);
        sf::Vector2f brPos = m_text.findCharacterPos(end_index) + (sf::Vector2f) { (float) m_font->getGlyph(m_textContents[end_index], m_text.getCharacterSize(), false, m_text.getOutlineThickness()).advance, (float) m_text.getCharacterSize()};

        sf::RectangleShape nRect(brPos-tlPos);
        nRect.setPosition(tlPos);
        nRect.setFillColor(m_highlightColor);
        highlightRects.push_back(nRect);
    }
}

#pragma endregion