#include "ppu.hpp"
#include <iostream>

// void DisplayData::drawSpritesToBuffer(std::vector<uint8_t>& buffer) 
// {
//     updateSprites();
//     for (int i = 0; i < sprites.size(); i++)
//     {
//         std::vector<uint8_t> pixels;
//         getTile(tile_data_start_1 + sprites[i].tile_index * 16, pixels);
//         writeTileToBuffer(sprites[i].x - 8, sprites[i].y - 16, pixels, buffer, 160, 144, true, sprites[i].x_flip, sprites[i].y_flip);
//     }
// }

// void DisplayData::updateSprites() 
// {
//     for (uint16_t i = oam_start; i < oam_end; i+=4)
//     {
//         uint8_t oam_entry_index = (i - oam_start) / 4;
//         sprites[oam_entry_index].y = memory.read(i + 0, false);
//         sprites[oam_entry_index].x = memory.read(i + 1, false);
//         sprites[oam_entry_index].tile_index = memory.read(i + 2, false);
//         sprites[oam_entry_index].y_flip = memory.read(i + 3, false) & 0b01000000;
//         sprites[oam_entry_index].x_flip = memory.read(i + 3, false) & 0b00100000;
//     }
// }

// void DisplayData::writeViewToBuffer(uint8_t tileview_index, uint8_t tiledata_index, std::vector<uint8_t>& buffer, int buffer_size_x, int buffer_size_y) 
// {
//     uint16_t view_start_address;
//     uint16_t view_end_address;

//     if(tileview_index == 0)
//     {
//         view_start_address = tile_view_start_1;
//         view_end_address = tile_view_end_1;
//     }
//     else if(tileview_index == 1)
//     {
//         view_start_address = tile_view_start_2;
//         view_end_address = tile_view_end_2;
//     }

//     int tileXPos = 0;
//     int tileYPos = 0;
//     std::vector<uint8_t> pixels; // Stores the current 8x8-pixel tile.
//     for (int i = view_start_address; i < view_end_address; i++)
//     {
//         // Populate the pixels array. memory.read(i) is the tile_index. Adding 0x8000 and multiplying 16 gives us the starting address of our next tile.
//         uint16_t tile_address = tiledata_index ? 0x8000 + memory.read(i, false) * 16 : 0x9000 + (int8_t)memory.read(i, false) * 16;
//         getTile(tile_address, pixels); 

//         writeTileToBuffer(tileXPos * 8, tileYPos * 8, pixels, buffer, buffer_size_x, buffer_size_y);
//         tileXPos++;
//         if(tileXPos >= 32)
//         {
//             tileXPos = 0;
//             tileYPos++;
//         }
//     }
// }

// void DisplayData::writeTileToBuffer(int start_x, int start_y, std::vector<uint8_t>& pixels, std::vector<uint8_t>& buffer, int buffer_size_x, int buffer_size_y, bool is_transparent, bool flip_x, bool flip_y) 
// {
//     if(pixels.size() != 64) return;
//     if(buffer.size() != buffer_size_x * buffer_size_y) return;

//     for (int y = 0; y < 8; y++)
//     {
//         for (int x = 0; x < 8; x++)
//         {
//             int flipped_x = x, flipped_y = y;
//             if(flip_y)
//             {
//                 flipped_y = 7 - y;
//             }
//             else if(flip_x)
//             {
//                 flipped_x = 7 - x;
//             }

//             uint8_t current_pixel = pixels[flipped_y * 8 + flipped_x];

//             if((current_pixel == 0) && is_transparent) continue;

//             uint8_t offset_x = start_x + x;
//             uint8_t offset_y = start_y + y;
//             if(offset_y < buffer_size_y && offset_x < buffer_size_x)
//             {
//                 buffer[offset_y * buffer_size_x + offset_x] = current_pixel;
//             }
//         }
//     }
// }

