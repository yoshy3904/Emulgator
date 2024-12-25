#include "emulator.hpp"
#include "ui.hpp"
#include <SFML/Graphics.hpp>

#include <iostream>
#include <chrono>
#include <filesystem>

/*
@todo
Try threadless Donkey Kong and Kirby CHECKED: Neither working threadless.
Check PPU
Make debugger more advanced (show different screens of ppu)

-CPU: 
    Stop Instruction Implementation
-Sound: 
    Implement all 4 sound channels completely.
    Fix delayed frequency change.
-Memory Banking:
    Implement more MBCs, finish MBC1
    Loading system for different MBCs
-PPU
    Priority
    Special hardware behaviour, like changing scx,scy during mode 3
-UI
    Detailed overview of the inner workings.
*/

int main()
{
    sf::RenderWindow window(sf::VideoMode(1072, 650), "Emulgator", sf::Style::Default);
    window.setFramerateLimit(60);
    
    ui::ResourceHandler<sf::String, sf::Font> font_handler;
    font_handler.load("Normal", "arial-monospaced.ttf");
    
    ui::ResourceHandler<sf::String, sf::Texture> resource_handler;
    resource_handler.load("ScrollBarUp", "rsc/scrollbar_up.png");
    resource_handler.load("ScrollBarDown", "rsc/scrollbar_down.png");
    resource_handler.load("DropDown", "rsc/dropdown.png");

    ui::Canvas canvas(window, font_handler.get("Normal"));
    canvas.setIcons(&resource_handler.get("ScrollBarUp"), &resource_handler.get("ScrollBarDown"), &resource_handler.get("DropDown"));

    std::mutex mutex;

    try
    {
        Emulator emulator(mutex);
        Debugger debugger(canvas, emulator, mutex);

        while(window.isOpen())
        {
            sf::Event event;
            while(window.pollEvent(event))
            {
                canvas.updateAllEvents(event);
                debugger.updateEvents(event);
                if(event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }

            window.clear(sf::Color(240, 240, 240));

            emulator.processScreenBuffers();
            canvas.updateAllLogic();
            debugger.updateLogic();
            canvas.drawAll();
            
            window.display();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << "\nPress Space to continue...\n";
        while(!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {}
    }
}

