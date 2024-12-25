#include "memory.hpp"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <cmath>

MemoryBankController::MemoryBankController(Memory& p_memory)
    : cartridge_path(), memory(p_memory)
{

}

void MemoryBankController::loadFromFile(std::vector<uint8_t>& p_memory, uint16_t p_destination, uint16_t p_source, int p_length)
{
    std::ifstream stream(cartridge_path, std::ifstream::binary);
    if(stream.is_open())
    {
        int offset = 0;
        stream.seekg(p_source);
        while(stream.good() && (offset < p_length))
        {
            if(p_destination + offset >= p_memory.size()) 
            {
                break;
            }
            p_memory[p_destination + offset] = stream.get();
            offset++;
        }
    }
    else
    {
        std::cout << "Failed loading bank from file \"" << cartridge_path << "\"." << std::endl;
    }
}

NoMBC::NoMBC(Memory& p_memory)
    : MemoryBankController::MemoryBankController(p_memory)
{
    rom.resize(0x8000, 0x00);
}

void NoMBC::reset(const std::string& p_cartridge_path)
{
    std::cout << "Reset NoMBC \"" << p_cartridge_path << "\"" << std::endl;
    cartridge_path = p_cartridge_path;
    loadFromFile(rom, 0x0000, 0x0000, 0x8000);
}

uint8_t NoMBC::read(uint16_t p_address) const
{
    return rom.at(p_address);
}

void NoMBC::write(uint16_t p_address, uint8_t p_value)
{
    // Not allowed to write to rom. No further external ram available.
}

MBC1::MBC1(Memory& p_memory)
    : MemoryBankController::MemoryBankController(p_memory)
{
}

void MBC1::reset(const std::string& p_cartridge_path) 
{
    std::cout << "Reset MBC1 \"" << p_cartridge_path << "\"" << std::endl;
    cartridge_path = p_cartridge_path;
    
    ram_enable_register = false;
    bank1_register = 0x01;
    bank2_register = 0x00;
    mode_register = 0x00;  

    // Initialize rom to 32KB, 64KB, 128KB, 256KB, 512KB, 1MB or 2MB.
    rom_size = std::pow(2, memory.getCartridge().rom_size_code) * 0x8000;
    rom.resize(rom_size, 0x00);
    std::cout << "ROM Size: " << rom_size << std::endl;
    // Initialize external ram to 8KB or 32KB.
    external_ram_size = 0;
    if(memory.getCartridge().ram_size_code == 0x02)
    {
        external_ram_size = 0x2000;
    }
    else if(memory.getCartridge().ram_size_code == 0x03) 
    {
        external_ram_size = 0x8000;
    }
    external_ram.resize(external_ram_size, 0x00);
    std::cout << "External RAM Size: " << external_ram_size << std::endl;

    loadFromFile(rom, 0x0000, 0x0000, rom_size);
}

uint8_t MBC1::read(uint16_t address) const
{
    // Reading external ram.
    if((address >= 0xa000 && address <= 0xbfff) && ram_enable_register && external_ram_size > 0)
    {
        if(mode_register == 0)
        {
            // External ram is locked at bank 0.
            return external_ram.at(address & 0b0001111111111111);
        }
        else if(mode_register == 1)
        {
            if(external_ram_size == 0x8000)
            {
                // External ram is switched with the Bank 2 register.
                return external_ram.at((bank2_register << 13) + (address & 0b0001111111111111));
            }
            else if(external_ram_size == 0x2000)
            {
                // External ram is locked at bank 0.
                return external_ram.at(address & 0b0001111111111111);
            }
        }
    }

    // Reading from rom.
    if(address >= 0x0000 && address <= 0x3fff)
    {
        if(mode_register == 0)
        {
            return rom.at(address & 0b000000011111111111111);
        }
        else if(mode_register == 1)
        {
            if(rom_size >= 0x100000) // In >1MB cartridges bank 2 register is used to access banks 0x00/20/40/60 in frirst rom range.
            {
                return rom.at((bank2_register << 19) + (address & 0b000000011111111111111));
            }
            else
            {
                return rom.at(address & 0b000000011111111111111);
            }
        }
    }
    if(address >= 0x4000 && address <= 0x7fff)
    {
        int required_bits = memory.getCartridge().rom_size_code + 1 + 14; // Required bits for addressing 4000-7fff (14 bits from the GameBoy and rom_size_code + 1 bits).
        int mask = 0b111111111111111111111 >> (21 - required_bits); 
        int real_address = (bank2_register << 19) + (bank1_register << 14) + (address & 0b000000011111111111111);

        return rom.at(real_address & mask);
    }

    return 0xff;
}

