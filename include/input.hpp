#pragma once
#include "SHARP_LR35902.hpp"
#include "memory.hpp"

#include <stdint.h>

class SHARP_LR35902;
class Memory;

class Input
{
public:
    enum Keys
    {
        Down    = 1 << 7,
        Up      = 1 << 6,
        Left    = 1 << 5,
        Right   = 1 << 4,

        Start   = 1 << 3,
        Select  = 1 << 2,
        B       = 1 << 1,
        A       = 1 << 0,
    };
private:
    Memory& memory; // Update I/O port JOYPAD at 0xff00.
    SHARP_LR35902& cpu; // Request JOYPAD interrupt.

    uint8_t dir_button_state = 0;
    uint8_t action_button_state = 0;
public:
    Input(Memory& p_memory, SHARP_LR35902& p_cpu);

    void markKeyPressed(Keys key);
    void markKeyReleased(Keys key);

    bool anyButtonPressed();

    void update();
};
