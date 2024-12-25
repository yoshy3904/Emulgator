#pragma once
#include "timing.hpp"
#include "ppu.hpp"
#include "apu.hpp"

#include <stdint.h>
#include <vector>
#include <string>

class PPU;
class APU;
class Timer;

/*
MEMORY MAP

0000	3FFF	16 KiB ROM bank 00	From cartridge, usually a fixed bank
4000	7FFF	16 KiB ROM Bank 01~NN	From cartridge, switchable bank via mapper (if any)
8000	9FFF	8 KiB Video RAM (VRAM)	In CGB mode, switchable bank 0/1
A000	BFFF	8 KiB External RAM	From cartridge, switchable bank if any
C000	CFFF	4 KiB Work RAM (WRAM)	
D000	DFFF	4 KiB Work RAM (WRAM)	In CGB mode, switchable bank 1~7
E000	FDFF	Mirror of C000~DDFF (ECHO RAM)	Nintendo says use of this area is prohibited.
FE00	FE9F	Sprite attribute table (OAM)	
FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited
FF00	FF7F	I/O Registers	
FF80	FFFE	High RAM (HRAM)	
FFFF	FFFF	Interrupts Enable Register (IE)

I/O REGISTERS

FF00 - Joypad information register.

FF01 - SB (Serial transfer data)
FF02 - SC (Serial Transfer Control)

FF04 - DIV (Divider Register): Increments at a rate of 16384hz.
FF05 - TIMA (Timer counter): Incremented at frequency specified in TAC. Throws TIMER Interrupt on overflow.
FF06 - TMA (Timer Modulo): Holds the vlaue to which TIMA is set after an overflow.
FF07 - TAC (Timer Control): Specifices the frequency at which TIMA increments.

FF0F - Interrupt Request.

FF10 - Sound
FF11 - Sound
FF12 - Sound
FF13 - Sound
FF14 - Sound
FF16 - Sound
FF17 - Sound
FF18 - Sound
FF19 - Sound
FF1a - Sound
FF1b - Sound
FF1c - Sound
FF1d - Sound
FF1e - Sound
FF30-FF3F - Sound
FF20 - Sound
FF21 - Sound
FF22 - Sound
FF23 - Sound
FF24 - Sound
FF25 - Sound
FF26 - Sound

FF40 - LCDC (LCD Control)
FF41 - STAT (LCD Status)
FF42 - SCY
FF43 - SCX
FF44 - LY
FF45 - LYC
FF47 - BGP (BG Palette Data)
FF48 - OBP0
FF49 - OBP1
FF4A - WY
FF4B - WX

FFFF - Interrupt Enable.


ROM Sizes: 
32 kib  (0x00) 2 banks 1 bit
64 kib  (0x01) 4 banks 2 bits
128 kib (0x02) 8 banks 3 bits
256 kib (0x03) 16 banks 4 bits
512 kib (0x04) 32 banks 5 bits
1 Mib   (0x05) 64 banks 6 bits
2 Mib   (0x06) 128 banks 7 bits
4 Mib   (0x07) 256 banks
8 Mib   (0x08) 512 banks

RAM Sizes:
No RAM (0x00)
Unused Code (0x01)
8 kib (0x02)
32 kib (0x03)
128 kib (0x04)
64 kib (0x05)
*/

class MemoryBankController
{
protected:
    Memory& memory;
    std::string cartridge_path;
public:
    MemoryBankController(Memory& p_memory);

    // Called when a new cartridge is inserted. Resets the cartridge_path, registers and initially loads memory.
    virtual void reset(const std::string& p_cartridge_path) = 0;
    // Read from rom or external ram defined by the mbc.
    virtual uint8_t read(uint16_t p_address) const = 0;
    // Write to rom (for changing registers) or external ram defined by the mbc.
    virtual void write(uint16_t p_address, uint8_t p_value) = 0;

    // Loads bytes from a cartridge file into the p_memory vector.
    void loadFromFile(std::vector<uint8_t>& p_memory, uint16_t p_destination, uint16_t p_source, int p_length);
};