void MBC1::write(uint16_t address, uint8_t value) 
{
    // External ram enable.
    if(address >= 0x0000 && address <= 0x1fff)
    {
        // Enable external ram if the lower 4 bits of the value written is 0x0a.
        ram_enable_register = ((value & 0x0f) == 0x0a);
    }

    // ROM Bank Number.
    if(address >= 0x2000 && address <= 0x3fff)
    {
        if((value & 0b00011111) == 0) 
        {
            bank1_register = 1;
        }
        else
        {
            bank1_register = value & 0b00011111;
        }
    }

    // RAM Bank Number OR Upper Bits of ROM Bank Number.
    if(address >= 0x4000 && address <= 0x5fff)
    {
        bank2_register = value & 0b11;
    }

    // Banking Mode Select.
    if(address >= 0x6000 && address <= 0x7fff)
    {
        mode_register = value & 1;
    }

    // External ram write.
    if((address >= 0xa000 && address <= 0xbfff) && ram_enable_register && external_ram_size > 0)
    {
        if(mode_register == 0)
        {
            // External ram is locked at bank 0.
            external_ram[address & 0b0001111111111111] = value;
        }
        else if(mode_register == 1)
        {
            if(external_ram_size == 0x8000)
            {
                // External ram is switched with the Bank 2 register.
                external_ram[(bank2_register << 13) + (address & 0b0001111111111111)] = value;
            }
            else if(external_ram_size == 0x2000)
            {
                // External ram is locked at bank 0.
                external_ram[address & 0b0001111111111111] = value;
            }
        }
    }
}

bool MBC1::getRamEnableRegister() const
{
    return ram_enable_register;
}

uint8_t MBC1::getBank1Register() const
{
    return bank1_register;
}

uint8_t MBC1::getBank2Register() const
{
    return bank2_register;
}

uint8_t MBC1::getModeRegister() const
{
    return mode_register;
}

const std::vector<uint8_t>& MBC1::getRom() const
{
    return rom;
}

const std::vector<uint8_t>& MBC1::getExternalRam() const
{
    return external_ram;
}

Memory::Memory(PPU& p_ppu, APU& p_apu, Timer& p_timer) 
    : internal_memory(64 * 1024), 
    ppu(p_ppu), 
    apu(p_apu), 
    timer(p_timer), 
    no_mbc(*this), 
    mbc1(*this), 
    cartridge({"", 0x00, 0x00, 0x00, "", "", "", no_mbc, mbc1 })
{
    cartridge_type_lookup = 
    { 
        "ROM ONLY", "MBC1", "MBC1 + RAM", "MBC1 + RAM + BATTERY", "UNUSED CARTRIDGE CODE", "MBC2", "MBC2 + BATTERY", "UNUSED CARTRIDGE CODE", "ROM + RAM",
        "ROM + RAM + BATTERY", "UNUSED CARTRIDGE CODE", "MMM01", "MMM01 + RAM", "MMM01 + RAM + BATTERY", "UNUSED CARTRIDGE CODE", "MBC3 + TIMER + BATTERY",
        "MBC3 + TIMER + RAM + BATTERY", "MBC3", "MBC3 + RAM", "MBC3 + RAM + BATTERY", "UNUSED CARTRIDGE CODE", "UNUSED CARTRIDGE CODE", "UNUSED CARTRIDGE CODE", 
        "UNUSED CARTRIDGE CODE", "UNUSED CARTRIDGE CODE", "MBC5", "MBC5 + RAM", "MBC5 + RAM + BATTERY", "MBC5 + RUMBLE", "MBC5 + RUMBLE + RAM", "MBC5 + RUMBLE + RAM + BATTERY",
        "UNUSED CARTRIDGE CODE", "MBC6", "UNUSED CARTRIDGE CODE", "MBC7 + SENSOR + RUMBLE + RAM + BATTERY"
    };
    rom_lookup = 
    { 
        "32 KB", "64 KB", "128 KB", "256 KB", "512 KB", "1024 KB", "2048 KB", "4096 KB", "8186 KB" 
    };
    ram_lookup = 
    { 
        "No RAM", "Invalid RAM size", "8 KB", "32 KB", "128 KB", "64 KB"
    };
}

