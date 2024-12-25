#include "apu.hpp"

#include <cmath>
#include <bitset>

APU::APU(Memory& p_memory, std::mutex& p_mutex) 
    : mutex(p_mutex),
    out("sound_out.bin", std::ios::out | std::ios::binary),
    memory(p_memory)
{
    duty_cycles[0][0] = 0; duty_cycles[1][0] = 1; duty_cycles[2][0] = 1; duty_cycles[3][0] = 0;
    duty_cycles[0][1] = 1; duty_cycles[1][1] = 1; duty_cycles[2][1] = 1; duty_cycles[3][1] = 0;
    duty_cycles[0][2] = 0; duty_cycles[1][2] = 0; duty_cycles[2][2] = 1; duty_cycles[3][2] = 1;
    duty_cycles[0][3] = 0; duty_cycles[1][3] = 0; duty_cycles[2][3] = 1; duty_cycles[3][3] = 1;
    duty_cycles[0][4] = 0; duty_cycles[1][4] = 0; duty_cycles[2][4] = 0; duty_cycles[3][4] = 1;
    duty_cycles[0][5] = 0; duty_cycles[1][5] = 0; duty_cycles[2][5] = 0; duty_cycles[3][5] = 1;
    duty_cycles[0][6] = 0; duty_cycles[1][6] = 0; duty_cycles[2][6] = 0; duty_cycles[3][6] = 1;
    duty_cycles[0][7] = 0; duty_cycles[1][7] = 0; duty_cycles[2][7] = 0; duty_cycles[3][7] = 1;

    initialize(1, sample_rate);
}

APU::~APU() 
{ 
    out.close(); 
    stop();
}

bool APU::onGetData(Chunk& data)
{
    mutex.lock();

    // Run update as long as it takes to generate 1024 new samples.
    for (int i = 0; i < (cpu_frequency/sample_rate) * 1024; i++)
    {
        update();
    }
    
    data.sampleCount = 1024;
    data.samples = samples;

    mutex.unlock();

    return true;
}

void APU::onSeek(sf::Time timeOffset) 
{
    // Cannot be implemented.
}

// Called by memory when a channel is triggered. Triggering a channel activates it and refreshes certain registers.
void APU::trigger(int p_channel)
{
    if(p_channel == 1)
    {
        ch1_active = true;
        
        // Length timer.
        ch1_length_timer = memory.read(0xff11) & 0b111111;
        // Envelope.
        ch1_volume = memory.read(0xff12) >> 4;
        ch1_direction = (memory.read(0xff12) >> 3) & 1;
        ch1_initial_sweep_pace = memory.read(0xff12) & 0b111;
        // Sweep.
        ch1_duration_sweep_pace = (memory.read(0xff10) >> 4) & 0b111;
        
        memory.write(0xff26, memory.read(0xff26) | 0b0001);
    }
    if(p_channel == 2)
    {
        ch2_active = true;
        // Length timer;
        ch2_length_timer = memory.read(0xff16) & 0b111111;
        // Envelope.
        ch2_volume = memory.read(0xff17) >> 4;
        ch2_direction = (memory.read(0xff17) >> 3) & 1;
        ch2_initial_sweep_pace = memory.read(0xff17) & 0b111;
        memory.write(0xff26, memory.read(0xff26) | 0b0010);
    }
    if(p_channel == 3)
    {
        ch3_active = true;
        ch4_length_timer = memory.read(0xff1b);
        memory.write(0xff26, memory.read(0xff26) | 0b0100);
    }
    if(p_channel == 4)
    {
        ch4_active = true;
        ch4_length_timer = memory.read(0xff20) & 0b111111;
        ch4_volume = memory.read(0xff21) >> 4;
        memory.write(0xff26, memory.read(0xff26) | 0b1000);

        lfsr = 0;
    }
}