PPU::PPU(Memory& p_memory, SHARP_LR35902& p_cpu) 
    : memory(p_memory), cpu(p_cpu),
    screen_buffer(screen_width * screen_height),
    background_buffer(actual_screen_width * actual_screen_height),
    window_buffer(actual_screen_width * actual_screen_height)
{
    // RGBA Colors: Day & Night Color Scheme.
    color_palette[3] = 0x011a27ff;
    color_palette[2] = 0x063852ff;
    color_palette[1] = 0xf0810fff;
    color_palette[0] = 0xe6df44ff;

    // RGBA Colors: Blue.
    // color_palette[3] = 0x1e3d58ff;
    // color_palette[2] = 0x057dcdff;
    // color_palette[1] = 0x43b0f1ff;
    // color_palette[0] = 0xe8eef1ff;

    // Grey Scale.
    // color_palette[3] = 0x00261cff; // Dark green.
    // color_palette[2] = 0x044d29ff;
    // color_palette[1] = 0x168039ff;
    // color_palette[0] = 0x45bf55ff; // Light green.

    // Grey Scale.
    // color_palette[3] = 0x202020ff; // Dark.
    // color_palette[2] = 0x606060ff;
    // color_palette[1] = 0x9f9f9fff;
    // color_palette[0] = 0xdfdfdfff; // Light.

}

void PPU::setLCDCBit(LCDC mask, bool p_state) 
{
    uint8_t byte = memory.read(0xff40);
    if(p_state == true)
    {
        byte |= mask;
    }
    else
    {
        byte &= ~mask;
    }
    memory.write(0xff40, byte);
}

bool PPU::getLCDCBit(LCDC mask) const
{
    return mask & memory.read(0xff40);
}

void PPU::setSTATBit(STAT mask, bool p_state) 
{
    uint8_t byte = memory.read(0xff41);
    if(p_state == true)
    {
        byte |= mask;
    }
    else
    {
        byte &= ~mask;
    }
    memory.write(0xff41, byte);    
}

void PPU::setLCDMode(LCDMODE state) 
{
    memory.write(0xff41, (memory.read(0xff41) >> 2) << 2 | state, false);
}

PPU::LCDMODE PPU::getLCDMode() const
{
    return (LCDMODE)(memory.read(0xff41) & 0b00000011);
}

bool PPU::getSTATBit(STAT mask) const
{
    return mask & memory.read(0xff41);    
}

const std::array<uint32_t, 4>& PPU::getColorPalette() const
{
    return color_palette;
}

uint32_t PPU::getCycleCount() const
{
    return cycles;
}

void PPU::processScreenBuffers() 
{
    writeTileMapToBuffer(getLCDCBit(LCDC::BG_TILE_MAP), getLCDCBit(LCDC::BG_AND_WIN_TILE_DATA), background_buffer, actual_screen_width, actual_screen_height);
    writeTileMapToBuffer(getLCDCBit(LCDC::WINDOW_TILE_MAP), getLCDCBit(LCDC::BG_AND_WIN_TILE_DATA), window_buffer, actual_screen_width, actual_screen_height);
}

