#pragma once
#include "SHARP_LR35902.hpp"
#include "memory.hpp"

#include <stdint.h>

class SHARP_LR35902;
class Memory;

uint64_t getCurrentTimeInMicroseconds();

/*
This function sleeps for the amount specified. It is not completely accurate for times below a millisecond, but still more accurate
than std::this_thread::sleep_for(). sleep_for() lets the thread rest at least.
@todo There is only a version for windows.
@param amount Time in microseconds.
@return void
*/
void sleepMicroseconds(uint64_t amount);

/* 
TIMING

HARDWARE TIMER BEHAVIOUR

TIMA is incremented at a frequency specified in the lower two bits of the TAC register.
Once it overflows it is reset to TMA.

OBSCURE TIMER BEHAVIOUR

There are 3 ways an extra increment to the TIMA register can be made due to the hardware design
of the timers.

      ________________________________  <-- Reset on DIV_Write
     /     DIV        |               \ <-- Increase on every cycle
     |7 6 5 4 3 2 1 0 | 7 6 5 4 3 2 1 0|
                  |     |   |   |
                __|___ _|___|___|___
                | 0     3   2   1  | <---TAC Frequency----
                |___Multiplexer____|                   | | 
                         |                             | |    
                         |                             | |     
                        AND <---------TAC Enable------ | |                  
                         |                           | | |       
                Falling Edge Detector      7 6 5 4 3 2 1 0                    
                         |                           TAC              
                    TIMA Increment                                            

When the output of the AND is a falling edge (meaning it changes from 1 to 0) TIMA will increment.

How it works normally:
The DIV register is actually the MSB of a 16 bit internel counter. So if the 16-bit counter is incremented
every cycle, then DIV will be incremented every 256 cycles (size of one byte).
Now the TAC chooses a frequency by selecting a bit from the 16-bit internel counter with the help of the multiplexer.
If TAC enable is 1 and the output of the multiplexer changes from 1 to 0, there will be a TIMA increment.

There are other ways a falling edge can be produced:

1. When TAC enable changes from 1 to 0 and the output of the multiplexer is 1.
2. When TAC enable does not change but the TAC frequency gets changed by the programmer. If the changing of frequency makes the output of the multiplexer
go from 1 to 0, there will be a falling edge.
3. When TAC does not change but DIV is written when the output of the multiplexer is 1. Because writing to DIV resets the whole 16-bit internel counter the 
output of the multiplexer will change from 1 to 0.

When is the output of the multiplexer 1?
When the bit selected in the 16-bit internel counter by TAC is 1.
In code: cycles & (clock/2)

if old_enable = 0 
    // Falling edge not possible.
else 
    // On TAC enable change from 1 to 0:
    if old_enable = 1 && new_enable = 0 && cycles & (clock/2) != 0
        tima++;
    // On TAC clock change:
    else old_enable = 1 && new_enable = 1 && cycles & (clock/2) != 0
        tima++;

//On DIV write multiplexer output changes from 1 to 0:
if timer_enable = 1 && cycles & (clock/2) != 0
    tima++;
*/

class Timer
{
public:
    uint16_t internal_counter;
private:
    bool old_frequencyBit;
    bool new_frequencyBit;
    
    Memory& memory; // Change memory.
    SHARP_LR35902& cpu; // Request Timer interrupt.
public:
    Timer(Memory& p_memory, SHARP_LR35902& p_cpu);
    void update();

    /* 
    Returns a mask with the bit set that is currently selected in the internal counter by the TAC register. In hardware the lower two bits
    in the TAC register select this bit with a multiplexer.
    */
    uint16_t getFrequencyBit();
    /*
    Returns the 2 bit of the TAC register which holds the timer enabled flag.
    */
    bool isTimerEnabled();
};
