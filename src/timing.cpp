#include "timing.hpp"

#include <windows.h>
#include <thread>
#include <iostream>

uint64_t getCurrentTimeInMicroseconds()
{
#if defined(_WIN32) || defined(_WIN64)
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    
    double inverse = 1000000.0 / frequency.QuadPart;
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return static_cast<uint64_t>(time.QuadPart * inverse);
#endif
}

void sleepMicroseconds(uint64_t amount)
{
#if defined(_WIN32) || defined(_WIN64)
    uint64_t start_time = getCurrentTimeInMicroseconds();
    uint64_t end_time = start_time + amount;

    while(getCurrentTimeInMicroseconds() < end_time);
#endif
}

Timer::Timer(Memory& p_memory, SHARP_LR35902& p_cpu) : memory(p_memory), cpu(p_cpu)
{
    
}

void Timer::update() 
{
    /*
    The DIV Timer is incremented at a frequency of 16384 Hz. That means every 256 t-cycles, and every 64 m-cycles an increment happens.
    In hardware it is nothing else than the MSB of a 16-bit internal counter. That is also way it increments every 256 cycles (that is one byte).
    */
    memory.write(0xff04, (internal_counter & 0xff00) >> 8, false);

    /*
    TIMA Timer
    */
    if(isTimerEnabled())
    {
        if(old_frequencyBit && !new_frequencyBit)
        {
            memory.write(0xff05, memory.read(0xff05) + 1);
            if(memory.read(0xff05) == 0x00)
            {
                // Set TIMA to TMA and throw an interrupt.
                memory.write(0xff05, memory.read(0xff06));
                cpu.requestInterrupt(SHARP_LR35902::TIMER);
            }
        }
    }
    old_frequencyBit = internal_counter & getFrequencyBit();
    internal_counter++;
    new_frequencyBit = internal_counter & getFrequencyBit();
}

uint16_t Timer::getFrequencyBit() 
{
    uint16_t clock_select = memory.read(0xff07) & 0b11; // Get the lower two bits of the TAC.
    switch(clock_select)
    {
        case 0b00: return 1 << 9;
        case 0b01: return 1 << 3;
        case 0b10: return 1 << 5;
        case 0b11: return 1 << 7;
    }

    return 0x00;
}

bool Timer::isTimerEnabled() 
{
    return memory.read(0xff07) & 0b100;
}