void PPU::update() 
{
    /*
    The PPU is updated at a frequency of ~4 MHz. Each call to this function is called a "dot" (-> 4 million dots happen in one second).
    One frame takes 70224 dots. 154 scanlines have to be processed in this time. Each scanline takes 456 dots to finish processing.

    PPU Modes
    OAM Scan -> Collects all objects (max 10 entries) which are visible in the scanline. This takes 2 dots per object which results in 80 dots in total.
    Drawing Pixels -> Pixels are drawn to the screen. Length depends on what has to be drawn.
    Horizontal Blank -> Wait.
    Vertical Blank -> Wait.

    Implemented:
    - Background tiles rendering
    - STAT Interrupts
    - Basic object rendering
    - Palettes
    - Object flipping x, y
    - Object drawing priority
    - Object enable
    - Objects with positions 0-8
    - Window
    */

    // PPU Modes.
    if(memory.read(0xff44) >= 144) 
    {
        setLCDMode(VBLANK);
        fetcher_y = 0;
        output_y = 0;
        window_on_scanline = false;
    }
    else
    {
        if(cycles == 0)
        {
            setLCDMode(OAMSCAN);
            
            // Find all objects on the current scanline.
            writeOAMToBuffer();
            objects_on_scanline.clear();
            for (int i = 0; i < objects.size(); i++)
            {
                int actual_object_height = objects[i].y - 16;
                if(actual_object_height >= fetcher_y - (getLCDCBit(LCDC::OBJECT_SIZE) ? 15 : 7) && actual_object_height <= fetcher_y)
                {
                    objects_on_scanline.push_back(objects[i]);
                    if(objects_on_scanline.size() == 10) break;
                }
            }
            
            // Read how much the screen is scrolled at the start of each scanline.
            scroll_x = memory.read(0xff43);
            scroll_y = memory.read(0xff42);

            // Read window position (maybe the ppu actually reads those registers somewhere else?).
            window_x = memory.read(0xff4b);
            window_y = memory.read(0xff4a);

            if(window_triggered) window_internal_counter++;
            window_triggered = false;

            if(memory.read(0xff44) == memory.read(0xff4a)) // LY = WY.
            {
                window_on_scanline = true; 
                window_internal_counter = 0;
            }
        }
        if(cycles == 80)
        {
            setLCDMode(DRAWING_PIXELS);

            // Push 8 empty pixels to the fifo at the start of each line. Prefechting this empty tile is necessary due to objects that might be positioned at 0-8 (-8 to -1 in screen coordinates).
            for (int i = 0; i < 8; i++)
            {
                fifo.push_back({});
            }
        }
    }
    
    if(getLCDMode() == DRAWING_PIXELS)
    {
        // Draw window.
        if(getLCDCBit(LCDC::WINDOW_ENABLE) && window_on_scanline && !window_triggered && output_x == memory.read(0xff4b) - 7)
        {
            // Reset fetch.
            fetcher_cycles = 0;
            fetcher_x = memory.read(0xff4b) / 8;
            // Clear fifo.
            fifo.clear();

            window_triggered = true;
        }

        // Fetcher. 
        if(fetcher_cycles == 0)
        {
            // Another 8 pixels are pulled every 8 dots (2 dots tile read, 2 dots read tiledata low, 2 dots readtile data high, 2 dots sleep/waiting until the FIFO has only 8 pixels left). 
            fetchTile();

            // Remove front pixels at start of scanline for scrolling.
            if(cycles == 80)
            {
                int pixels_to_erase = scroll_x % 8;
                for (int i = 0; i < pixels_to_erase; i++)
                {
                    fifo.erase(fifo.begin());
                }
            }

            fetcher_x++;
            fetcher_cycles = 8;
        }

        fetcher_cycles--;

        // Push to LCD if there are at least 8 pixels in the FIFO.
        if(fifo.size() > 8)
        {
            // Object fetcher.
            tryFetchingObject(output_x);

            // Apply palettes.
            uint8_t pixel_palette = 0;
            if(fifo.front().palette == PALETTE::BGP) pixel_palette = memory.read(0xff47);
            if(fifo.front().palette == PALETTE::OBP0) pixel_palette = memory.read(0xff48);
            if(fifo.front().palette == PALETTE::OBP1) pixel_palette = memory.read(0xff49);
            uint8_t palette_color = (pixel_palette >> (fifo.front().color * 2)) & 0b11;

            // Push to LCD.
            if(output_x >= 0) // The first 8 empty pixels of each scanline won't be visible and are therefore just discarded.
            {
                screen_buffer[output_x + output_y * 160] = palette_color;
            }
            fifo.erase(fifo.begin());

            output_x++;

            // End of drawing pixels to this scanline. Discard unnecessary data.
            if(output_x >= 160)  
            {
                output_x = -8;
                output_y++;
                setLCDMode(HBLANK);

                if(getSTATBit(STAT::HBLANK_STAT_INTERRUPT))
                {
                    cpu.requestInterrupt(SHARP_LR35902::Interrupt::LCD_STAT);
                }

                fifo.clear();
                fetcher_cycles = 0;
                fetcher_x = 0;
                fetcher_y++;
            }
        }
    }

    // STAT Interrupts.
    if(cycles == 0 && getSTATBit(STAT::OAM_STAT_INTERRUPT))
    {
        cpu.requestInterrupt(SHARP_LR35902::Interrupt::LCD_STAT);
    }
    if(cycles == 0 && (memory.read(0xff44) == memory.read(0xff45)) && getSTATBit(STAT::LYC_EQUALS_LY_INTERRUPT)) // Request only at the start of each scanline.
    {
        cpu.requestInterrupt(SHARP_LR35902::Interrupt::LCD_STAT);
    }

    // Interrupts on entering VBLANK period.
    if(cycles == 0 && memory.read(0xff44) == 144)
    {
        cpu.requestInterrupt(SHARP_LR35902::VBLANK);

        if(getSTATBit(STAT::VBLANK_STAT_INTERRUPT))
        {
            cpu.requestInterrupt(SHARP_LR35902::Interrupt::LCD_STAT);
        }
    }

    cycles++;

    // Scanline ends.
    if(cycles == 456)
    {
        // Increment LY or wrap it back to 0.
        memory.write(0xff44, memory.read(0xff44) + 1, false);
        if(memory.read(0xff44) >= 154)
        {
            memory.write(0xff44, 0, false);
        }

        // Check if LY = LYC.
        setSTATBit(LY_EQUALS_LYC, memory.read(0xff45) == memory.read(0xff44));
        
        // Reset cycles.
        cycles = 0;
    }
}

