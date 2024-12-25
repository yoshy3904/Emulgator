#include "emulator.hpp"

#include <iostream>

#include "timing.hpp"

Emulator::Emulator(std::mutex& p_mutex) 
    : mutex(p_mutex),
    thread_finished(false), 
    enabled(false), 
    thread(), 
    memory(ppu, apu, timer), 
    cpu(memory), 
    ppu(memory, cpu), 
    input(memory, cpu), 
    timer(memory, cpu),
    apu(memory, mutex)
{
    thread = std::thread(&Emulator::worker, this);

    cycles_per_second_timer = std::chrono::high_resolution_clock::now();
}

Emulator::~Emulator() 
{
    std::cout << "Trying to join worker." << std::endl;
    thread_finished = true;
    thread.join();
    std::cout << "Joined" << std::endl;
}

void Emulator::processScreenBuffers() 
{
    ppu.processScreenBuffers();
}

void Emulator::reset()
{
    cpu.reset();
}

void Emulator::loadCartridge(const std::string& p_string)
{
    if(!enabled)
    {
        cpu.reset();
        memory.loadCartridge(p_string);
    }
}

void Emulator::markKeyPressed(Input::Keys key)
{
    input.markKeyPressed(key);
}

void Emulator::markKeyReleased(Input::Keys key)
{
    input.markKeyReleased(key);
}

void Emulator::setSpeed(int p_percentage)
{
    frequency_percentage = p_percentage;
    cycle_budget = 0;
    step_duration_timer = std::chrono::high_resolution_clock::now();
}

int Emulator::getSpeed()
{
    return frequency_percentage;
}

void Emulator::setVolume(float p_volume)
{
    apu.setVolume(p_volume);
}
float Emulator::getVolume()
{
    return apu.getVolume();
}

const Memory& Emulator::getMemory()
{
    return memory;
}

const SHARP_LR35902& Emulator::getCPU()
{
    return cpu;
}

const PPU& Emulator::getPPU()
{
    return ppu;
}

const APU& Emulator::getAPU()
{
    return apu;
}

void Emulator::setEnabled(bool p_bool) 
{
    enabled = p_bool;
    step_duration_timer = std::chrono::high_resolution_clock::now();
    cycle_budget = 0;

    if(p_bool)
    {
        apu.play();
    }
    else
    {
        apu.stop();
    }
}

bool Emulator::isEnabled() const
{
    return enabled;
}

void Emulator::worker()
{
    while(!thread_finished)
    {
        if(enabled == false) continue;

        /*
        If the actual work time of the computer to execute n cycles is larger than the time we would like to execute them in, we start 'lacking behing'
        which results in a steadily increasing cycle_budget. We ask for more cycles, than can actually be executed. How many cycles can be executed in a certain amount of time
        is machine dependent. How many cycles are requested per time is dependent on the frequency of the cpu. 
        
        if work_time for n cycles > desired_time for n cycles: starts 'lacking behind' and cycle_budget increases steadily
        */

        auto now = std::chrono::high_resolution_clock::now();
        int time_passed = std::chrono::duration_cast<std::chrono::milliseconds>(now - step_duration_timer).count();
        int cycles_to_execute = time_passed/1000.f * cpu_frequency * frequency_percentage/100.f;
        cycle_budget += cycles_to_execute;
        step_duration_timer = std::chrono::high_resolution_clock::now();

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Sleep shortly so that time_passed has a meaningful value (and not just 0 since barely any time passed).
        
        while(cycle_budget >= 0)
        {
            int cycles_since_last_instruction = step();
            // Subtract cycle budget.
            cycle_budget -= cycles_since_last_instruction;
        }
    }
}

// Execute as many cycles needed to finish the next instruction.
int Emulator::step() 
{
    mutex.lock();

    // CPU.
    cycles_since_last_instruction = cpu.nextInstruction();

    // Count cylces.
    cycle_counter += cycles_since_last_instruction;
    auto now = std::chrono::high_resolution_clock::now();
    if(std::chrono::duration_cast<std::chrono::milliseconds>(now - cycles_per_second_timer).count() >= 1000)
    {
        cycle_count_per_second = cycle_counter;
        cycle_counter = 0;
        cycles_per_second_timer = std::chrono::high_resolution_clock::now();
    }

    // INTERRUPTS. Interrupts are executed between the CPU instructions.
    if(cpu.interrupt_master_enable)
    {
        // Execute interrupt when IF and IE flag are enabled.
        using CPU_INT = SHARP_LR35902::Interrupt;
        if((memory.read(0xff0f) & CPU_INT::VBLANK) && (memory.read(0xffff) & CPU_INT::VBLANK))
            executeInterrupt(CPU_INT::VBLANK, 0x0040);
        else if((memory.read(0xff0f) & CPU_INT::LCD_STAT) && (memory.read(0xffff) & CPU_INT::LCD_STAT))
            executeInterrupt(CPU_INT::LCD_STAT, 0x0048);
        else if((memory.read(0xff0f) & CPU_INT::TIMER) && (memory.read(0xffff) & CPU_INT::TIMER))
            executeInterrupt(CPU_INT::TIMER, 0x0050);
        else if((memory.read(0xff0f) & CPU_INT::SERIAL) && (memory.read(0xffff) & CPU_INT::SERIAL))
            executeInterrupt(CPU_INT::SERIAL, 0x0058);
        else if((memory.read(0xff0f) & CPU_INT::JOYPAD) && (memory.read(0xffff) & CPU_INT::JOYPAD))
            executeInterrupt(CPU_INT::JOYPAD, 0x0060);
    }

    // LCD and TIMER.
    for (uint8_t i = 0; i < cycles_since_last_instruction; i++)
    {
        ppu.update();
        timer.update();
    }
    
    // INPUT.
    input.update();

    mutex.unlock();

    return cycles_since_last_instruction;
}

void Emulator::executeInterrupt(SHARP_LR35902::Interrupt p_type, uint16_t address) 
{
    // Disable interrupts so we do not get interrupted while executing the interrupt. The IE register is never reset by hardware.
    memory.write(0xff0f, 0x00);
    cpu.interrupt_master_enable = false;

    // Push value of pc onto stack.
    cpu.sp--;
    memory.write(cpu.sp, cpu.pc >> 8);
    cpu.sp--;
    memory.write(cpu.sp, cpu.pc & 0x00ff);

    // Jump to interrupt vector.
    cpu.pc = address;
}
