#include "input.hpp"

#include <iostream>

Input::Input(Memory& p_memory, SHARP_LR35902& p_cpu) : memory(p_memory), cpu(p_cpu)
{

}

void Input::markKeyPressed(Keys key) 
{
    if(key == Start || key == Select || key == A || key == B)
        action_button_state |= key;
    else if(key == Down || key == Up || key == Left || key == Right)
        dir_button_state |= (key >> 4);
    
    cpu.requestInterrupt(SHARP_LR35902::Interrupt::JOYPAD);
}

void Input::markKeyReleased(Keys key) 
{
    if(key == Start || key == Select || key == A || key == B)
        action_button_state &= ~key;
    else if(key == Down || key == Up || key == Left || key == Right)
        dir_button_state &= ~(key >> 4);
}

bool Input::anyButtonPressed() 
{
    return ((dir_button_state & 0xf) != 0) || ((action_button_state & 0xf) != 0);
}

void Input::update() 
{
    /*
    Update Joypad port (required to disable restrictions when writingto memory since lower 4 bits of 0xff00 are normally read only).
    
    Joypad Port (0xFF00)
    Bit 7 - Not used
    Bit 6 - Not used
    Bit 5 - P15 Select Action buttons    (0=Select)
    Bit 4 - P14 Select Direction buttons (0=Select)
    Bit 3 - P13 Input: Down  or Start    (0=Pressed) (Read Only)
    Bit 2 - P12 Input: Up    or Select   (0=Pressed) (Read Only)
    Bit 1 - P11 Input: Left  or B        (0=Pressed) (Read Only)
    Bit 0 - P10 Input: Right or A        (0=Pressed) (Read Only)
    */

    if((memory.read(0xff00) & 0b00010000) == 0) // Asking for directional keys.
    {
        memory.write(0xff00, (memory.read(0xff00) & 0xf0) | ~dir_button_state, false);
        memory.write(0xff00, memory.read(0xff00) | 0b11000000, false);
    }
    else if((memory.read(0xff00) & 0b00100000) == 0) // Asking for action keys.
    {
        memory.write(0xff00, (memory.read(0xff00) & 0xf0) | ~action_button_state, false);
        memory.write(0xff00, memory.read(0xff00) | 0b11000000, false);
    }
}