void PPU::getTile(uint16_t p_address, std::vector<uint8_t>& p_pixels) const 
{
    if(p_pixels.size() != 64)
    {
        p_pixels.resize(64, 0);
    }
    int row = 0;
    for (int i = 0; i < 16; i+=2)
    {
        uint8_t current_byte = memory.read(p_address + i, false);
        uint8_t next_byte = memory.read(p_address + i + 1, false);

        // Loop over each bit.
        for(int j = 7; j >= 0; j--)
        {
            uint8_t msbit = (next_byte >> j) & 1;
            uint8_t lsbit = (current_byte >> j) & 1;
            p_pixels[row * 8 + (7 - j)] = (msbit << 1) | lsbit;
        }
        row++;
    }
}

void PPU::writeTileMapToBuffer(bool p_tile_map, bool p_tile_data, std::vector<uint8_t>& buffer, int buffer_size_x, int buffer_size_y)
{
    if(buffer.size() != buffer_size_x * buffer_size_y)
    {
        std::cout << "Buffer size doesn't match!" << std::endl;
        return;
    }

    std::vector<uint8_t> tile;
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            uint16_t tile_map_pointer = p_tile_map ? tile_map_2_pointer : tile_map_1_pointer;
            uint8_t tile_index = memory.read(tile_map_pointer + (y * 32 + x), false);
            uint16_t tile_base_pointer = p_tile_data ? tile_data_01_pointer + tile_index * 16 : tile_data_12_pointer + (int8_t)tile_index * 16;
            getTile(tile_base_pointer, tile);

            writeTileToBuffer(x * 8, y * 8, tile, buffer, buffer_size_x, buffer_size_y, false, false, false);
        }
    }
}