// 1 update = 1 dot
void APU::update() 
{
    // Count cylces per second.
    cycle_counter++;
    auto now = std::chrono::high_resolution_clock::now();
    if(std::chrono::duration_cast<std::chrono::milliseconds>(now - cycles_per_second_timer).count() >= 1000)
    {
        cycle_count_per_second = cycle_counter;
        cycle_counter = 0;
        cycles_per_second_timer = std::chrono::high_resolution_clock::now();
    }

    /*
    SOUND CHANNEL 1/2
    
    The channel has a 11 bit value describing its duration. The duration describes the frequency being played.
    Actually an internal counter counts from this duration up to 2048 (max number with 11 bits). Each increment
    takes 4 dots. Everytime 2048 is hit, the duty pointer will advance. 
    Apart from amplitude and duration describing the wave of this channel, there also is a duty cycle. Duty cycle 
    is a term to describe in % how long one period of a wave is high. A periode describes one low and one high part 
    of a digital wave. Their ratio  (always regarding the time the wave is high) is the duty cycle. 
    For the gameboy this waveform can be described with 8 numbers since the duty cycle can be 12.5%, 25%, 50% or  
    75%. The dutypointer is the index into where we are in this waveform/duty cycle. Like said, the duty pointer is 
    incremented everytime duration hits 2048 and a new sample is created. We look up the current duty cycle and the 
    duty pointer, getting a value of 0 or 1, amplifying that by multiplying some volume and finally saving it so 
    that the sample can be played back.
    */

    ch1_duty_cycle = memory.read(0xff11) >> 6;
    ch2_duty_cycle = memory.read(0xff16) >> 6;

    if(cycles % 4 == 0)
    {
        if(ch1_duration >= 2048)
        {
            ch1_duty_pointer++;
            if(ch1_duty_pointer >= 8)
                ch1_duty_pointer = 0;

            ch1_duration = (memory.read(0xff14) & 0b111) << 8 | memory.read(0xff13);
        }
        ch1_duration++;

        if(ch2_duration >= 2048)
        {
            ch2_duty_pointer++;
            if(ch2_duty_pointer >= 8)
                ch2_duty_pointer = 0;

            ch2_duration = (memory.read(0xff19) & 0b111) << 8 | memory.read(0xff18);
        }
        ch2_duration++;
    }

    /*
    SOUND CHANNEL 3
    */

    if(cycles % 2 == 0)
    {
        if(ch3_duration >= 2048)
        {
            ch3_sample_index++;
            if(ch3_sample_index >= 32) ch3_sample_index = 0;

            int wave_byte = memory.read(0xff30 + (ch3_sample_index / 2));
            ch3_current_sample = (wave_byte % 2 == 0) ? wave_byte >> 4 : wave_byte & 0b1111;

            ch3_duration = (memory.read(0xff1e) & 0b111) << 8 | memory.read(0xff1d);
        }
        ch3_duration++;
    }

    /*
    SOUND CHANNEL 4
    */

    int clock_shift = memory.read(0xff22) >> 4;
    int clock_divider = memory.read(0xff22) & 0b111;
    ch4_frequency_timer = 16.f * (clock_divider == 0 ? 0.5f : clock_divider) * std::pow(2, clock_shift);

    ch4_frequency_counter++;
    if(ch4_frequency_counter >= ch4_frequency_timer)
    {
        // Clock lfsr.
        ch4_out = lfsr & 1;

        int new_bit = ~(lfsr ^ (lfsr >> 1)) & 1;
        lfsr = (lfsr >> 1) | (new_bit << 14);
     
        // lfsr 7-bit mode.
        int lfsr_width = (memory.read(0xff22) >> 3) & 1;
        if(lfsr_width)
        {
            lfsr &= 0b10111111; // Reset bit 7.
            lfsr |= new_bit << 6; // Set bit 7 if new_bit is true.
        }

        ch4_frequency_counter = 0;
    }

    if(cycles >= cpu_frequency/512)
    {
        // APU counter is incremented at a rate of 512 Hz.
        div_apu++;

        lengthTimer();
        volumeChange();
        durationChange();

        if(div_apu >= 8)
        {
            div_apu = 0;
        }

        cycles = 0;
    }
    cycles++;

    sample_creation_cycles++;
    // Generate a new sample at the given sample rate.
    if(sample_creation_cycles >= cpu_frequency/sample_rate)
    {
        samples[sample_counter] = 0;
        // Channel 1.
        int ch1_sample = ch1_active ? (10000 * ((duty_cycles[ch1_duty_cycle][ch1_duty_pointer] * ch1_volume)/15.f)) : 0;
        samples[sample_counter] += ch1_sample;
        
        ch1_sample_list.push_back(ch1_sample);
        if(ch1_sample_list.size() > 3200) ch1_sample_list.erase(ch1_sample_list.begin());
        
        // Channel 2.
        int ch2_sample = ch2_active ? (10000 * ((duty_cycles[ch2_duty_cycle][ch2_duty_pointer] * ch2_volume)/15.f)) : 0;
        samples[sample_counter] += ch2_sample;
        
        ch2_sample_list.push_back(ch2_sample);
        if(ch2_sample_list.size() > 3200) ch2_sample_list.erase(ch2_sample_list.begin());
        
        // Channel 3.
        int ch3_sample = ch3_active ? (10000 * (ch3_current_sample / 15.f)) : 0;
        samples[sample_counter] += ch3_sample;

        ch3_sample_list.push_back(ch3_sample);
        if(ch3_sample_list.size() > 3200) ch3_sample_list.erase(ch3_sample_list.begin());

        // Channel 4.
        int ch4_sample = ch4_active ? (10000.f * (ch4_out * ch4_volume)/15.f) : 0;
        samples[sample_counter] += ch4_sample;

        ch4_sample_list.push_back(ch4_sample);
        if(ch4_sample_list.size() > 3200) ch4_sample_list.erase(ch4_sample_list.begin());

        // Output samples to a file.
        uint8_t msb = (samples[sample_counter] & 0xff00) >> 8;
        uint8_t lsb = samples[sample_counter] & 0xff;
        out.put(msb);
        out.put(lsb);

        sample_counter++;
        if(sample_counter >= 1024)
        {
            sample_counter = 0;
        }
        sample_creation_cycles = 0;
    }
}

