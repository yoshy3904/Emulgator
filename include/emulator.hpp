#pragma once
#include "SHARP_LR35902.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "input.hpp"
#include "timing.hpp"
#include "apu.hpp"

#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

class Emulator
{
public:
    uint64_t cycle_count_per_second = 0;
    int cycle_budget = 0;
private:
    std::thread thread;
    std::mutex& mutex; // Protects shared data by main thread and emulator thread.

    std::atomic<bool> thread_finished;
    std::atomic<bool> enabled = false; // Whether the emulator is running (at start set to false); meaning the cpu executes instructions, ppu processes pictures and so on.

    uint8_t cycles_since_last_instruction = 0; // After executing a step and therefore a cpu instruction this will contain the number of cycles that was needed for the operation.
    
    uint64_t cycle_counter = 0;
    std::chrono::_V2::system_clock::time_point cycles_per_second_timer;

    float cpu_frequency = 4194304.f;
    float frequency_percentage = 100.f;
    std::chrono::_V2::system_clock::time_point step_duration_timer;

    Memory memory;
    SHARP_LR35902 cpu;
    PPU ppu;
    Input input;
    Timer timer;
    APU apu;
public:
    Emulator(std::mutex& p_mutex);
    ~Emulator();

    /*
    Updates the screen buffers of the emulator. Information the screen buffers can be retrieved though the debug information.
    */
    void processScreenBuffers();

    void reset();
    void loadCartridge(const std::string& p_string);
    void markKeyPressed(Input::Keys key);
    void markKeyReleased(Input::Keys key);

    void setSpeed(int p_percentage);
    int getSpeed();
    void setVolume(float p_volume);
    float getVolume();

    const Memory& getMemory();
    const SHARP_LR35902& getCPU();
    const PPU& getPPU();
    const APU& getAPU();

    void setEnabled(bool p_state);
    bool isEnabled() const;
    /*
    Executes one simulation step of the gameboy. One cpu instruction is executed and other hardware updates accordingly.
    */
    int step();
private:
    /*
    Runs the emulator as long as thread_finished is false. It is run in a different thread to keep the emulator clean from debugging/ui code which slows it down.
    */
    void worker();

    void executeInterrupt(SHARP_LR35902::Interrupt p_type, uint16_t address);
};