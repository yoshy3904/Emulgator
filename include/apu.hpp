#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <thread>
#include <fstream>
#include <iostream>
#include <mutex>
#include <atomic>

#include "memory.hpp"

// class SoundWaveStream : public sf::SoundStream
// {
// private:
//     const unsigned int sample_rate = 44100;
//     const unsigned int samples = 750;
//     const float two_pi = 6.28318f;
//     const float increment = 1.0f/44100.0f;
//     float virtualTime = 0.f;
//     float buffer_x = 0.f;

//     sf::Int16 sample_array[22500];

//     Application& app;
//     sf::Image buffer;
//     sf::Texture texture;
//     sf::Sprite sprite;
// public:
//     float frequency;
//     float duty_cycle = 0.5f;
//     float amplitude = 10000;

// public:
//     SoundWaveStream(Application& p_app) : app(p_app), frequency(400.f)
//     {
//         buffer.create(1300.f, 100.f);
//         texture.loadFromImage(buffer);
//         sprite.setTexture(texture);

//         sprite.setPosition(sf::Vector2f(330.f, 63.f));

//         initialize(1, sample_rate);
//     }

//     void update()
//     {
//         app.window.draw(sprite);
//     }

//     virtual bool onGetData(Chunk& data)
//     {
//         for (int i = 0; i < samples; i++) 
//         {
//             // Calculate next sample for sound wave.
//             // A square wave can be constructed out of two saw-tooth waves offsetted and subtracted from another.
//             // float a = 0.f;
//             // float b = 0.f;
//             // float p = duty_cycle * two_pi;
//             // for (float n = 1; n < 40; n++)
//             // {
//             //     a += sin(n * virtualTime * two_pi * frequency) / n;
//             //     b += sin(n * (frequency * virtualTime - p) * two_pi) / n;
//             // }
//             // sample_array[i] = amplitude * (a - b);
//             // virtualTime += increment;
            
//             float a = 0.f; // First saw tooth wave.
//             float b = 0.f; // Second saw tooth wave.
//             float p = duty_cycle * two_pi; // Offset between both waves.
//             for (float n = 1; n < 40; n++) // Calculate current value for the waves depending on the time variable.
//             {
//                 float c = n * frequency * two_pi * virtualTime;
//                 a += sin(c) / n;
//                 b += sin(c - p * n) / n;
//             }
//             sample_array[i] = amplitude * (a - b); // Subtract and amplify the waves.
//             virtualTime += increment;

//             // Calculate x position in buffer.
//             buffer_x += 1.f;
//             // Clear buffer on overflow.
//             if(buffer_x >= 1200)
//             {
//                 buffer_x = 0;
//                 for (int y = 0; y < 100; y++)
//                 {
//                     for (int x = 0; x < 1300; x++)
//                     {
//                         buffer.setPixel(x, y, sf::Color::Black);   
//                     }
//                 }
//             }
//             // Draw to buffer.
//             buffer.setPixel(buffer_x, 50.f + ((sample_array[i] / amplitude) * 10.f), sf::Color::Magenta);
//         }
//         texture.loadFromImage(buffer);
//         sprite.setTexture(texture);
        
//         data.samples = sample_array;
//         data.sampleCount = samples;
//         return true;
//     }

//     virtual void onSeek(sf::Time timeOffset) {}
// };

/*
Sound Channel 1 (Pulse)

*/

class APU : public sf::SoundStream
{
public:
    sf::Int16 samples[1024];
    mutable std::vector<sf::Int16> ch1_sample_list;
    mutable std::vector<sf::Int16> ch2_sample_list;
    mutable std::vector<sf::Int16> ch3_sample_list;
    mutable std::vector<sf::Int16> ch4_sample_list;
    bool duty_cycles[4][8];
    
    // Channel 1.
    bool ch1_active = false;
    int ch1_duty_pointer = 0;
    uint8_t ch1_duty_cycle = 0;
    uint16_t ch1_duration = 0;
    int ch1_length_timer = 0;

    int ch1_volume = 0;
    int ch1_sweep_pace = 0;
    int ch1_direction = 0;
    int ch1_initial_sweep_pace = 0;

    int ch1_duration_sweep_pace = 0;

    // Channel 2.
    bool ch2_active = false;
    int ch2_duty_pointer = 0;
    uint8_t ch2_duty_cycle = 0;
    uint16_t ch2_duration = 0;
    int ch2_length_timer = 0;

    int ch2_volume = 0;
    int ch2_sweep_pace = 0;
    int ch2_direction = 0;
    int ch2_initial_sweep_pace = 0;

    // Channel 3.
    bool ch3_active = false;
    int ch3_sample_index = 0;
    int ch3_duration = 0;
    int ch3_current_sample = 0;
    int ch3_length_timer = 0;

    // Channel 4.
    bool ch4_active = false;
    int ch4_length_timer = 0;
    uint16_t lfsr = 0;
    int ch4_out = 0;

    int ch4_frequency_timer = 1;
    int ch4_frequency_counter = 0;

    int ch4_volume = 0;
    int ch4_sweep_pace = 0;

    int ch4_debug_counter = 0;

    // General.
    int sample_creation_cycles = 0;
    uint64_t cycles = 0;

    int sample_rate = 48000;
    int sample_batch_size = 1024;
    int sample_counter = 0;

    int cpu_frequency = 4194304.f;
    int div_apu = 0;

    uint64_t cycle_count_per_second = 0;
    uint64_t cycle_counter = 0;
    std::chrono::_V2::system_clock::time_point cycles_per_second_timer;
private:
    Memory& memory;
    std::mutex& mutex;
    std::ofstream out;
public:
    APU(Memory& p_emulator, std::mutex& p_mutex);
    ~APU();

    void trigger(int p_channel);
    void update();

    virtual bool onGetData(Chunk& data);
    virtual void onSeek(sf::Time timeOffset);

    int getDigitalOutput(int p_channel);
    float getAnalogOutput(int p_channel);
private:
    void lengthTimer();
    void volumeChange();
    void durationChange();
};
