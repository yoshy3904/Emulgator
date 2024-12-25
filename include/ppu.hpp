#pragma once
#include "SHARP_LR35902.hpp"
#include "memory.hpp"

#include <stdint.h>
#include <vector>
#include <array>
#include <string>

class SHARP_LR35902;
class Memory;

/*
PPU (Picture Processing Unit)

The PPU controls the display of the gameboy. The display is a LCD (Liquid Crystal Display) with a resolution of 160x144 pixels.
The original gameboy had a LCD with a size of 4,7 x 4,3cm.

Display Timing

The PPU can be in 4 different states when drawing a frame. Every 17556 cycles a full frame is drawn to the LCD. This corresponds to
about 60 fps (59.73).

The LY (LCD Y Coordinate) register keeps track of the current horizontal line that is about to be drawn, is being drawn, or has just been drawn. Its value
is stored at 0xff44, readonly. LY interates from 0 to 153.

During the lines 0-143 the PPU is in mode 0, 2 or 3.
At the start of each line the PPU is in mode 2 for 80 cycles. It is searching for objects/sprites on the OAM.
Then it enters into mode 3 for 172-289 cycles. Now it is drawing to the LCD.
Then it enters into mode 0 for 87-204 cycles. This marks the HBLANK period.
Drawing a full line takes 456 cycles in total.
During the lines 144-153 the PPU is in mode 1. Mode 1 is the VBLANK period and takes 4560 cycles.
*/

/* Handling timing, creation of frames, registers. */
class PPU
{
public:
    struct Object
    {
        uint8_t x;
        uint8_t y;
        uint8_t tile_index;
        bool priority;
        bool x_flip;
        bool y_flip;
        bool palette_number;
    };
    struct Pixel
    {
        int color = 0; // Value between 0-3.
        int palette = 0;
        bool background_priority = false;
    };
    enum LCDC
    {
        LCD_AND_PPU_ENABLE              = 1 << 7, // Turns display on or off.
        WINDOW_TILE_MAP                 = 1 << 6, // Selects the tile view to be used for the window.
        WINDOW_ENABLE                   = 1 << 5, // Whether the window should be drawn to the LCD.
        BG_AND_WIN_TILE_DATA            = 1 << 4, // Whether tile data should be read from area 1 or 2.
        BG_TILE_MAP                     = 1 << 3, // Selects the tile view to be used for the background: 0 = 9800, 1 = 9C00.
        OBJECT_SIZE                     = 1 << 2, // Whether tiles are 8x16 (1) or 8x8 (0).
        OBJECT_ENABLE                   = 1 << 1, // Whether objects should be displayed.
        BG_AND_WINDOW_ENABLE            = 1 << 0, // Whether the background and window should be drawn to the LCD.
    };
    enum STAT
    {
        LYC_EQUALS_LY_INTERRUPT         = 1 << 6, 
        OAM_STAT_INTERRUPT              = 1 << 5,
        VBLANK_STAT_INTERRUPT           = 1 << 4,
        HBLANK_STAT_INTERRUPT           = 1 << 3,
        LY_EQUALS_LYC                   = 1 << 2
    };
    enum LCDMODE
    {
        HBLANK = 0b00, VBLANK = 0b01, OAMSCAN = 0b10, DRAWING_PIXELS = 0b11
    };
    enum PALETTE
    {
        BGP = 0, OBP0 = 1, OBP1 = 2
    };

    const int screen_width = 160;
    const int screen_height = 144;
    const int actual_screen_width = 256;
    const int actual_screen_height = 256;

    std::vector<uint8_t> screen_buffer;
    std::vector<uint8_t> background_buffer;
    std::vector<uint8_t> window_buffer;
    std::array<Object, 40> objects;
private:
    uint32_t cycles = 0;

    Memory& memory;
    SHARP_LR35902& cpu;

    std::array<uint32_t, 4> color_palette;

    const uint16_t tile_data_01_pointer = 0x8000;
    const uint16_t tile_data_12_pointer = 0x9000;
    const uint16_t tile_map_1_pointer = 0x9800;
    const uint16_t tile_map_2_pointer = 0x9c00;

    const uint16_t oam_start = 0xfe00;
    const uint16_t oam_end = 0xfe9f;


public:
    std::vector<Pixel> fifo;
    int output_x = -8;
    int output_y = 0;
    bool lock_fifo = false;
    int fetcher_x = 0;
    int fetcher_y = 0;

    std::vector<Object> objects_on_scanline;

    int fetcher_cycles = 0;
    uint8_t scroll_x = 0;
    uint8_t scroll_y = 0;
    std::string info_text;

    uint8_t window_x = 0;
    uint8_t window_y = 0;
    uint8_t window_internal_counter = 0;

    bool window_on_scanline = false;
    bool window_triggered = false;

public:
    PPU(Memory& p_memory, SHARP_LR35902& p_cpu);
    
    void setLCDCBit(LCDC p_mask, bool p_state);
    bool getLCDCBit(LCDC p_mask) const;

    void setSTATBit(STAT p_mask, bool p_state);
    bool getSTATBit(STAT p_mask) const;

    void setLCDMode(LCDMODE p_state);
    LCDMODE getLCDMode() const;

    const std::array<uint32_t, 4>& getColorPalette() const;
    uint32_t getCycleCount() const;

    void processScreenBuffers();
    void update();
    
    void getTile(uint16_t p_address, std::vector<uint8_t>& p_pixels) const;
private:
    void tryFetchingObject(int p_x);
    void fetchTile();

    void writeTileToBuffer(int start_x, int start_y, std::vector<uint8_t>& tile, std::vector<uint8_t>& buffer, int buffer_size_x, int buffer_size_y, bool is_transparent, bool flip_x, bool flip_y); 
    void writeTileMapToBuffer(bool p_tile_map, bool p_tile_data, std::vector<uint8_t>& buffer, int buffer_size_x, int buffer_size_y);
    void writeOAMToBuffer();
};