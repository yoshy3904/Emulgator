#pragma once
#include "memory.hpp"

#include <stdint.h>
#include <vector>
#include <functional>
#include <string>

class Memory;

/*
Emulation of the SHARP_LR35902 CPU used in the Gameboy.
It is based on the Intel8080 and the Zilog80 CPUs.

Used info on the instruction set:
https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html 07/07/2021 22:42 (1)
http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf 07/07/2021 22:42 (2)
https://gbdev.io/pandocs/CPU_Instruction_Set.html 07/07/2021 22:42 (3)
https://gbdev.io/gb-opcodes/optables/ 23/07/2021 (4)

It seems like source 2 is wrong with the instructions RRCA RLCA RRA RLA. The zero flag is NOT set, but reset.
Some cycles in source 1 are wrong.

CYCLES
The SHARP LR35902 has a clock frequency of 4.194304 MHz. Altough one could argue that it only operates at a frequency of 1 MHz.
The reason for this is that the CPU can only ever run as fast as the RAM which runs at a frequency of 1MHz. So in general,
when talking about cycles or clock cycles of the GameBoy there are T-cycles and M-cycles. M-cylces are just T-cycles divided by 4.
That is possible because all timings of the CPU are divisble by 4. The namings of these cycles are not standardized and it seems
like every piece of documentation calls them differently.
But it is not that hard to keep them apart. Just remember that a NOP can take 4 cycles or 1 cycle.

DESIGN
I am using two big look-up tables. This is useful for a disassembler because it is easy to look up information about an opcode.
Some opcodes in these tables have the same function just with different operands involved. The CPU uses c++-functions to group those instructions 
and uses a switch() statement to run the appropriate code for each operand.

A common theme in the cpu's instructions set is this configuration/order of registers:    
opcode reg, B
opcode reg, C
opcode reg, D
opcode reg, E
opcode reg, H
opcode reg, L
opcode reg, (HL)
opcode reg, A

FLAGS

HALF CARRY
The half carry flag is confusing (but not that different to the carry flag). There are multiple cases, in each it operates a little different.

Normal addition:
Half carry checks if there is a carry from bit 3. So you add the lower 4 bits of both values together and check if they are bigger
than the maximum a 4 bit int can store, which is 0xf.
(x & 0x0f) + (y & 0x0f) > 0xf

Normal subtraction:
In this case half carry acts as borrow flag (carry too). In a subtraction there is always a borrow when you subtract a bigger number from a smaller number. 
The result will always be negativ. So you can just check if the result of the subtraction of the lower 4 bits of each number will be negativ.
(x & 0x0f) < (y & 0x0f)
OR 
(x & 0x0f) - (y & 0x0f) < 0

16-bit addition:
Same as normal addition but this time you check for a carry from bit 11.
(x & 0x0fff) + (y & 0x0fff) > 0x0fff
*/

class SHARP_LR35902
{
public:
    // MAIN REGISTERS

    uint8_t a; // 8-bit register called accumulator. Most operations are done with this register.
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp; // Stack pointer.
    uint16_t pc; // Program counter holds the address of the next instruction in memory.

    // STATUS REGISTER
    uint8_t flags;

    enum Flags
    {
        Carry           = 1 << 4, 
        HalfCarry       = 1 << 5, 
        Subtraction     = 1 << 6,
        Zero            = 1 << 7
    };
    
    // INTERRUPTS
    bool interrupt_master_enable;
    enum Interrupt 
    {
        VBLANK      = 1 << 0,
        LCD_STAT    = 1 << 1,
        TIMER       = 1 << 2,
        SERIAL      = 1 << 3,
        JOYPAD      = 1 << 4
    };

    // INSTRUCTIONS
    uint8_t opcode; // Store the fetched opcode temporarily.
    uint8_t operand1;
    uint8_t operand2;
    struct Instruction
    {
        std::string name;
        void(SHARP_LR35902::*func)();
        int cycles;
        int length;
    };
    std::vector<Instruction> instruction_table;
    std::vector<Instruction> prefix_instruction_table; // Instructions prefixed with '0xcb'.

    bool halted = false;
    bool stopped = false;
private:
    Memory& memory;
    uint8_t current_cycle_count;

public:
    SHARP_LR35902(Memory& p_memory);
    
    /*
    Fetch instruction bytes from RAM and execute it, advance program counter. Returns the amount of cycles the instruction needed to execute (NOP = 4).
    */
    uint8_t nextInstruction();

    void requestInterrupt(Interrupt p_interrupt);

    void reset();
private:
    bool getFlagBit(Flags p_mask);
    void setFlagBit(Flags p_mask, bool p_state);

    // General.
    void NOP();
    void STOP();
    void DAA();
    void CPL();
    void SCF();
    void CCF();
    void DI();
    void EI();
    void RST();
    void HALT();

    // Load.
    void LD();

    // Stack.
    void POP();
    void PUSH();

    // Logic-Gates.
    void AND();
    void OR();
    void XOR();

    // Bitwise-operations.
    void RLCA();
    void RRCA();
    void RLA();
    void RRA();
    void RLC();
    void RRC();
    void RL();
    void RR();
    void SLA();
    void SRA();
    void SRL();
    
    void SET();
    void RES();
    void BIT();
    void SWAP();

    // Arithmetic/Logic.
    void ADD();
    void ADC();
    void SUB();
    void SBC();
    void CP();
    void INC();
    void DEC();

    // Jump.
    void JR();
    void JP();
    void RET();
    void CALL();

    void XXX(); // Catching unimplemented opcodes (11 in total).
};