int APU::getDigitalOutput(int p_channel)
{
    // Each channel produces a digital output ranging from 0x00 to 0x0f.    
    switch(p_channel)
    {
        case 1: return duty_cycles[ch1_duty_cycle][ch1_duty_pointer] * ch1_volume;
        case 2: return duty_cycles[ch2_duty_cycle][ch2_duty_pointer] * ch2_volume;
        case 3: return ch3_current_sample;
        case 4: return ch4_out * ch4_volume;
        default:
        std::cout << "Incorrect channel number!" << std::endl;
        return 0;
    }
}

float APU::getAnalogOutput(int p_channel)
{
    return ((getDigitalOutput(p_channel)/15.f) * 2.f) - 1;
}

void APU::lengthTimer()
{
    // Length timer is incremented at a rate of 256 Hz.
    if((div_apu % 2) == 0)
    {
        // Channel 1.
        if((memory.read(0xff14) >> 6 & 1) == 1)
        {
            ch1_length_timer++;
            if(ch1_length_timer == 64) 
            {
                ch1_active = false;
                memory.write(0xff26, memory.read(0xff26) & 0b11111110);
            }
        }
        // Channel 2.
        if((memory.read(0xff19) >> 6 & 1) == 1)
        {
            ch2_length_timer++;
            if(ch2_length_timer == 64) 
            {
                ch2_active = false;
                memory.write(0xff26, memory.read(0xff26) & 0b11111101);
            }
        }
        // Channel 3.
        if((memory.read(0xff1e) >> 6 & 1) == 1)
        {
            ch3_length_timer++;
            if(ch3_length_timer == 64)
            {
                ch3_active = false;
                memory.write(0xff26, memory.read(0xff26) & 0b11111011);
            }
        }
        // Channel 4.
        if((memory.read(0xff23) >> 6 & 1) == 1)
        {
            ch4_length_timer++;
            if(ch4_length_timer == 64) 
            {
                ch4_active = false;
                memory.write(0xff26, memory.read(0xff26) & 0b11110111);
            }
        }
    }
}

void APU::volumeChange()
{
    // Volume change is clocked at a rate of 64 Hz.
    if(div_apu == 7)
    {
        // Channel 1.
        if(ch1_initial_sweep_pace > 0)
        {
            ch1_sweep_pace++;

            if(ch1_sweep_pace >= ch1_initial_sweep_pace)
            {
                ch1_volume += (ch1_direction ? 1 : -1);
                if(ch1_volume < 0) ch1_volume = 0;
                if(ch1_volume > 15) ch1_volume = 15;
                ch1_sweep_pace = 0;
            }
        }

        // Channel 2.
        if(ch2_initial_sweep_pace > 0)
        {
            ch2_sweep_pace++;

            if(ch2_sweep_pace >= ch2_initial_sweep_pace)
            {
                ch2_volume += (ch2_direction ? 1 : -1);
                if(ch2_volume < 0) ch2_volume = 0;
                if(ch2_volume > 15) ch2_volume = 15;
                ch2_sweep_pace = 0;
            }
        }

        // Channel 4.
        int ch4_direction = (memory.read(0xff21) >> 3) & 1;
        int ch4_initial_sweep_pace = memory.read(0xff21) & 0b111;

        if(ch4_initial_sweep_pace > 0)
        {
            ch4_sweep_pace++;

            if(ch4_sweep_pace >= ch4_initial_sweep_pace)
            {
                ch4_volume += (ch4_direction ? 1 : -1);
                if(ch4_volume < 0) ch4_volume = 0;
                if(ch4_volume > 15) ch4_volume = 15;
                ch4_sweep_pace = 0;
            }
        }
    }
}

void APU::durationChange()
{
    // Duration sweep is clocked at 128 Hz.
    if(div_apu == 2 || div_apu == 6)
    {
        int ch1_sweep_direction = (memory.read(0xff10) >> 3) & 1;
        int ch1_sweep_slope = memory.read(0xff10) & 0b111;

        // Only do sweep iterations/duration changes, if sweep pace is not zero.
        if((memory.read(0xff10) >> 4) & 0b111 != 0)
        {
            ch1_duration_sweep_pace--;
            if(ch1_duration_sweep_pace <= 0)
            {
                int old_duration = (memory.read(0xff14) & 0b111) << 8 | memory.read(0xff13);
                int new_duration = old_duration + (ch1_sweep_direction ? -1 : 1) * old_duration / std::pow(2, ch1_sweep_slope);

                if(new_duration > 0x7ff)
                {
                    ch1_active = false;
                }
                else
                {
                    // Add/subtract to the current duration in NR13/NR14 if slope (n) is greater 0.
                    if(ch1_sweep_slope > 0)
                    {
                        memory.write(0xff14, (memory.read(0xff14) & 0b11111000) | (new_duration >> 8));
                        memory.write(0xff13, new_duration & 0xff);
                    }
                }

                ch1_duration_sweep_pace = (memory.read(0xff10) >> 4) & 0b111;
            }
        }
    }
}