class NoMBC : public MemoryBankController
{
private:
    std::vector<uint8_t> rom;
public:
    NoMBC(Memory& p_memory);

    void reset(const std::string& p_cartridge_path) override;
    uint8_t read(uint16_t p_address) const override;
    void write(uint16_t p_address, uint8_t p_value) override;
};

class MBC1 : public MemoryBankController
{
/*
MBC1 (Memory Banking Controller 1)

2 MiB of ROM - 128 ROM Banks
32 KiB of RAM - 4 RAM Banks

RAMG - Addressed by writing to 0x0000-0x1fff
Enables and disables RAM at 0xa000-0xbfff (disabled by default).
 - Writing 0x0a means enable.
 - Every other value means disable (writes ignored, reads undefined).

BANK1 (5 bit register) - Addressed by writing to 0x2000-0x3fff
Lower 5 bits of the ROM Bank number for ROM at 0x4000-0x7fff.
Writing 0x00000 is not allowed, 0x00001 will be written instead.
If the register uses less than 5 bits (<512KB), all 5 bits will still be checked for 0->1 translation (making it possible to load bank 0). 

BANK2 (2 bit register) - Addressed by writing to 0x4000-0x5fff
1. Upper 2 bits of the ROM Bank number for ROM at 0x4000-0x7fff.
2. 2 bits that select RAM Bank for RAM at 0xa000-0xbfff.
3. 2 bits shifted by 5 indicate the ROM Bank for 0x0000-0x3fff

MODE (1 bit register) - Addressed by writing to 0x6000-0x7fff
0b0 - BANK2 first case (>= 1MB)
0b1 - Large RAM (>8KB): BANK 2 second case
      Large ROM (>=1MB): BANK 2 first and third case
*/
private:
    std::vector<uint8_t> rom;
    std::vector<uint8_t> external_ram;

    int rom_size = 0;
    int external_ram_size = 0;

    bool ram_enable_register = false;
    uint8_t bank1_register = 0x00;
    uint8_t bank2_register = 0x00;
    uint8_t mode_register = 0x00;
public:
    MBC1(Memory& p_memory);

    void reset(const std::string& p_cartridge_path) override;
    uint8_t read(uint16_t p_address) const override;
    void write(uint16_t p_address, uint8_t p_value) override;

    bool getRamEnableRegister() const;
    uint8_t getBank1Register() const;
    uint8_t getBank2Register() const;
    uint8_t getModeRegister() const;
    const std::vector<uint8_t>& getRom() const;
    const std::vector<uint8_t>& getExternalRam() const;
};


class Memory
{
public:
    struct Cartridge
    {
        std::string title;
        uint8_t type_code;
        uint8_t rom_size_code;
        uint8_t ram_size_code;
        std::string type_string;
        std::string rom_size_string;
        std::string ram_size_string;

        const NoMBC& no_mbc;
        const MBC1& mbc1;
    };
private:
    std::vector<uint8_t> internal_memory;
    Cartridge cartridge;

    NoMBC no_mbc;
    MBC1 mbc1;

    PPU& ppu;
    APU& apu;
    Timer& timer;

    std::vector<std::string> cartridge_type_lookup;
    std::vector<std::string> rom_lookup;
    std::vector<std::string> ram_lookup;
public:
    Memory(PPU& p_ppu, APU& p_apu, Timer& p_timer);

    // Read from the 64 kilobyte internal memory. Software should only call this function with restrictions enabled.
    uint8_t read(uint16_t p_address, bool restricted = true) const;
    // Write to the 64 kilobyte internal memory. Software should only call this function with restrictions enabled.
    void write(uint16_t p_address, uint8_t p_value, bool restricted = true);

    void loadCartridge(const std::string& p_file_path);
    const Cartridge& getCartridge() const;
private:
    void loadHeader(const std::string& p_file_path);

    bool isRom(uint16_t p_address) const;
    bool isExternalRam(uint16_t p_address) const;
};