uint8_t Memory::read(uint16_t p_address, bool restricted) const
{
    if(restricted) 
    {
        if(internal_memory[0xff40] & 0b10000000) // Check if LCD is on.
        {
            // Make VRAM and OAM RAM inaccessible during certain PPU modes.
            uint8_t ppu_mode = internal_memory[0xff41] & 0b00000011;
            if((p_address >= 0x8000) && (p_address <= 0x9fff)) // Address is in VRAM area.
            {
                if(ppu_mode == 3)
                {
                    return 0xff;
                }
            }
            if((p_address >= 0xfe00) && (p_address <= 0xfe9f)) // Address is in OAM area.
            {
                if(ppu_mode == 2 || ppu_mode == 3)
                {
                    return 0xff;
                }
            }
        }
    }

    // Read using memory bank controllers if they exist.
    if(cartridge.type_code == 0)
    {
        if(isRom(p_address))
        {
            return no_mbc.read(p_address);
        }
        else
        {
            return internal_memory[p_address];
        }
    }
    else if (cartridge.type_code == 1 || cartridge.type_code == 2 || cartridge.type_code == 3)
    {
        if(isRom(p_address) || isExternalRam(p_address))
        {
            return mbc1.read(p_address);
        }
        else
        {
            return internal_memory[p_address];
        }
    }

    return 0xff;
}

void Memory::write(uint16_t p_address, uint8_t p_value, bool restricted) 
{
    if(restricted) 
    {
        if(internal_memory[0xff40] & 0b10000000) // Check if LCD is on.
        {
            // Make VRAM and OAM RAM read-only during certain PPU modes.
            uint8_t ppu_mode = internal_memory[0xff41] & 0b00000011;
            if((p_address >= 0x8000) && (p_address <= 0x9fff)) // Address is in VRAM area.
            {
                if(ppu_mode == 3)
                {
                    return;
                }
            }
            if((p_address >= 0xfe00) && (p_address <= 0xfe9f)) // Address is in OAM area.
            {
                if(ppu_mode == 2 || ppu_mode == 3)
                {
                    return;
                }
            }
        }

        // Make lower 3 bits of STAT register read-only.
        if(p_address == 0xff41)
        {
            internal_memory[0xff41] = (p_value & 0b11111000) | (internal_memory[p_address] & 0b00000111);
            return;
        }

        // Make LY read-only.
        if(p_address == 0xff44)
        {
            return;
        }

        // JOYPAD. Make the lower 4 bits of the JOYPAD register non-writable.
        if(p_address == 0xff00)
        {
            internal_memory[0xff00] = ((p_value & 0xf0) | (internal_memory[p_address] & 0x0f));
            internal_memory[0xff00] = internal_memory[0xff00] | 0b11000000; 
            return;
        }

        // TAC WRITE.
        if(p_address == 0xff07)
        {
            uint16_t clock_values[4] = { 1024, 16, 64, 256 };
            bool old_enable = internal_memory[0xff07] & 0b00000100;
            bool new_enable = p_value & 0b00000100;
            uint8_t old_clock = internal_memory[0xff07] & 0b00000011;
            uint8_t new_clock = p_value & 0b00000011;

            if((new_enable == 0) && (old_enable == 1))
            {
                internal_memory[0xff05] += ((timer.internal_counter & timer.getFrequencyBit()) != 0);
            }
            if((new_enable == 1) && (old_enable == 1))
            {
                internal_memory[0xff05] += (((timer.internal_counter & clock_values[old_clock]) != 0) && ((timer.internal_counter & clock_values[new_clock]) == 0));
            }

            internal_memory[p_address] = p_value;
            return;
        }

        // DIV RESET. Reset the internal counter. This resets DIV and also affects TIMA. If the internal counter gets repeatedly set to 0, TIMA will not increase.
        if(p_address == 0xff04)
        {
            //If the timer is enabled (TAC enabled) and the multiplexer output was 1, TIMA is increased, because after DIV reset the mutliplexer output is 0 (and that is a falling edge).
            if((read(0xff07) & 0b00000100) && (timer.internal_counter & timer.getFrequencyBit()))
            {
                internal_memory[0xff05] += 1;
            }
            timer.internal_counter = 0;
            return;
        }
    }

    // Write using memory bank controllers if they exist.
    if(cartridge.type_code == 0)
    {
        if(isRom(p_address))
        {
            no_mbc.write(p_address, p_value);
        }
        else
        {
            internal_memory[p_address] = p_value;
        }
    }
    else if (cartridge.type_code == 1 || cartridge.type_code == 2 || cartridge.type_code == 3)
    {
        if(isRom(p_address) || isExternalRam(p_address))
        {
            mbc1.write(p_address, p_value);
        }
        else
        {
            internal_memory[p_address] = p_value;
        }
    }
    
    // Sound channel triggering.
    if(p_address == 0xff14 && (p_value >> 7) == 1)
    {
        apu.trigger(1);
    }
    if(p_address == 0xff19 && (p_value >> 7) == 1)
    {
        apu.trigger(2);
    }
    if(p_address == 0xff1e && (p_value >> 7) == 1)
    {
        apu.trigger(3);
    }
    if(p_address == 0xff23 && (p_value >> 7) == 1)
    {
        apu.trigger(4);
    }

    // OAM DMA Transfer.
    if(p_address == 0xff46)
    {
        uint16_t start_address = p_value << 8;
        for (uint16_t i = 0; i <= 0x009f; i++)
        {
            internal_memory[0xfe00 + i] = read(start_address + i);
        }
    }
}

