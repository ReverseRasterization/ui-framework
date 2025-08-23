/*

    TODO:

        TEXTBOXES: 

            Add support for hidden characters

            Fix issues where characters that go under the baseline (ie. g, y, etc.) throw off the positioning of the text
            Fix highlighting where it'll highlight above the characters (ts pmo bro)
            Fix the tail being THICC whenever the textbox is wide

            Add multiline support (mostly implemented just make it so that the user can make new lines in mutability) and an option to disable it
                Implement text wrapping
                For CENTER alignment, make each line align with the center
                For RIGHT alignment, make each line stick to the right

            Add scaling options, all allow multi-line and single-line
                Expansion (multi-line only)
                Scroll effect                                 
                Resizing (already have)

        BUTTONS:

            Fix buttons adjustText function to work just as the text box class does
*/

#include <iostream>
#include <random>
#include <stack>
#include <string>
#include <vector>
#include <cassert>
#include <chrono>

#include <SFML/Graphics.hpp>

#include "ui/layout.h"
#include "ui/frame.h"
#include "ui/elements/button.h"
#include "ui/elements/textbox.h"
#include "ui/elements/image.h"

struct iElement
{
    Button* btn {nullptr};
    Textbox* txbx {nullptr};
    Element* element {nullptr};
    
    Element::Type type;

    iElement() = default;

    static iElement from(Element* target)
    {
        if (target)
        {
            Element::Type t = target->getType();
            iElement result;

            result.type = t;
            result.element = target;

            if (t==Element::T_BTN)
            {
                result.btn = dynamic_cast<Button*>(target);
            }else if (t==Element::T_TXBX)
            {
                result.txbx = dynamic_cast<Textbox*>(target);
            }

            return result;
        }

        return {};
    }

    bool isButton(){return btn!=nullptr;}
    bool isTextbox(){return txbx!=nullptr;}
    bool exists(){return element != nullptr;}
};

iElement getInteractiveFromPosition(sf::Vector2f pos, Frame& active_frame);

std::string generateRandomString(size_t len) {
    static const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, charset.size() - 1);

    std::string result;
    result.reserve(len);

    for (size_t i = 0; i < len; ++i) {
        result += charset[dist(rng)];
    }

    return result;
}