void PPU::tryFetchingObject(int p_x)
{
    // Object fetcher. Object fetching has to be checked on every pixel. I couldn't find reliable resources about object rendering timing. 
    // (Important to call after BG fetcher, since the BG fetcher can erase pixels from the fifo which might also erase object data if the object 
    // was "merged" onto the lower 8 pixels in the fifo before.)
    if(getLCDCBit(LCDC::OBJECT_ENABLE))
    {
        for (int i = 0; i < objects_on_scanline.size(); i++)
        {
            int actual_object_height = objects_on_scanline[i].y - 16;
            if(objects_on_scanline[i].x - 8 == p_x)
            {
                uint8_t tile_index = 0;
                uint16_t tile_data_pointer = 0;
                int tile_row = 0;
                int tiledata_low = 0;
                int tiledata_high = 0;
                if(getLCDCBit(LCDC::OBJECT_SIZE))
                {
                    tile_index = objects_on_scanline[i].tile_index & 0xfe;
                    tile_data_pointer = 0x8000 + tile_index * 16;
                    tile_row = objects_on_scanline[i].y_flip ? (15 - (fetcher_y - actual_object_height)) : (fetcher_y - actual_object_height);

                    if(tile_row < 8)
                    {
                        tiledata_low = memory.read(tile_data_pointer + 2 * tile_row, false);
                        tiledata_high = memory.read(tile_data_pointer + 2 * tile_row + 1, false);
                    }
                    else
                    {
                        tiledata_low = memory.read(tile_data_pointer + 16 + 2 * (tile_row - 8), false);
                        tiledata_high = memory.read(tile_data_pointer + 16 + 2 * (tile_row - 8) + 1, false);
                    }
                }
                else
                {
                    tile_index = objects_on_scanline[i].tile_index;
                    tile_data_pointer = 0x8000 + tile_index * 16;
                    tile_row = objects_on_scanline[i].y_flip ? (7 - (fetcher_y - actual_object_height)) : (fetcher_y - actual_object_height);

                    tiledata_low = memory.read(tile_data_pointer + 2 * tile_row, false);
                    tiledata_high = memory.read(tile_data_pointer + 2 * tile_row + 1, false);
                }

                for(int j = 0; j < 8; j++)
                {
                    uint8_t msbit = (tiledata_high >> j) & 1;
                    uint8_t lsbit = (tiledata_low >> j) & 1;
                    uint8_t color = (msbit << 1) | lsbit;
                    
                    int pixel_index = objects_on_scanline[i].x_flip ? j : (7 - j);
                    if(pixel_index >= fifo.size()) continue;
                    if(objects_on_scanline[i].priority && fifo[pixel_index].color != 0) continue; // Draw BG + Window colors 1-3 over objects if OBJ-to-BG priority is enabled.
                    if(color != 0 && fifo[pixel_index].palette == PALETTE::BGP) // Only draw the object if the underlying pixel is from the background (essentially, the first sprite that comes will stay -> sprites later in oam or with higher x position will be drawn below this sprite). Also don't draw transparent pixels (color 0b00).
                    {
                        fifo[pixel_index].color = color;
                        fifo[pixel_index].palette = objects_on_scanline[i].palette_number ? PALETTE::OBP1 : PALETTE::OBP0;
                        fifo[pixel_index].background_priority = objects_on_scanline[i].priority;
                    }
                }
            }
        }
    }
}