bool Memory::isRom(uint16_t p_address) const
{
    return p_address <= 0x7fff;
}

bool Memory::isExternalRam(uint16_t p_address) const
{
    return p_address >= 0xa000 && p_address <= 0xbfff;
}

void Memory::loadCartridge(const std::string& p_file_path) 
{
    if(!std::filesystem::exists(p_file_path))
    {
        std::cout << "Invalid cartridge path \"" << p_file_path << "\"." << std::endl;
        return;
    }

    loadHeader(p_file_path);

    // Initialize memory bank controller present in the cartridge.
    if(cartridge.type_code == 0)
    {
        no_mbc.reset(p_file_path);
    }
    else if (cartridge.type_code == 1 || cartridge.type_code == 2 || cartridge.type_code == 3)
    {
        mbc1.reset(p_file_path);
    }
}

const Memory::Cartridge& Memory::getCartridge() const
{
    return cartridge;
}

void Memory::loadHeader(const std::string& p_file_path)
{
    std::ifstream stream(p_file_path, std::ios::in | std::ios::binary | std::ios::ate);
    if(stream.is_open())
    {
        // Read bytes from file.
        std::streampos size = stream.tellg();
        if(size < 0x014f)
        {
            std::cout << "Tried loading header from file that was too small (\"" << p_file_path << "\" with " << size << " bytes)." << std::endl;
            return;
        }
        char* bytes = new char[0x0150];
        stream.seekg(0, std::ios::beg);
        stream.read(bytes, 0x0150);
        stream.close();
        
        // Read info bytes.
        cartridge.type_code = bytes[0x0147];
        cartridge.rom_size_code = bytes[0x0148];
        cartridge.ram_size_code = bytes[0x0149];
        
        // Set string corresponding to bytes.
        cartridge.type_string = cartridge.type_code >= cartridge_type_lookup.size() ? "CARTRIDGE TYPE NOT SUPPORTED" : cartridge_type_lookup[cartridge.type_code];
        cartridge.rom_size_string = cartridge.rom_size_code >= rom_lookup.size() ? "ROM SIZE NOT SUPPORTED" : rom_lookup[cartridge.rom_size_code];
        cartridge.ram_size_string = cartridge.ram_size_code >= ram_lookup.size() ? "RAM SIZE NOT SUPPORTED" : ram_lookup[cartridge.ram_size_code];

        // Read title.
        cartridge.title = "";
        uint8_t currentByte = 0xff;
        uint16_t titleAddress = 0x0134;
        while(true)
        {
            currentByte = bytes[titleAddress];
            titleAddress++;
            if(currentByte == 0x00) break;
            cartridge.title += char(currentByte);
            if(titleAddress > 0x0143) break;
        }

        // Free data.
        delete[] bytes;
    }    
}