void simulateStressTest(Textbox& textbox, int iterations = 5000) {
    std::string expected;
    std::stack<std::string> undoStack;
    std::stack<std::string> redoStack;

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> opDist(0, 9); // 0-4 = insert/delete, 5-7 = undo, 8-9 = redo
    std::uniform_int_distribution<> charDist(32, 126);
    std::uniform_int_distribution<> strLenDist(1, 10);

    for (int i = 0; i < iterations; i++) {
        int op = opDist(rng);

        if (op <= 2 && expected.size() < 500) { // insert random char
            char c = static_cast<char>(charDist(rng));
            std::cout << "inserting: " << c << '\n';

            textbox.handleKey(c, false);
            expected.insert(textbox.getFocusPosition() - 1, 1, c);
            undoStack.push(expected);
            while (!redoStack.empty()) redoStack.pop();
        }
        else if (op == 3 && !expected.empty()) { // backspace
            int pos = textbox.getFocusPosition();
            if (pos > 0) {
                textbox.handleKey(8, false); // BACKSPACE
                expected.erase(pos - 1, 1);
                undoStack.push(expected);
                while (!redoStack.empty()) redoStack.pop();
            }
        }
        else if (op == 4) { // paste random string
            std::string str = generateRandomString(strLenDist(rng));
            textbox.appendString(str);
            expected.insert(textbox.getFocusPosition() - str.size(), str);
            undoStack.push(expected);
            while (!redoStack.empty()) redoStack.pop();
        }
        else if (op == 5 && !undoStack.empty()) { // undo
            redoStack.push(expected);
            textbox.handleKey(26, false); // UNDO
            undoStack.pop();
            expected = undoStack.empty() ? "" : undoStack.top();
        }
        else if (op == 6 && !redoStack.empty()) { // redo
            undoStack.push(expected);
            textbox.handleKey(26, true); // REDO
            expected = redoStack.top();
            redoStack.pop();
        }

        // Validate after every operation
        std::string actual = textbox.getString(); // or however you get the string
        if (actual != expected) {
            std::cerr << "❌ Mismatch at iteration " << i << "\n";
            std::cerr << "Expected: \"" << expected << "\"\n";
            std::cerr << "Actual:   \"" << actual << "\"\n";
            exit(1);
        }

        // Optional: log progress
        if (i % 500 == 0) {
            std::cout << "[✓] " << i << " iterations passed\n";
        }
    }

    std::cout << "\n✅ Stress test passed all " << iterations << " iterations.\n";
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 800}), "UI Framework Test");

    sf::Font font;
    if (!font.openFromFile("assets/font.ttf"))
    {
        std::cerr << "Failed to load font!\n";
        return -1;
    }

    sf::Texture buttonSet;
    if(!buttonSet.loadFromFile("assets/uicons.png"))
    {
        std::cerr << "Failed to load uicons tileset!";
        return -1;
    }

    Frame frame(&window, sf::Color::White, GridLayout(1, 1));
    frame.setSize({500.f, 300.f});
    frame.setOutline(Outline(10.f, sf::Color::Blue, {500.f, 300.f}));
    frame.setAlignment(Layout::Alignment::CENTER);

    Textbox* tbox = new Textbox("", &font, {100.f, 100.f}, sf::Color::White, 0.04,Textbox::TextAlignment::CENTER, sf::Color::Black, sf::Color::Red, 0.05f);
    tbox->enableMutability(1000, sf::Color::Green);
    tbox->setAlignment(Layout::Alignment::CENTER);
    tbox->setOutline(Outline(5.f, sf::Color::Red, tbox->getSize()));
    tbox->setPlaceholderText("Type here");
    //tbox->setRestrictions({Textbox::Restriction::NO_NUMBERS});

    simulateStressTest(*tbox);

    // Button* btn = new Button({50.f, 50.f}, Layout::Alignment::CENTER, Button::Text("CLEAR", &font, 4, sf::Color::White, sf::Color::Black, 0.2f));
    // btn->setCellOccupancy(1);
    // btn->setOutline(Outline(5.f, sf::Color::Black, btn->getSize()));
    // btn->setBackgroundColor(sf::Color::Red); 

    // Image* img = new Image({50.f, 50.f}, Layout::Alignment::CENTER);
    // img->setCellOccupancy(2);
    // img->setImage(Image::TileTexture(&buttonSet, {64, 0}, {32, 32}));
    // img->setOutline(Outline(5.f, sf::Color::Red, {50.f, 50.f}));

    // btn->onClick = [&]() {
    //     tbox->setString("");
    // };

    frame.addChild(tbox);
    // frame.addChild(btn);
    // frame.addChild(img);

    Textbox* currTextbox {nullptr};
    sf::Vector2f highlightStartPos = {-1.f, -1.f};

    const auto btnCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand).value();
    const auto txtCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Text).value();
    const auto defaultCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
                window.setView(sf::View(visibleArea));

                frame.onWindowResized();
            }

            if (event->is<sf::Event::MouseMoved>())
            {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (!frame.getGlobalBounds().contains(mousePos)) continue;

                iElement target = getInteractiveFromPosition(mousePos, frame);

                if (target.isButton())
                {
                    window.setMouseCursor(btnCursor);
                }else if (target.isTextbox())
                {
                    window.setMouseCursor(txtCursor);
                }else if (currTextbox == nullptr) {
                    window.setMouseCursor(defaultCursor);
                }

                if (currTextbox && highlightStartPos.x > -1.f)
                {
                    currTextbox->highlight(highlightStartPos, mousePos);
                }

            }

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                    continue;

                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                // Textbox highlighting
                if (currTextbox && currTextbox->getGlobalBounds().contains(mousePos))
                {
                    highlightStartPos = mousePos;
                }

                iElement target = getInteractiveFromPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)), frame);

                if (currTextbox && currTextbox != target.element)
                {
                    currTextbox->clickOff();
                    currTextbox = nullptr;
                    window.setMouseCursor(defaultCursor);
                }

                if(!target.exists()) continue;

                if (target.isButton())
                {
                    target.btn->onClick();
                    continue;
                }
                
                if (target.isTextbox())
                {
                    if (currTextbox && target.txbx != currTextbox) currTextbox->clickOff();

                    currTextbox = target.txbx;
                    target.txbx->handleClick(mousePos);
                }
            }

            if (event->is<sf::Event::MouseButtonReleased>())
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || !currTextbox || highlightStartPos.x == -1.f)
                    continue;

                highlightStartPos = {-1.f, -1.f};
            }

            if (event->is<sf::Event::KeyPressed>())
            {
                if (!currTextbox)
                   continue;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
                {
                    currTextbox->shiftFocus(-1);
                }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
                {
                    currTextbox->shiftFocus(1);
                }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
                {
                    currTextbox->shiftFocus(0, 1);
                }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
                {
                    currTextbox->shiftFocus(0, -1);
                }
            }

            if(const auto* textEntered = event->getIf<sf::Event::TextEntered>())
            {
                if (currTextbox)
                {
                    currTextbox->handleKey(textEntered->unicode, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift));
                }
            }

        }


        // if (currTextbox) {
        //     sf::Vector2f pos = currTextbox->getPosition();
        //     sf::Vector2f size = currTextbox->getSize();

        //     // 🧪 Random highlight
        //     sf::Vector2f start = {
        //         pos.x + static_cast<float>(rand() % static_cast<int>(size.x)),
        //         pos.y + static_cast<float>(rand() % static_cast<int>(size.y))
        //     };

        //     sf::Vector2f end = {
        //         pos.x + static_cast<float>(rand() % static_cast<int>(size.x)),
        //         pos.y + static_cast<float>(rand() % static_cast<int>(size.y))
        //     };

        //     currTextbox->highlight(start, end);

        //     // 🧪 Random key input: a-z, space, or backspace
        //     int keyType = rand() % 10;
        //     char32_t c;
        //     if (keyType < 7) {
        //         c = U'a' + (rand() % 26); // Normal letters
        //     } else if (keyType == 7) {
        //         c = 32; // Space
        //     } else {
        //         c = 8; // Backspace
        //     }

        //     currTextbox->handleKey(c);

        //     // 🧪 Random caret movement
        //     int xShift = (rand() % 3) - 1; // -1, 0, or 1
        //     int yShift = (rand() % 3) - 1; // up/down support

        //     currTextbox->shiftFocus(xShift, yShift);
        // }

        window.clear();

        frame.draw(window);

        window.display();
    }

    return 0;
}

iElement getInteractiveFromPosition(sf::Vector2f pos, Frame& active_frame)
{
    Element* target = active_frame.getChildFromPosition(pos);
    iElement result = iElement::from(target);

    if (result.isButton())
    {
        if(result.btn->isActive() && result.btn->onClick)
        {
            return result;
        }
    }else if (result.isTextbox())
    {
        if(result.txbx->getMutable())
        {
            return result;
        }
    }

    return {};
}