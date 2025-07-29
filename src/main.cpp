/*
    TODO:

    Add text alignment for text boxes
*/

#include <iostream>
#include <format>

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

    Frame frame(&window, sf::Color::White, GridLayout(3, 1));
    frame.setSize({500.f, 300.f});
    frame.setOutline(Outline(10.f, sf::Color::Blue, {500.f, 300.f}));
    frame.setAlignment(Layout::Alignment::CENTER);

    Textbox* tbox = new Textbox("", &font, {100.f, 25.f}, sf::Color::White, 0.04, sf::Color::Black, sf::Color::Red, 0.05f);
    tbox->enableMutability();
    tbox->setAlignment(Layout::Alignment::CENTER);
    tbox->setOutline(Outline(5.f, sf::Color::Red, tbox->getSize()));
    tbox->setPlaceholderText("Type here");

    Button* btn = new Button({50.f, 50.f}, Layout::Alignment::CENTER, Button::Text("CLEAR", &font, 4, sf::Color::White, sf::Color::Black, 0.2f));
    btn->setCellOccupancy(1);
    btn->setOutline(Outline(5.f, sf::Color::Black, btn->getSize()));
    btn->setBackgroundColor(sf::Color::Red); 

    Image* img = new Image({50.f, 50.f}, Layout::Alignment::CENTER);
    img->setCellOccupancy(2);
    img->setImage(Image::TileTexture(&buttonSet, {64, 0}, {32, 32}));
    img->setOutline(Outline(5.f, sf::Color::Red, {50.f, 50.f}));

    btn->onClick = [&]() {
        tbox->setString("");
    };

    frame.addChild(tbox);
    frame.addChild(btn);
    frame.addChild(img);

    Textbox* currTextbox {nullptr};

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
            }

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

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
                }
            }

            if(const auto* textEntered = event->getIf<sf::Event::TextEntered>())
            {

                if (currTextbox)
                {
                    currTextbox->handleKey(textEntered->unicode);
                }
            }

        }

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