void PPU::fetchTile()
{
    uint16_t current_tilemap_address = 0;
    int tile_x = 0; // Fetcher keeps track of the tile it is on.
    int tile_y = 0;
    int tile_row = 0; // We now know the tile we want to push data from to the FIFO next, but we also need to know which row of the tile overlaps the current scanline.
    if(window_triggered)
    {
        current_tilemap_address = getLCDCBit(LCDC::WINDOW_TILE_MAP) ? 0x9c00 : 0x9800;
        tile_x = (fetcher_x - (memory.read(0xff4b) / 8)) & 0x1f; // Fetcher keeps track of the tile it is on.
        tile_y = (window_internal_counter & 255) / 8;
        tile_row = (window_internal_counter & 255) % 8;
    }
    else
    {
        current_tilemap_address = getLCDCBit(LCDC::BG_TILE_MAP) ? 0x9c00 : 0x9800;
        tile_x = ((scroll_x / 8) + fetcher_x) & 0x1f; // Fetcher keeps track of the tile it is on.
        tile_y = ((scroll_y + fetcher_y) & 255) / 8;
        tile_row = ((scroll_y + fetcher_y) & 255) % 8;
    }
    uint16_t current_tile_address = current_tilemap_address + (tile_x + 32 * tile_y);
    uint16_t tile_data_pointer = getLCDCBit(LCDC::BG_AND_WIN_TILE_DATA) ? (0x8000 + memory.read(current_tile_address, false) * 16) : (0x9000 + (int8_t)memory.read(current_tile_address, false) * 16);

    // Since each tile consists of 16 bytes of data, a row of 8 pixels has 2 bytes data.
    int tiledata_low = memory.read(tile_data_pointer + 2 * tile_row, false); // Reading VRAM is locked during Mode 3, but not for the PPU since it has to draw something. Therefore restrictions can be disabled for a software component.
    int tiledata_high = memory.read(tile_data_pointer + 2 * tile_row + 1, false);

    // Convert the byte data to color information and push the new pixels to the FIFO.
    for(int j = 7; j >= 0; j--)
    {
        uint8_t msbit = (tiledata_high >> j) & 1;
        uint8_t lsbit = (tiledata_low >> j) & 1;
        fifo.push_back( { getLCDCBit(LCDC::BG_AND_WINDOW_ENABLE) ? ((msbit << 1) | lsbit) : 0, PALETTE::BGP, false } );
    }

    info_text = std::to_string(tile_x) + ", " + std::to_string(tile_y) + ": TileMap " + std::to_string(current_tile_address) + " Offset " + std::to_string(memory.read(current_tile_address, false)) + " TilePointer " + std::to_string(tile_data_pointer) + " TileRow " + std::to_string(tile_row) + " TileData " + std::to_string(tiledata_low) + ", " + std::to_string(tiledata_high) + "\n";
}

void PPU::writeTileToBuffer(int start_x, int start_y, std::vector<uint8_t>& tile, std::vector<uint8_t>& buffer, int buffer_size_x, int buffer_size_y, bool is_transparent, bool flip_x, bool flip_y)
{
    if(tile.size() != 64 || buffer.size() != buffer_size_x * buffer_size_y)
    {
        std::cout << "Wrong tile size or buffer doesn't match size!" << std::endl;
        return;
    }

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            int flipped_x = flip_x ? 7 - x : x;
            int flipped_y = flip_y ? 7 - y : y;

            uint8_t current_pixel = tile[flipped_y * 8 + flipped_x];

            if((current_pixel == 0) && is_transparent) continue;

            uint8_t offset_x = start_x + x;
            uint8_t offset_y = start_y + y;
            if(offset_y < buffer_size_y && offset_x < buffer_size_x)
            {
                buffer[offset_y * buffer_size_x + offset_x] = current_pixel;
            }
        }
    }
}

void PPU::writeOAMToBuffer()
{
    uint16_t object_address = oam_start;
    for (int i = 0; i < objects.size(); i++)
    {
        // Sprite position.
        objects[i].y = memory.read(object_address, false);
        objects[i].x = memory.read(object_address + 1, false);
        // Tile index.
        objects[i].tile_index = memory.read(object_address + 2, false);
        // Attributes.
        objects[i].priority = memory.read(object_address + 3, false) & 0b10000000;
        objects[i].x_flip = memory.read(object_address + 3, false) & 0b00100000;
        objects[i].y_flip = memory.read(object_address + 3, false) & 0b01000000;
        objects[i].palette_number = memory.read(object_address + 3, false) & 0b00010000;

        object_address += 4;
    }
}
