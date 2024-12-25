#include "SHARP_LR35902.hpp"
#include <iostream> // Debugging.

SHARP_LR35902::SHARP_LR35902(Memory& p_memory) : memory(p_memory)
{
    reset();

    using S = SHARP_LR35902; // Less to type.
    instruction_table = 
    {
        { "NOP", &S::NOP, 4, 1 }, { "LD BC,d16", &S::LD, 12, 3 }, { "LD (BC),A", &S::LD, 8, 1 }, { "INC BC", &S::INC, 8, 1 }, { "INC B", &S::INC, 4, 1 }, { "DEC B", &S::DEC, 4, 1 }, { "LD B,d8", &S::LD, 8, 2 }, { "RLCA", &S::RLCA, 4, 1 }, { "LD (a16),SP", &S::LD, 20, 2 }, { "ADD HL,BC", &S::ADD, 8, 1 }, { "LD A,(BC)", &S::LD, 8, 1 }, { "DEC BC", &S::DEC, 8, 1 }, { "INC C", &S::INC, 4, 1 }, { "DEC C", &S::DEC, 4, 1 }, { "LD C,d8", &S::LD, 8, 1 }, { "RRCA", &S::RRCA, 4, 1 },
        { "STOP", &S::STOP, 4, 2 }, { "LD DE,d16", &S::LD, 12, 3 }, { "LD (DE),A", &S::LD, 8, 1 }, { "INC DE", &S::INC, 8, 1 }, { "INC D", &S::INC, 4, 1 }, { "DEC D", &S::DEC, 4, 1 }, { "LD D,d8", &S::LD, 8, 2 }, { "RLA", &S::RLA, 4, 1 }, { "JR r8", &S::JR, 12, 2 }, { "ADD HL,DE", &S::ADD, 8, 1 }, { "LD A,(DE)", &S::LD, 8, 1 }, { "DEC DE", &S::DEC, 8, 1 }, { "INC E", &S::INC, 4, 1 }, { "DEC E", &S::DEC, 4, 1 }, { "LD E,d8", &S::LD, 8, 2 }, { "RRA", &S::RRA, 4, 1 },
        { "JR NZ,r8", &S::JR, 12 << 16 | 8, 2 }, { "LD HL,d16", &S::LD, 12, 3 }, { "LD (HL+),A", &S::LD, 8, 1 }, { "INC HL", &S::INC, 8, 1 }, { "INC H", &S::INC, 4, 1 }, { "DEC H", &S::DEC, 4, 1 }, { "LD H,d8", &S::LD, 8, 2 }, { "DAA", &S::DAA, 4, 1 }, { "JR Z,r8", &S::JR, 12 << 16 | 8, 2 }, { "ADD HL,HL", &S::ADD, 8, 1 }, { "LD A,(HL+)", &S::LD, 8, 1 }, { "DEC HL", &S::DEC, 8, 1 }, { "INC L", &S::INC, 4, 1 }, { "DEC L", &S::DEC, 4, 1 }, { "LD L,d8", &S::LD, 8, 2 }, { "CPL", &S::CPL, 4, 1 },
        { "JR NC,r8", &S::JR, 12 << 16 | 8, 2 }, { "LD SP,d16", &S::LD, 12, 3 }, { "LD (HL-),A", &S::LD, 8, 1 }, { "INC SP", &S::INC, 8, 1 }, { "INC (HL)", &S::INC, 12, 1 }, { "DEC (HL)", &S::DEC, 12, 1 }, { "LD (HL),d8", &S::LD, 12, 2 }, { "SCF", &S::SCF, 4, 1 }, { "JR C,r8", &S::JR, 12 << 16 | 8, 2 }, { "ADD HL,SP", &S::ADD, 8, 1 }, { "LD A,(HL-)", &S::LD, 8, 1 }, { "DEC SP", &S::DEC, 8, 1 }, { "INC A", &S::INC, 4, 1 }, { "DEC A", &S::DEC, 4, 1 }, { "LD A,d8", &S::LD, 8, 2 }, { "CCF", &S::CCF, 4, 1 },
        { "LD B,B", &S::LD, 4, 1 }, { "LD B,C", &S::LD, 4, 1 }, { "LD B,D", &S::LD, 4, 1 }, { "LD B,E", &S::LD, 4, 1 }, { "LD B,H", &S::LD, 4, 1 }, { "LD B,L", &S::LD, 4, 1 }, { "LD B,(HL)", &S::LD, 8, 1 }, { "LD B,A", &S::LD, 4, 1 }, { "LD C,B", &S::LD, 4, 1 }, { "LD C,C", &S::LD, 4, 1 }, { "LD C,D", &S::LD, 4, 1 }, { "LD C,E", &S::LD, 4, 1 }, { "LD C,H", &S::LD, 4, 1 }, { "LD C,L", &S::LD, 4, 1 }, { "LD C,(HL)", &S::LD, 8, 1 }, { "LD C,A", &S::LD, 4, 1 },
        { "LD D,B", &S::LD, 4, 1 }, { "LD D,C", &S::LD, 4, 1 }, { "LD D,D", &S::LD, 4, 1 }, { "LD D,E", &S::LD, 4, 1 }, { "LD D,H", &S::LD, 4, 1 }, { "LD D,L", &S::LD, 4, 1 }, { "LD D,(HL)", &S::LD, 8, 1 }, { "LD D,A", &S::LD, 4, 1 }, { "LD E,B", &S::LD, 4, 1 }, { "LD E,C", &S::LD, 4, 1 }, { "LD E,D", &S::LD, 4, 1 }, { "LD E,E", &S::LD, 4, 1 }, { "LD E,H", &S::LD, 4, 1 }, { "LD E,L", &S::LD, 4, 1 }, { "LD E,(HL)", &S::LD, 8, 1 }, { "LD E,A", &S::LD, 4, 1 },
        { "LD H,B", &S::LD, 4, 1 }, { "LD H,C", &S::LD, 4, 1 }, { "LD H,D", &S::LD, 4, 1 }, { "LD H,E", &S::LD, 4, 1 }, { "LD H,H", &S::LD, 4, 1 }, { "LD H,L", &S::LD, 4, 1 }, { "LD H,(HL)", &S::LD, 8, 1 }, { "LD H,A", &S::LD, 4, 1 }, { "LD L,B", &S::LD, 4, 1 }, { "LD L,C", &S::LD, 4, 1 }, { "LD L,D", &S::LD, 4, 1 }, { "LD L,E", &S::LD, 4, 1 }, { "LD L,H", &S::LD, 4, 1 }, { "LD L,L", &S::LD, 4, 1 }, { "LD L,(HL)", &S::LD, 8, 1 }, { "LD L,A", &S::LD, 4, 1 },
        { "LD (HL),B", &S::LD, 8, 1 }, { "LD (HL),C", &S::LD, 8, 1 }, { "LD (HL),D", &S::LD, 8, 1 }, { "LD (HL),E", &S::LD, 8, 1 }, { "LD (HL),H", &S::LD, 8, 1 }, { "LD (HL),L", &S::LD, 8, 1 }, { "HALT", &S::HALT, 4, 1 }, { "LD (HL),A", &S::LD, 8, 1 }, { "LD A,B", &S::LD, 4, 1 }, { "LD A,C", &S::LD, 4, 1 }, { "LD A,D", &S::LD, 4, 1 }, { "LD A,E", &S::LD, 4, 1 }, { "LD A,H", &S::LD, 4, 1 }, { "LD A,L", &S::LD, 4, 1 }, { "LD A,(HL)", &S::LD, 8, 1 }, { "LD A,A", &S::LD, 4, 1 },
        { "ADD A,B", &S::ADD, 4, 1 }, { "ADD A,C", &S::ADD, 4, 1 }, { "ADD A,D", &S::ADD, 4, 1 }, { "ADD A,E", &S::ADD, 4, 1 }, { "ADD A,H", &S::ADD, 4, 1 }, { "ADD A,L", &S::ADD, 4, 1 }, { "ADD A,(HL)", &S::ADD, 8, 1 }, { "ADD A,A", &S::ADD, 4, 1 }, { "ADC A,B", &S::ADC, 4, 1 }, { "ADC A,C", &S::ADC, 4, 1 }, { "ADC A,D", &S::ADC, 4, 1 }, { "ADC A,E", &S::ADC, 4, 1 }, { "ADC A,H", &S::ADC, 4, 1 }, { "ADC A,L", &S::ADC, 4, 1 }, { "ADC A,(HL)", &S::ADC, 8, 1 }, { "ADC A,A", &S::ADC, 4, 1 },
        { "SUB B", &S::SUB, 4, 1 }, { "SUB C", &S::SUB, 4, 1 }, { "SUB D", &S::SUB, 4, 1 }, { "SUB E", &S::SUB, 4, 1 }, { "SUB H", &S::SUB, 4, 1 }, { "SUB L", &S::SUB, 4, 1 }, { "SUB (HL)", &S::SUB, 8, 1 }, { "SUB A", &S::SUB, 4, 1 }, { "SBC A,B", &S::SBC, 4, 1 }, { "SBC A,C", &S::SBC, 4, 1 }, { "SBC A,D", &S::SBC, 4, 1 }, { "SBC A,E", &S::SBC, 4, 1 }, { "SBC A,H", &S::SBC, 4, 1 }, { "SBC A,L", &S::SBC, 4, 1 }, { "SBC A,(HL)", &S::SBC, 8, 1 }, { "SBC A,A", &S::SBC, 4, 1 }, 
        { "AND B", &S::AND, 4, 1 }, { "AND C", &S::AND, 4, 1 }, { "AND D", &S::AND, 4, 1 }, { "AND E", &S::AND, 4, 1 }, { "AND H", &S::AND, 4, 1 }, { "AND L", &S::AND, 4, 1 }, { "AND (HL)", &S::AND, 8, 1 }, { "AND A", &S::AND, 4, 1 }, { "XOR B", &S::XOR, 4, 1 }, { "XOR C", &S::XOR, 4, 1 }, { "XOR D", &S::XOR, 4, 1 }, { "XOR E", &S::XOR, 4, 1 }, { "XOR H", &S::XOR, 4, 1 }, { "XOR L", &S::XOR, 4, 1 }, { "XOR (HL)", &S::XOR, 8, 1 }, { "XOR A", &S::XOR, 4, 1 }, 
        { "OR B", &S::OR, 4, 1 }, { "OR C", &S::OR, 4, 1 }, { "OR D", &S::OR, 4, 1 }, { "OR E", &S::OR, 4, 1 }, { "OR H", &S::OR, 4, 1 }, { "OR L", &S::OR, 4, 1 }, { "OR (HL)", &S::OR, 8, 1 }, { "OR A", &S::OR, 4, 1 }, { "CP B", &S::CP, 4, 1 }, { "CP C", &S::CP, 4, 1 }, { "CP D", &S::CP, 4, 1 }, { "CP E", &S::CP, 4, 1 }, { "CP H", &S::CP, 4, 1 }, { "CP L", &S::CP, 4, 1 }, { "CP (HL)", &S::CP, 8, 1 }, { "CP A", &S::CP, 4, 1 }, 
        { "RET NZ", &S::RET, 20 << 16 | 8, 1 }, { "POP BC", &S::POP, 12, 1 }, { "JP NZ,a16", &S::JP, 16 << 16 | 12, 3 }, { "JP a16", &S::JP, 16, 3 },  { "CALL NZ,a16", &S::CALL, 24 << 16 | 12, 3 }, { "PUSH BC", &S::PUSH, 16, 1 }, { "ADD A,d8", &S::ADD, 8, 2 }, { "RST 00H", &S::RST, 16, 1 },  { "RET Z", &S::RET, 20 << 16 | 8, 1 }, { "RET", &S::RET, 16, 1 }, { "JP Z,a16", &S::JP, 16 << 16 | 12, 3 },  { "PREFIX CB", &S::XXX, 4, 1 }, { "CALL Z,a16", &S::CALL, 24 << 16 | 12, 3 }, { "CALL a16", &S::CALL, 24, 3 }, { "ADC A,d8", &S::ADC, 8, 2 }, { "RST 08H", &S::RST, 16, 1 },
        { "RET NC", &S::RET, 20 << 16 | 8, 1 }, { "POP DE", &S::POP, 12, 1 }, { "JP NC,a16", &S::JP, 16 << 16 | 12, 3 }, { "XXX", &S::XXX, 0, 1 },  { "CALL NC,a16", &S::CALL, 24 << 16 | 12, 3 }, { "PUSH DE", &S::PUSH, 16, 1 }, { "SUB d8", &S::SUB, 8, 2 }, { "RST 10H", &S::RST, 16, 1 },  { "RET C", &S::RET, 20 << 16 | 8, 1 }, { "RETI", &S::RET, 16, 1 }, { "JP C,a16", &S::JP, 16 << 16 | 12, 3 },  { "XXX", &S::XXX, 0, 1 }, { "CALL C,a16", &S::CALL, 24 << 16 | 12, 3 }, { "XXX", &S::XXX, 0, 1 }, { "SBC A,d8", &S::SBC, 8, 2 }, { "RST 18H", &S::RST, 16, 1 }, 
        { "LDH (a8),A", &S::LD, 12, 2 }, { "POP HL", &S::POP, 12, 1 }, { "LD (C),A", &S::LD, 8, 1 }, { "XXX", &S::XXX, 0, 1 }, { "XXX", &S::XXX, 0, 1 }, { "PUSH HL", &S::PUSH, 16, 1 }, { "AND d8", &S::AND, 8, 2 }, { "RST 20H", &S::RST, 16, 1 }, { "ADD SP,r8", &S::ADD, 16, 2 }, { "JP (HL)", &S::JP, 4, 1 }, { "LD (a16),A", &S::LD, 16, 3 }, { "XXX", &S::XXX, 0, 1 }, { "XXX", &S::XXX, 0, 1 }, { "XXX", &S::XXX, 0, 1 }, { "XOR d8", &S::XOR, 8, 2 }, { "RST 28H", &S::RST, 16, 1 }, 
        { "LDH A,(a8)", &S::LD, 12, 2 }, { "POP AF", &S::POP, 12, 1 }, { "LD A,(C)", &S::LD, 8, 1 }, { "DI", &S::DI, 4, 1 }, { "XXX", &S::XXX, 0, 1 }, { "PUSH AF", &S::PUSH, 16, 1 }, { "OR d8", &S::OR, 8, 2 }, { "RST 30H", &S::RST, 16, 1  }, { "LD HL,SP+r8", &S::LD, 12, 2 }, { "LD SP,HL", &S::LD, 8, 1 }, { "LD A,(a16)", &S::LD, 16, 3 }, { "EI", &S::EI, 4, 1 }, { "XXX", &S::XXX, 0, 1 }, { "XXX", &S::XXX, 0, 1 }, { "CP d8", &S::CP, 8, 2 }, { "RST 38H", &S::RST, 16, 1 }
    };

    prefix_instruction_table = 
    {
        { "RLC B", &S::RLC, 8, 1 }, { "RLC C", &S::RLC, 8, 1 }, { "RLC D", &S::RLC, 8, 1 }, { "RLC E", &S::RLC, 8, 1 }, { "RLC H", &S::RLC, 8, 1 }, { "RLC L", &S::RLC, 8, 1 }, { "RLC (HL)", &S::RLC, 16, 1 }, { "RLC A", &S::RLC, 8, 1 }, { "RRC B", &S::RRC, 8, 1 }, { "RRC C", &S::RRC, 8, 1 }, { "RRC D", &S::RRC, 8, 1 }, { "RRC E", &S::RRC, 8, 1 }, { "RRC H", &S::RRC, 8, 1 }, { "RRC L", &S::RRC, 8, 1 }, { "RRC (HL)", &S::RRC, 16, 1 }, { "RRC A", &S::RRC, 8, 1 },
        { "RL B", &S::RL, 8, 1 }, { "RL C", &S::RL, 8, 1 }, { "RL D", &S::RL, 8, 1 }, { "RL E", &S::RL, 8, 1 }, { "RL H", &S::RL, 8, 1 }, { "RL L", &S::RL, 8, 1 }, { "RL (HL)", &S::RL, 16, 1 }, { "RL A", &S::RL, 8, 1 }, { "RR B", &S::RR, 8, 1 }, { "RR C", &S::RR, 8, 1 }, { "RR D", &S::RR, 8, 1 }, { "RR E", &S::RR, 8, 1 }, { "RR H", &S::RR, 8, 1 }, { "RR L", &S::RR, 8, 1 }, { "RR (HL)", &S::RR, 16, 1 }, { "RR A", &S::RR, 8, 1 },
        { "SLA B", &S::SLA, 8, 1 }, { "SLA C", &S::SLA, 8, 1 }, { "SLA D", &S::SLA, 8, 1 }, { "SLA E", &S::SLA, 8, 1 }, { "SLA H", &S::SLA, 8, 1 }, { "SLA L", &S::SLA, 8, 1 }, { "SLA (HL)", &S::SLA, 16, 1 }, { "SLA A", &S::SLA, 8, 1 }, { "SRA B", &S::SRA, 8, 1 }, { "SRA C", &S::SRA, 8, 1 }, { "SRA D", &S::SRA, 8, 1 }, { "SRA E", &S::SRA, 8, 1 }, { "SRA H", &S::SRA, 8, 1 }, { "SRA L", &S::SRA, 8, 1 }, { "SRA (HL)", &S::SRA, 16, 1 }, { "SRA A", &S::SRA, 8, 1 },
        { "SWAP B", &S::SWAP, 8, 1 }, { "SWAP C", &S::SWAP, 8, 1 }, { "SWAP D", &S::SWAP, 8, 1 }, { "SWAP E", &S::SWAP, 8, 1 }, { "SWAP H", &S::SWAP, 8, 1 }, { "SWAP L", &S::SWAP, 8, 1 }, { "SWAP (HL)", &S::SWAP, 16, 1 }, { "SWAP A", &S::SWAP, 8, 1 }, { "SRL B", &S::SRL, 8, 1 }, { "SRL C", &S::SRL, 8, 1 }, { "SRL D", &S::SRL, 8, 1 }, { "SRL E", &S::SRL, 8, 1 }, { "SRL H", &S::SRL, 8, 1 }, { "SRL L", &S::SRL, 8, 1 }, { "SRL (HL)", &S::SRL, 16, 1 }, { "SRL A", &S::SRL, 8, 1 },
        { "BIT 0,B", &S::BIT, 8, 1 }, { "BIT 0,C", &S::BIT, 8, 1 }, { "BIT 0,D", &S::BIT, 8, 1 }, { "BIT 0,E", &S::BIT, 8, 1 }, { "BIT 0,H", &S::BIT, 8, 1 }, { "BIT 0,L", &S::BIT, 8, 1 }, { "BIT 0,(HL)", &S::BIT, 12, 1 }, { "BIT 0,A", &S::BIT, 8, 1 }, { "BIT 1,B", &S::BIT, 8, 1 }, { "BIT 1,C", &S::BIT, 8, 1 }, { "BIT 1,D", &S::BIT, 8, 1 }, { "BIT 1,E", &S::BIT, 8, 1 }, { "BIT 1,H", &S::BIT, 8, 1 }, { "BIT 1,L", &S::BIT, 8, 1 }, { "BIT 1,(HL)", &S::BIT, 12, 1 }, { "BIT 1,A", &S::BIT, 8, 1 },
        { "BIT 2,B", &S::BIT, 8, 1 }, { "BIT 2,C", &S::BIT, 8, 1 }, { "BIT 2,D", &S::BIT, 8, 1 }, { "BIT 2,E", &S::BIT, 8, 1 }, { "BIT 2,H", &S::BIT, 8, 1 }, { "BIT 2,L", &S::BIT, 8, 1 }, { "BIT 2,(HL)", &S::BIT, 12, 1 }, { "BIT 2,A", &S::BIT, 8, 1 }, { "BIT 3,B", &S::BIT, 8, 1 }, { "BIT 3,C", &S::BIT, 8, 1 }, { "BIT 3,D", &S::BIT, 8, 1 }, { "BIT 3,E", &S::BIT, 8, 1 }, { "BIT 3,H", &S::BIT, 8, 1 }, { "BIT 3,L", &S::BIT, 8, 1 }, { "BIT 3,(HL)", &S::BIT, 12, 1 }, { "BIT 3,A", &S::BIT, 8, 1 },
        { "BIT 4,B", &S::BIT, 8, 1 }, { "BIT 4,C", &S::BIT, 8, 1 }, { "BIT 4,D", &S::BIT, 8, 1 }, { "BIT 4,E", &S::BIT, 8, 1 }, { "BIT 4,H", &S::BIT, 8, 1 }, { "BIT 4,L", &S::BIT, 8, 1 }, { "BIT 4,(HL)", &S::BIT, 12, 1 }, { "BIT 4,A", &S::BIT, 8, 1 }, { "BIT 5,B", &S::BIT, 8, 1 }, { "BIT 5,C", &S::BIT, 8, 1 }, { "BIT 5,D", &S::BIT, 8, 1 }, { "BIT 5,E", &S::BIT, 8, 1 }, { "BIT 5,H", &S::BIT, 8, 1 }, { "BIT 5,L", &S::BIT, 8, 1 }, { "BIT 5,(HL)", &S::BIT, 12, 1 }, { "BIT 5,A", &S::BIT, 8, 1 },
        { "BIT 6,B", &S::BIT, 8, 1 }, { "BIT 6,C", &S::BIT, 8, 1 }, { "BIT 6,D", &S::BIT, 8, 1 }, { "BIT 6,E", &S::BIT, 8, 1 }, { "BIT 6,H", &S::BIT, 8, 1 }, { "BIT 6,L", &S::BIT, 8, 1 }, { "BIT 6,(HL)", &S::BIT, 12, 1 }, { "BIT 6,A", &S::BIT, 8, 1 }, { "BIT 7,B", &S::BIT, 8, 1 }, { "BIT 7,C", &S::BIT, 8, 1 }, { "BIT 7,D", &S::BIT, 8, 1 }, { "BIT 7,E", &S::BIT, 8, 1 }, { "BIT 7,H", &S::BIT, 8, 1 }, { "BIT 7,L", &S::BIT, 8, 1 }, { "BIT 7,(HL)", &S::BIT, 12, 1 }, { "BIT 7,A", &S::BIT, 8, 1 },
        { "RES 0,B", &S::RES, 8, 1 }, { "RES 0,C", &S::RES, 8, 1 }, { "RES 0,D", &S::RES, 8, 1 }, { "RES 0,E", &S::RES, 8, 1 }, { "RES 0,H", &S::RES, 8, 1 }, { "RES 0,L", &S::RES, 8, 1 }, { "RES 0,(HL)", &S::RES, 16, 1 }, { "RES 0,A", &S::RES, 8, 1 }, { "RES 1,B", &S::RES, 8, 1 }, { "RES 1,C", &S::RES, 8, 1 }, { "RES 1,D", &S::RES, 8, 1 }, { "RES 1,E", &S::RES, 8, 1 }, { "RES 1,H", &S::RES, 8, 1 }, { "RES 1,L", &S::RES, 8, 1 }, { "RES 1,(HL)", &S::RES, 16, 1 }, { "RES 1,A", &S::RES, 8, 1 },
        { "RES 2,B", &S::RES, 8, 1 }, { "RES 2,C", &S::RES, 8, 1 }, { "RES 2,D", &S::RES, 8, 1 }, { "RES 2,E", &S::RES, 8, 1 }, { "RES 2,H", &S::RES, 8, 1 }, { "RES 2,L", &S::RES, 8, 1 }, { "RES 2,(HL)", &S::RES, 16, 1 }, { "RES 2,A", &S::RES, 8, 1 }, { "RES 3,B", &S::RES, 8, 1 }, { "RES 3,C", &S::RES, 8, 1 }, { "RES 3,D", &S::RES, 8, 1 }, { "RES 3,E", &S::RES, 8, 1 }, { "RES 3,H", &S::RES, 8, 1 }, { "RES 3,L", &S::RES, 8, 1 }, { "RES 3,(HL)", &S::RES, 16, 1 }, { "RES 3,A", &S::RES, 8, 1 },
        { "RES 4,B", &S::RES, 8, 1 }, { "RES 4,C", &S::RES, 8, 1 }, { "RES 4,D", &S::RES, 8, 1 }, { "RES 4,E", &S::RES, 8, 1 }, { "RES 4,H", &S::RES, 8, 1 }, { "RES 4,L", &S::RES, 8, 1 }, { "RES 4,(HL)", &S::RES, 16, 1 }, { "RES 4,A", &S::RES, 8, 1 }, { "RES 5,B", &S::RES, 8, 1 }, { "RES 5,C", &S::RES, 8, 1 }, { "RES 5,D", &S::RES, 8, 1 }, { "RES 5,E", &S::RES, 8, 1 }, { "RES 5,H", &S::RES, 8, 1 }, { "RES 5,L", &S::RES, 8, 1 }, { "RES 5,(HL)", &S::RES, 16, 1 }, { "RES 5,A", &S::RES, 8, 1 },
        { "RES 6,B", &S::RES, 8, 1 }, { "RES 6,C", &S::RES, 8, 1 }, { "RES 6,D", &S::RES, 8, 1 }, { "RES 6,E", &S::RES, 8, 1 }, { "RES 6,H", &S::RES, 8, 1 }, { "RES 6,L", &S::RES, 8, 1 }, { "RES 6,(HL)", &S::RES, 16, 1 }, { "RES 6,A", &S::RES, 8, 1 }, { "RES 7,B", &S::RES, 8, 1 }, { "RES 7,C", &S::RES, 8, 1 }, { "RES 7,D", &S::RES, 8, 1 }, { "RES 7,E", &S::RES, 8, 1 }, { "RES 7,H", &S::RES, 8, 1 }, { "RES 7,L", &S::RES, 8, 1 }, { "RES 7,(HL)", &S::RES, 16, 1 }, { "RES 7,A", &S::RES, 8, 1 },
        { "SET 0,B", &S::SET, 8, 1 }, { "SET 0,C", &S::SET, 8, 1 }, { "SET 0,D", &S::SET, 8, 1 }, { "SET 0,E", &S::SET, 8, 1 }, { "SET 0,H", &S::SET, 8, 1 }, { "SET 0,L", &S::SET, 8, 1 }, { "SET 0,(HL)", &S::SET, 16, 1 }, { "SET 0,A", &S::SET, 8, 1 }, { "SET 1,B", &S::SET, 8, 1 }, { "SET 1,C", &S::SET, 8, 1 }, { "SET 1,D", &S::SET, 8, 1 }, { "SET 1,E", &S::SET, 8, 1 }, { "SET 1,H", &S::SET, 8, 1 }, { "SET 1,L", &S::SET, 8, 1 }, { "SET 1,(HL)", &S::SET, 16, 1 }, { "SET 1,A", &S::SET, 8, 1 },
        { "SET 2,B", &S::SET, 8, 1 }, { "SET 2,C", &S::SET, 8, 1 }, { "SET 2,D", &S::SET, 8, 1 }, { "SET 2,E", &S::SET, 8, 1 }, { "SET 2,H", &S::SET, 8, 1 }, { "SET 2,L", &S::SET, 8, 1 }, { "SET 2,(HL)", &S::SET, 16, 1 }, { "SET 2,A", &S::SET, 8, 1 }, { "SET 3,B", &S::SET, 8, 1 }, { "SET 3,C", &S::SET, 8, 1 }, { "SET 3,D", &S::SET, 8, 1 }, { "SET 3,E", &S::SET, 8, 1 }, { "SET 3,H", &S::SET, 8, 1 }, { "SET 3,L", &S::SET, 8, 1 }, { "SET 3,(HL)", &S::SET, 16, 1 }, { "SET 3,A", &S::SET, 8, 1 },
        { "SET 4,B", &S::SET, 8, 1 }, { "SET 4,C", &S::SET, 8, 1 }, { "SET 4,D", &S::SET, 8, 1 }, { "SET 4,E", &S::SET, 8, 1 }, { "SET 4,H", &S::SET, 8, 1 }, { "SET 4,L", &S::SET, 8, 1 }, { "SET 4,(HL)", &S::SET, 16, 1 }, { "SET 4,A", &S::SET, 8, 1 }, { "SET 5,B", &S::SET, 8, 1 }, { "SET 5,C", &S::SET, 8, 1 }, { "SET 5,D", &S::SET, 8, 1 }, { "SET 5,E", &S::SET, 8, 1 }, { "SET 5,H", &S::SET, 8, 1 }, { "SET 5,L", &S::SET, 8, 1 }, { "SET 5,(HL)", &S::SET, 16, 1 }, { "SET 5,A", &S::SET, 8, 1 },
        { "SET 6,B", &S::SET, 8, 1 }, { "SET 6,C", &S::SET, 8, 1 }, { "SET 6,D", &S::SET, 8, 1 }, { "SET 6,E", &S::SET, 8, 1 }, { "SET 6,H", &S::SET, 8, 1 }, { "SET 6,L", &S::SET, 8, 1 }, { "SET 6,(HL)", &S::SET, 16, 1 }, { "SET 6,A", &S::SET, 8, 1 }, { "SET 7,B", &S::SET, 8, 1 }, { "SET 7,C", &S::SET, 8, 1 }, { "SET 7,D", &S::SET, 8, 1 }, { "SET 7,E", &S::SET, 8, 1 }, { "SET 7,H", &S::SET, 8, 1 }, { "SET 7,L", &S::SET, 8, 1 }, { "SET 7,(HL)", &S::SET, 16, 1 }, { "SET 7,A", &S::SET, 8, 1 },
    };
}

bool SHARP_LR35902::getFlagBit(Flags p_mask)
{
    return p_mask & flags;
}

void SHARP_LR35902::setFlagBit(Flags p_mask, bool p_state) 
{
    if(p_state == true)
    {
        // Set a bit.
        flags |= p_mask;
    }
    else
    {
        // Reset a bit.
        flags &= ~p_mask;
    }
}

uint8_t SHARP_LR35902::nextInstruction() 
{
    current_cycle_count = 0;

    opcode = memory.read(pc);
    pc++;

    if(opcode == 0xcb)
    {
        // Execute opcode prefixed with 0xcb.
        opcode = memory.read(pc);
        pc++;
        (this->*prefix_instruction_table[opcode].func)();
        current_cycle_count += prefix_instruction_table[opcode].cycles;
    }
    else
    {
        // Execute opcode.
        (this->*instruction_table[opcode].func)();

        /*
        JP, JR, CALL, RET instructions can take a different amount of cycles to execute depending on whether the jump actually happens or not.
        In order to save this extra information we use the upper 2 bytes of the 4 byte integer "cycles" to store the amount of cycles when the jump was taken and
        the lower 2 bytes for the amount of cycles when the jump was not taken.
        */
        current_cycle_count += instruction_table[opcode].cycles & 0xffff; 

        // Extra cycles for CALL
        if((opcode == 0xc4) && !getFlagBit(Flags::Zero)) current_cycle_count += 12;
        if((opcode == 0xd4) && !getFlagBit(Flags::Carry)) current_cycle_count += 12;
        if((opcode == 0xcc) && getFlagBit(Flags::Zero)) current_cycle_count += 12;
        if((opcode == 0xdc) && getFlagBit(Flags::Carry)) current_cycle_count += 12;

        // Extra cycles for RET
        if((opcode == 0xc0) && !getFlagBit(Flags::Zero)) current_cycle_count += 12;
        if((opcode == 0xc8) && getFlagBit(Flags::Zero)) current_cycle_count += 12;
        if((opcode == 0xd0) && !getFlagBit(Flags::Carry)) current_cycle_count += 12;
        if((opcode == 0xd8) && getFlagBit(Flags::Carry)) current_cycle_count += 12;

        // Extra cycles for JP
        if((opcode == 0xc2) && !getFlagBit(Flags::Zero)) current_cycle_count += 4;
        if((opcode == 0xd2) && !getFlagBit(Flags::Carry)) current_cycle_count += 4;
        if((opcode == 0xda) && getFlagBit(Flags::Zero)) current_cycle_count += 4;
        if((opcode == 0xca) && getFlagBit(Flags::Carry)) current_cycle_count += 4;

        // Extra cycles for JR
        if((opcode == 0x20) && !getFlagBit(Flags::Zero)) current_cycle_count += 4;
        if((opcode == 0x28) && getFlagBit(Flags::Zero)) current_cycle_count += 4;
        if((opcode == 0x30) && !getFlagBit(Flags::Carry)) current_cycle_count += 4;
        if((opcode == 0x38) && getFlagBit(Flags::Carry)) current_cycle_count += 4;
    }

    return current_cycle_count;
}

void SHARP_LR35902::requestInterrupt(Interrupt p_interrupt) 
{
    memory.write(0xff0f, memory.read(0xff0f) | p_interrupt);
}

void SHARP_LR35902::reset()
{
    // The boot-ROM sets some of these values (sp, pc, others I am not sure).
    a = 0x11;
    b = 0x01;
    c = 0x00;
    d = 0x00;
    e = 0x08;
    h = 0x00;
    l = 0x7c;
    flags = 0x00;
    sp = 0xfffe; // On power-up the stack-pointer is set to 0xfffe.
    pc = 0x0100; // On power-up the program counter is set to 0x100.

    opcode = 0x00;
    operand1 = 0x00;
    operand2 = 0x00;

    interrupt_master_enable = false;
    memory.write(0xff0f, 0x00); // Interrupt request.
    memory.write(0xffff, 0x00); // Interrupt enable.
}

// INSTRUCTION IMPLEMENTATION //

void SHARP_LR35902::NOP() 
{
    // No operation.
}

void SHARP_LR35902::STOP() 
{
    // if(!(memory.Read(0xff0f) & 0b00010000))
    // {
    //     stopped = true;
    //     pc--;
    // }
    // stopped = false;
    std::cout << "Tried STOP." << std::endl;
}

void SHARP_LR35902::DAA()
{
    // After adding or subtracting two BCD (Binary-Coded-Decimal) numbers, the instruction DAA is used to put the result stored in the accumulator into BCD-format
    // as well. The instruction uses the carry and half carry flags to do so (only instruction that uses half carry).
    // You first look at the msb, then the lsb. Depending on the flags and the actual value in a, you add or subtract 0x6 for the lsb or 0x60 for the msb.

    /* TRYING A MORE INDEPTH EXPLANATION (for addition)
    Looking at the values a through f, one can see that adding 6 will give these results.
    0xa + 6 = 0x10 (16 in decimal)
    0xb + 6 = 0x11 (17 in decimal)
    0xc + 6 = 0x12 (18 in decimal)
    0xd + 6 = 0x13 (19 in decimal)
    0xe + 6 = 0x14 (20 in decimal)
    0xf + 6 = 0x15 (21 in decimal)

    Let's say we add 0xa0 and 0x0a together. The result is 0xaa. These are the steps to transfrom it into a bcd number:
    1. We change the upper byte of 0xaa according to the table by adding 6 to the upper byte (0x60).
    = 0x10a
    2. The we change the lower byte the same way.
    = 0x110
    Note that 0x10 was carried into the next number. Note also that the bcd number has a carry, so that flag would be set.
    Finally note we only have to do this if the value is greater than 0x9. Because everything else is already in bcd format.

    Then the bcd numbers written in decimal 100 (0xa0) + 0x010 (0x0a) = 0x110 (0xaa).

    It is about the same for subtraction.

    Source: https://forums.nesdev.com/viewtopic.php?t=15944 10/07/2021 15:13
    */

    if(!getFlagBit(Flags::Subtraction)) // If an addition was performed.
    {
        if(getFlagBit(Flags::Carry) || a > 0x99)
        {
            a += 0x60;
            setFlagBit(Flags::Carry, true); // Carry is set when the addition of the two bcd numbers is bigger than 99. E.g. 0x50 + 0x99 = 0xe9 and 145 in bcd -> Carry is set.
        }
        if(getFlagBit(Flags::HalfCarry) || (a & 0xf) > 0x9)
        {
            a += 0x6;
        }
    }
    else if(getFlagBit(Flags::Subtraction)) // If a subtraction was performed.
    {
        if(getFlagBit(Flags::Carry))
        {
            a -= 0x60;
        }
        if(getFlagBit(Flags::HalfCarry))
        {
            a -= 0x6;
        }
    }

    setFlagBit(Flags::Zero, a == 0);
    setFlagBit(Flags::HalfCarry, false);
}

void SHARP_LR35902::CPL() 
{
    a = ~a;
    setFlagBit(Flags::Subtraction, true);
    setFlagBit(Flags::HalfCarry, true);
}

void SHARP_LR35902::SCF() 
{
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
    setFlagBit(Flags::Carry, true);
}

void SHARP_LR35902::CCF() 
{
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
    // Toggle/flip carry flag.
    setFlagBit(Flags::Carry, !getFlagBit(Flags::Carry));
}

void SHARP_LR35902::DI() 
{
    interrupt_master_enable = false;
}

void SHARP_LR35902::EI() 
{
    interrupt_master_enable = true;
}

void SHARP_LR35902::RST() 
{
    // Push present address onto stack.
    sp--;
    memory.write(sp, pc >> 8);
    sp--;
    memory.write(sp, pc & 0x00ff);

    switch(opcode)
    {
        case 0xc7: pc = 0x00;
        break;
        case 0xcf: pc = 0x08;
        break;
        case 0xd7: pc = 0x10;
        break;
        case 0xdf: pc = 0x18;
        break;
        case 0xe7: pc = 0x20;
        break;
        case 0xef: pc = 0x28;
        break;
        case 0xf7: pc = 0x30;
        break;
        case 0xff: pc = 0x38;
        break;
    }
}

void SHARP_LR35902::HALT()
{
    // Halt as long as the bitwise AND of IE and IF is non-zero. 
    if((memory.read(0xff0f) & memory.read(0xffff)) == 0)
    {
        halted = true;
        pc--;
    }
    halted = false;
}

void SHARP_LR35902::CP() 
{
    uint16_t n;
    switch(opcode)
    {
        case 0xb8:
        n = b;
        break;
        case 0xb9:
        n = c;        
        break;
        case 0xba:
        n = d;
        break;
        case 0xbb:
        n = e;
        break;
        case 0xbc:
        n = h;
        break;
        case 0xbd:
        n = l;
        break;
        case 0xbe:
        n = memory.read(h << 8 | l);
        break;
        case 0xbf:
        n = a;
        break;
        case 0xfe:
        n = memory.read(pc);
        pc++;
        break;
    }
    setFlagBit(Flags::Zero, a == n);
    setFlagBit(Flags::Subtraction, true);
    setFlagBit(Flags::HalfCarry, (a & 0x0f) < (n & 0x0f));
    setFlagBit(Flags::Carry, a < n); // If the minuend is lower than the subtrahend there will be a borrow.
}

void SHARP_LR35902::LD() 
{
    uint16_t hl;
    int8_t n;

    switch(opcode)
    {
        case 0x01:
        operand1 = memory.read(pc);
        pc++;
        operand2 = memory.read(pc);
        pc++;
        b = operand2;
        c = operand1;
        break;
        case 0x02:
        memory.write(b << 8 | c, a);
        break;
        case 0x06:
        operand1 = memory.read(pc);
        pc++;
        b = operand1;
        break;
        case 0x08:
        operand1 = memory.read(pc);
        pc++;
        operand2 = memory.read(pc);
        pc++;
        memory.write(operand2 << 8 | operand1, sp & 0x00ff);
        memory.write((operand2 << 8 | operand1) + 1, sp >> 8);
        break;
        case 0x0a:
        a = memory.read(b << 8 | c);
        break;
        case 0x0e:
        operand1 = memory.read(pc);
        pc++;
        c = operand1;
        break;
        case 0x11:
        operand1 = memory.read(pc);
        pc++;
        operand2 = memory.read(pc);
        pc++;
        d = operand2;
        e = operand1;
        break;
        case 0x12:
        memory.write(d << 8 | e, a);
        break;
        case 0x16:
        operand1 = memory.read(pc);
        pc++;
        d = operand1;
        break;
        case 0x1a:
        a = memory.read(d << 8 | e);
        break;
        case 0x1e:
        operand1 = memory.read(pc);
        pc++;
        e = operand1;
        break;
        case 0x21:
        operand1 = memory.read(pc);
        pc++;
        operand2 = memory.read(pc);
        pc++;
        h = operand2;
        l = operand1;
        break;
        case 0x22:
        hl = h << 8 | l; 
        memory.write(hl, a);
        hl++;
        h = hl >> 8;
        l = hl & 0x00ff;
        break;
        case 0x26:
        operand1 = memory.read(pc);
        pc++;
        h = operand1;
        break;
        case 0x2a:
        a = memory.read(h << 8 | l);
        hl = h << 8 | l; 
        hl++;
        h = hl >> 8;
        l = hl & 0x00ff;
        break;
        case 0x2e:
        operand1 = memory.read(pc);
        pc++;
        l = operand1;
        break;
        case 0x31:
        operand1 = memory.read(pc);
        pc++;
        operand2 = memory.read(pc);
        pc++;
        sp = operand2 << 8 | operand1;
        break;
        case 0x32:
        hl = h << 8 | l;
        memory.write(hl, a);
        hl--;
        h = hl >> 8;
        l = hl & 0x00ff;
        break;
        case 0x36:
        operand1 = memory.read(pc);
        pc++;
        memory.write(h << 8 | l, operand1);
        break;
        case 0x3a:
        hl = h << 8 | l;
        a = memory.read(hl);
        hl--;
        h = hl >> 8;
        l = hl & 0x00ff;
        break;
        case 0x3e:
        operand1 = memory.read(pc);
        pc++;
        a = operand1;
        break;
        case 0x40:
		b = b;
		break;
		case 0x41:
		b = c;
		break;
		case 0x42:
		b = d;
		break;
		case 0x43:
		b = e;
		break;
		case 0x44:
		b = h;
		break;
		case 0x45:
		b = l;
		break;
		case 0x46:
		b = memory.read(h << 8 | l);
		break;
		case 0x47:
		b = a;
		break;
		case 0x48:
		c = b;
		break;
		case 0x49:
		c = c;
		break;
		case 0x4a:
		c = d;
		break;
		case 0x4b:
		c = e;
		break;
		case 0x4c:
		c = h;
		break;
		case 0x4d:
		c = l;
		break;
		case 0x4e:
		c = memory.read(h << 8 | l);
		break;
		case 0x4f:
		c = a;
		break;
		case 0x50:
		d = b;
		break;
		case 0x51:
		d = c;
		break;
		case 0x52:
		d = d;
		break;
		case 0x53:
		d = e;
		break;
		case 0x54:
		d = h;
		break;
		case 0x55:
		d = l;
		break;
		case 0x56:
		d = memory.read(h << 8 | l);
		break;
		case 0x57:
		d = a;
		break;
		case 0x58:
		e = b;
		break;
		case 0x59:
		e = c;
		break;
		case 0x5a:
		e = d;
		break;
		case 0x5b:
		e = e;
		break;
		case 0x5c:
		e = h;
		break;
		case 0x5d:
		e = l;
		break;
		case 0x5e:
		e = memory.read(h << 8 | l);
		break;
		case 0x5f:
		e = a;
		break;
		case 0x60:
		h = b;
		break;
		case 0x61:
		h = c;
		break;
		case 0x62:
		h = d;
		break;
		case 0x63:
		h = e;
		break;
		case 0x64:
		h = h;
		break;
		case 0x65:
		h = l;
		break;
		case 0x66:
		h = memory.read(h << 8 | l);
		break;
		case 0x67:
		h = a;
		break;
		case 0x68:
		l = b;
		break;
		case 0x69:
		l = c;
		break;
		case 0x6a:
		l = d;
		break;
		case 0x6b:
		l = e;
		break;
		case 0x6c:
		l = h;
		break;
		case 0x6d:
		l = l;
		break;
		case 0x6e:
		l = memory.read(h << 8 | l);
		break;
		case 0x6f:
		l = a;
		break;
		case 0x70:
		memory.write(h << 8 | l, b);
		break;
		case 0x71:
		memory.write(h << 8 | l, c);
		break;
		case 0x72:
		memory.write(h << 8 | l, d);
		break;
		case 0x73:
		memory.write(h << 8 | l, e);
		break;
		case 0x74:
		memory.write(h << 8 | l, h);
		break;
		case 0x75:
		memory.write(h << 8 | l, l);
		break;
		case 0x77:
		memory.write(h << 8 | l, a);
		break;
		case 0x78:
		a = b;
		break;
		case 0x79:
		a = c;
		break;
		case 0x7a:
		a = d;
        break;
		case 0x7b:
		a = e;
		break;
		case 0x7c:
		a = h;
		break;
		case 0x7d:
		a = l;
		break;
		case 0x7e:
		a = memory.read(h << 8 | l);
		break;
		case 0x7f:
		a = a;
		break;
		case 0xe0:
		operand1 = memory.read(pc);
		pc++;
		memory.write(0xff00 + operand1, a);
		break;
		case 0xe2:
		memory.write(0xff00 + c, a);
		break;
		case 0xea:
        operand1 = memory.read(pc);
        pc++;
        operand2 = memory.read(pc);
        pc++;
        memory.write(operand2 << 8 | operand1, a);
		break;
		case 0xf0:
        operand1 = memory.read(pc);
		pc++;
		a = memory.read(0xff00 + operand1);
		break;
		case 0xf2:
        a = memory.read(0xff00 + c);
		break;
		case 0xfa:
        operand1 = memory.read(pc);
        pc++;
        operand2 = memory.read(pc);
        pc++;
        a = memory.read(operand2 << 8 | operand1);
		break;
        case 0xf8:
        n = memory.read(pc);
        pc++;
        hl = sp + (int8_t)n;
        h = hl >> 8;
        l = hl & 0x00ff;
        setFlagBit(Flags::Zero, false);
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::HalfCarry, ((sp & 0x000f) + ((uint16_t)n & 0x000f)) > 0xf);
        setFlagBit(Flags::Carry, ((sp & 0x00ff) + ((uint16_t)n & 0x00ff)) > 0xff);
        break;
        case 0xf9:
        sp = h << 8 | l;
        break;
    }
}

void SHARP_LR35902::POP() 
{
    switch(opcode)
    {
        case 0xc1:
        c = memory.read(sp);
        sp++;
        b = memory.read(sp);
        sp++;
        break;
        case 0xd1:
        e = memory.read(sp);
        sp++;
        d = memory.read(sp);
        sp++;
        break;
        case 0xe1:
        l = memory.read(sp);
        sp++;
        h = memory.read(sp);
        sp++;
        break;
        case 0xf1:
        flags = memory.read(sp) & 0b11110000; // The bottom 4 bits of the flag register need to stay 0 all the time.
        sp++;
        a = memory.read(sp);
        sp++;
        break;
    }
}

void SHARP_LR35902::PUSH() 
{
    switch(opcode)
    {
        case 0xc5:
        sp--;
        memory.write(sp, b);
        sp--;
        memory.write(sp, c);
        break;
        case 0xd5:
        sp--;
        memory.write(sp, d);
        sp--;
        memory.write(sp, e);
        break;
        case 0xe5:
        sp--;
        memory.write(sp, h);
        sp--;
        memory.write(sp, l);
        break;
        case 0xf5:
        sp--;
        memory.write(sp, a);
        sp--;
        memory.write(sp, flags);
        break;
    }
}

void SHARP_LR35902::AND() 
{
    switch(opcode)
    {
        case 0xa0:
        a &= b;
        break;
        case 0xa1:
        a &= c;        
        break;
        case 0xa2:
        a &= d;
        break;
        case 0xa3:
        a &= e;
        break;
        case 0xa4:
        a &= h;
        break;
        case 0xa5:
        a &= l;
        break;
        case 0xa6:
        a &= memory.read(h << 8 | l);
        break;
        case 0xa7:
        a &= a;
        break;
        case 0xe6:
        a &= memory.read(pc);
        pc++;
        break;
    }
    setFlagBit(Flags::Zero, a == 0);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, true);
    setFlagBit(Flags::Carry, false);
}

void SHARP_LR35902::OR() 
{
    switch(opcode)
    {
        case 0xb0:
        a |= b;
        break;
        case 0xb1:
        a |= c;        
        break;
        case 0xb2:
        a |= d;
        break;
        case 0xb3:
        a |= e;
        break;
        case 0xb4:
        a |= h;
        break;
        case 0xb5:
        a |= l;
        break;
        case 0xb6:
        a |= memory.read(h << 8 | l);
        break;
        case 0xb7:
        a |= a;
        break;
        case 0xf6:
        a |= memory.read(pc);
        pc++;
        break;
    }
    setFlagBit(Flags::Zero, a == 0);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
    setFlagBit(Flags::Carry, false);    
}

void SHARP_LR35902::XOR() 
{
    switch(opcode)
    {
        case 0xa8:
        a ^= b;
        break;
        case 0xa9:
        a ^= c;        
        break;
        case 0xaa:
        a ^= d;
        break;
        case 0xab:
        a ^= e;
        break;
        case 0xac:
        a ^= h;
        break;
        case 0xad:
        a ^= l;
        break;
        case 0xae:
        a ^= memory.read(h << 8 | l);
        break;
        case 0xaf:
        a ^= a;
        break;
        case 0xee:
        a ^= memory.read(pc);
        pc++;
        break;
    }
    setFlagBit(Flags::Zero, a == 0);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
    setFlagBit(Flags::Carry, false);
}

uint8_t SetBit(uint8_t digit, uint8_t number)
{
    return number | (1 << digit);
}
uint8_t ResetBit(uint8_t digit, uint8_t number)
{
    return number & ~(1 << digit);
}
bool TestBit(int digit, uint8_t number)
{
    // If digit is set in number, returns 0. If reset, returns 1.
    return !(number & (1 << digit));
}

void SHARP_LR35902::SET() 
{
    switch(opcode)
    {
        case 0xc0: b = SetBit(0, b); break;
        case 0xc1: c = SetBit(0, c); break;
        case 0xc2: d = SetBit(0, d); break;
        case 0xc3: e = SetBit(0, e); break;
        case 0xc4: h = SetBit(0, h); break;
        case 0xc5: l = SetBit(0, l); break;
        case 0xc6: memory.write(h << 8 | l, SetBit(0, memory.read(h << 8 | l))); break;
        case 0xc7: a = SetBit(0, a); break;

        case 0xc8: b = SetBit(1, b); break;
        case 0xc9: c = SetBit(1, c); break;
        case 0xca: d = SetBit(1, d); break;
        case 0xcb: e = SetBit(1, e); break;
        case 0xcc: h = SetBit(1, h); break;
        case 0xcd: l = SetBit(1, l); break;
        case 0xce: memory.write(h << 8 | l, SetBit(1, memory.read(h << 8 | l))); break;
        case 0xcf: a = SetBit(1, a); break;

        case 0xd0: b = SetBit(2, b); break;
        case 0xd1: c = SetBit(2, c); break;
        case 0xd2: d = SetBit(2, d); break;
        case 0xd3: e = SetBit(2, e); break;
        case 0xd4: h = SetBit(2, h); break;
        case 0xd5: l = SetBit(2, l); break;
        case 0xd6: memory.write(h << 8 | l, SetBit(2, memory.read(h << 8 | l))); break;
        case 0xd7: a = SetBit(2, a); break;

        case 0xd8: b = SetBit(3, b); break;
        case 0xd9: c = SetBit(3, c); break;
        case 0xda: d = SetBit(3, d); break;
        case 0xdb: e = SetBit(3, e); break;
        case 0xdc: h = SetBit(3, h); break;
        case 0xdd: l = SetBit(3, l); break;
        case 0xde: memory.write(h << 8 | l, SetBit(3, memory.read(h << 8 | l))); break;
        case 0xdf: a = SetBit(3, a); break;

        case 0xe0: b = SetBit(4, b); break;
        case 0xe1: c = SetBit(4, c); break;
        case 0xe2: d = SetBit(4, d); break;
        case 0xe3: e = SetBit(4, e); break;
        case 0xe4: h = SetBit(4, h); break;
        case 0xe5: l = SetBit(4, l); break;
        case 0xe6: memory.write(h << 8 | l, SetBit(4, memory.read(h << 8 | l))); break;
        case 0xe7: a = SetBit(4, a); break;

        case 0xe8: b = SetBit(5, b); break;
        case 0xe9: c = SetBit(5, c); break;
        case 0xea: d = SetBit(5, d); break;
        case 0xeb: e = SetBit(5, e); break;
        case 0xec: h = SetBit(5, h); break;
        case 0xed: l = SetBit(5, l); break;
        case 0xee: memory.write(h << 8 | l, SetBit(5, memory.read(h << 8 | l))); break;
        case 0xef: a = SetBit(5, a); break;

        case 0xf0: b = SetBit(6, b); break;
        case 0xf1: c = SetBit(6, c); break;
        case 0xf2: d = SetBit(6, d); break;
        case 0xf3: e = SetBit(6, e); break;
        case 0xf4: h = SetBit(6, h); break;
        case 0xf5: l = SetBit(6, l); break;
        case 0xf6: memory.write(h << 8 | l, SetBit(6, memory.read(h << 8 | l))); break;
        case 0xf7: a = SetBit(6, a); break;

        case 0xf8: b = SetBit(7, b); break;
        case 0xf9: c = SetBit(7, c); break;
        case 0xfa: d = SetBit(7, d); break;
        case 0xfb: e = SetBit(7, e); break;
        case 0xfc: h = SetBit(7, h); break;
        case 0xfd: l = SetBit(7, l); break;
        case 0xfe: memory.write(h << 8 | l, SetBit(7, memory.read(h << 8 | l))); break;
        case 0xff: a = SetBit(7, a); break;
    }
}

void SHARP_LR35902::RES() 
{
    switch(opcode)
    {
        case 0x80: b = ResetBit(0, b); break;
        case 0x81: c = ResetBit(0, c); break;
        case 0x82: d = ResetBit(0, d); break;
        case 0x83: e = ResetBit(0, e); break;
        case 0x84: h = ResetBit(0, h); break;
        case 0x85: l = ResetBit(0, l); break;
        case 0x86: memory.write(h << 8 | l, ResetBit(0, memory.read(h << 8 | l))); break;
        case 0x87: a = ResetBit(0, a); break;

        case 0x88: b = ResetBit(1, b); break;
        case 0x89: c = ResetBit(1, c); break;
        case 0x8a: d = ResetBit(1, d); break;
        case 0x8b: e = ResetBit(1, e); break;
        case 0x8c: h = ResetBit(1, h); break;
        case 0x8d: l = ResetBit(1, l); break;
        case 0x8e: memory.write(h << 8 | l, ResetBit(1, memory.read(h << 8 | l))); break;
        case 0x8f: a = ResetBit(1, a); break;

        case 0x90: b = ResetBit(2, b); break;
        case 0x91: c = ResetBit(2, c); break;
        case 0x92: d = ResetBit(2, d); break;
        case 0x93: e = ResetBit(2, e); break;
        case 0x94: h = ResetBit(2, h); break;
        case 0x95: l = ResetBit(2, l); break;
        case 0x96: memory.write(h << 8 | l, ResetBit(2, memory.read(h << 8 | l))); break;
        case 0x97: a = ResetBit(2, a); break;

        case 0x98: b = ResetBit(3, b); break;
        case 0x99: c = ResetBit(3, c); break;
        case 0x9a: d = ResetBit(3, d); break;
        case 0x9b: e = ResetBit(3, e); break;
        case 0x9c: h = ResetBit(3, h); break;
        case 0x9d: l = ResetBit(3, l); break;
        case 0x9e: memory.write(h << 8 | l, ResetBit(3, memory.read(h << 8 | l))); break;
        case 0x9f: a = ResetBit(3, a); break;

        case 0xa0: b = ResetBit(4, b); break;
        case 0xa1: c = ResetBit(4, c); break;
        case 0xa2: d = ResetBit(4, d); break;
        case 0xa3: e = ResetBit(4, e); break;
        case 0xa4: h = ResetBit(4, h); break;
        case 0xa5: l = ResetBit(4, l); break;
        case 0xa6: memory.write(h << 8 | l, ResetBit(4, memory.read(h << 8 | l))); break;
        case 0xa7: a = ResetBit(4, a); break;

        case 0xa8: b = ResetBit(5, b); break;
        case 0xa9: c = ResetBit(5, c); break;
        case 0xaa: d = ResetBit(5, d); break;
        case 0xab: e = ResetBit(5, e); break;
        case 0xac: h = ResetBit(5, h); break;
        case 0xad: l = ResetBit(5, l); break;
        case 0xae: memory.write(h << 8 | l, ResetBit(5, memory.read(h << 8 | l))); break;
        case 0xaf: a = ResetBit(5, a); break;

        case 0xb0: b = ResetBit(6, b); break;
        case 0xb1: c = ResetBit(6, c); break;
        case 0xb2: d = ResetBit(6, d); break;
        case 0xb3: e = ResetBit(6, e); break;
        case 0xb4: h = ResetBit(6, h); break;
        case 0xb5: l = ResetBit(6, l); break;
        case 0xb6: memory.write(h << 8 | l, ResetBit(6, memory.read(h << 8 | l))); break;
        case 0xb7: a = ResetBit(6, a); break;

        case 0xb8: b = ResetBit(7, b); break;
        case 0xb9: c = ResetBit(7, c); break;
        case 0xba: d = ResetBit(7, d); break;
        case 0xbb: e = ResetBit(7, e); break;
        case 0xbc: h = ResetBit(7, h); break;
        case 0xbd: l = ResetBit(7, l); break;
        case 0xbe: memory.write(h << 8 | l, ResetBit(7, memory.read(h << 8 | l))); break;
        case 0xbf: a = ResetBit(7, a); break;
    }    
}

void SHARP_LR35902::BIT() 
{
    switch(opcode)
    {
        case 0x40: setFlagBit(Flags::Zero, TestBit(0, b)); break;
        case 0x41: setFlagBit(Flags::Zero, TestBit(0, c)); break;
        case 0x42: setFlagBit(Flags::Zero, TestBit(0, d)); break;
        case 0x43: setFlagBit(Flags::Zero, TestBit(0, e)); break;
        case 0x44: setFlagBit(Flags::Zero, TestBit(0, h)); break;
        case 0x45: setFlagBit(Flags::Zero, TestBit(0, l)); break;
        case 0x46: setFlagBit(Flags::Zero, TestBit(0, memory.read(h << 8 | l)));  break;
        case 0x47: setFlagBit(Flags::Zero, TestBit(0, a)); break;

        case 0x48: setFlagBit(Flags::Zero, TestBit(1, b)); break;
        case 0x49: setFlagBit(Flags::Zero, TestBit(1, c)); break;
        case 0x4a: setFlagBit(Flags::Zero, TestBit(1, d)); break;
        case 0x4b: setFlagBit(Flags::Zero, TestBit(1, e)); break;
        case 0x4c: setFlagBit(Flags::Zero, TestBit(1, h)); break;
        case 0x4d: setFlagBit(Flags::Zero, TestBit(1, l)); break;
        case 0x4e: setFlagBit(Flags::Zero, TestBit(1, memory.read(h << 8 | l)));  break;
        case 0x4f: setFlagBit(Flags::Zero, TestBit(1, a)); break;

        case 0x50: setFlagBit(Flags::Zero, TestBit(2, b)); break;
        case 0x51: setFlagBit(Flags::Zero, TestBit(2, c)); break;
        case 0x52: setFlagBit(Flags::Zero, TestBit(2, d)); break;
        case 0x53: setFlagBit(Flags::Zero, TestBit(2, e)); break;
        case 0x54: setFlagBit(Flags::Zero, TestBit(2, h)); break;
        case 0x55: setFlagBit(Flags::Zero, TestBit(2, l)); break;
        case 0x56: setFlagBit(Flags::Zero, TestBit(2, memory.read(h << 8 | l)));  break;
        case 0x57: setFlagBit(Flags::Zero, TestBit(2, a)); break;

        case 0x58: setFlagBit(Flags::Zero, TestBit(3, b)); break;
        case 0x59: setFlagBit(Flags::Zero, TestBit(3, c)); break;
        case 0x5a: setFlagBit(Flags::Zero, TestBit(3, d)); break;
        case 0x5b: setFlagBit(Flags::Zero, TestBit(3, e)); break;
        case 0x5c: setFlagBit(Flags::Zero, TestBit(3, h)); break;
        case 0x5d: setFlagBit(Flags::Zero, TestBit(3, l)); break;
        case 0x5e: setFlagBit(Flags::Zero, TestBit(3, memory.read(h << 8 | l)));  break;
        case 0x5f: setFlagBit(Flags::Zero, TestBit(3, a)); break;

        case 0x60: setFlagBit(Flags::Zero, TestBit(4, b)); break;
        case 0x61: setFlagBit(Flags::Zero, TestBit(4, c)); break;
        case 0x62: setFlagBit(Flags::Zero, TestBit(4, d)); break;
        case 0x63: setFlagBit(Flags::Zero, TestBit(4, e)); break;
        case 0x64: setFlagBit(Flags::Zero, TestBit(4, h)); break;
        case 0x65: setFlagBit(Flags::Zero, TestBit(4, l)); break;
        case 0x66: setFlagBit(Flags::Zero, TestBit(4, memory.read(h << 8 | l)));  break;
        case 0x67: setFlagBit(Flags::Zero, TestBit(4, a)); break;

        case 0x68: setFlagBit(Flags::Zero, TestBit(5, b)); break;
        case 0x69: setFlagBit(Flags::Zero, TestBit(5, c)); break;
        case 0x6a: setFlagBit(Flags::Zero, TestBit(5, d)); break;
        case 0x6b: setFlagBit(Flags::Zero, TestBit(5, e)); break;
        case 0x6c: setFlagBit(Flags::Zero, TestBit(5, h)); break;
        case 0x6d: setFlagBit(Flags::Zero, TestBit(5, l)); break;
        case 0x6e: setFlagBit(Flags::Zero, TestBit(5, memory.read(h << 8 | l)));  break;
        case 0x6f: setFlagBit(Flags::Zero, TestBit(5, a)); break;

        case 0x70: setFlagBit(Flags::Zero, TestBit(6, b)); break;
        case 0x71: setFlagBit(Flags::Zero, TestBit(6, c)); break;
        case 0x72: setFlagBit(Flags::Zero, TestBit(6, d)); break;
        case 0x73: setFlagBit(Flags::Zero, TestBit(6, e)); break;
        case 0x74: setFlagBit(Flags::Zero, TestBit(6, h)); break;
        case 0x75: setFlagBit(Flags::Zero, TestBit(6, l)); break;
        case 0x76: setFlagBit(Flags::Zero, TestBit(6, memory.read(h << 8 | l)));  break;
        case 0x77: setFlagBit(Flags::Zero, TestBit(6, a)); break;

        case 0x78: setFlagBit(Flags::Zero, TestBit(7, b)); break;
        case 0x79: setFlagBit(Flags::Zero, TestBit(7, c)); break;
        case 0x7a: setFlagBit(Flags::Zero, TestBit(7, d)); break;
        case 0x7b: setFlagBit(Flags::Zero, TestBit(7, e)); break;
        case 0x7c: setFlagBit(Flags::Zero, TestBit(7, h)); break;
        case 0x7d: setFlagBit(Flags::Zero, TestBit(7, l)); break;
        case 0x7e: setFlagBit(Flags::Zero, TestBit(7, memory.read(h << 8 | l)));  break;
        case 0x7f: setFlagBit(Flags::Zero, TestBit(7, a)); break;
    }
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, true);
}

uint8_t RotateRight(uint8_t n, bool& new_carry)
{
    new_carry = n & 1;
    n >>= 1;
    n |= (new_carry << 7);
    return n;
}
uint8_t RotateLeft(uint8_t n, bool& new_carry)
{
    new_carry = n & (1 << 7);
    n <<= 1;
    n |= new_carry;
    return n;
}
uint8_t RotateRightThroughCarry(uint8_t n, bool old_carry, bool& new_carry)
{
    new_carry = n & 1;
    n >>= 1;
    n |= (old_carry << 7);
    return n;
}
uint8_t RotateLeftThroughCarry(uint8_t n, bool old_carry, bool& new_carry)
{
    new_carry = n & (1 << 7);
    n <<= 1;
    n |= old_carry;
    return n;    
}
uint8_t ShiftRightMSBUnchanged(uint8_t n, bool& new_carry)
{
    new_carry = n & 1;
    uint8_t msb = n & (1 << 7);
    n >>= 1;
    n |= msb;
    return n;
}
uint8_t ShiftRight(uint8_t n, bool& new_carry)
{
    new_carry = n & 1;
    n >>= 1;
    return n;
}
uint8_t ShiftLeft(uint8_t n, bool& new_carry)
{
    new_carry = n & (1 << 7);
    n <<= 1;
    return n;
}
  
void SHARP_LR35902::RLCA() 
{
    bool new_carry;
    a = RotateLeft(a, new_carry);
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Zero, false);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
}

void SHARP_LR35902::RRCA() 
{
    bool new_carry;
    a = RotateRight(a, new_carry);
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Zero, false);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
}

void SHARP_LR35902::RLA() 
{
    bool new_carry;
    a = RotateLeftThroughCarry(a, getFlagBit(Flags::Carry), new_carry);
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Zero, false);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
}

void SHARP_LR35902::RRA() 
{
    // Right rotate through carry.
    bool new_carry;
    a = RotateRightThroughCarry(a, getFlagBit(Flags::Carry), new_carry);
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Zero, false);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
}

void SHARP_LR35902::RLC() 
{
    bool new_carry;
    switch(opcode)
    {
        case 0x00:
        b = RotateLeft(b, new_carry);
        setFlagBit(Flags::Zero, b == 0);
        break;
        case 0x01:
        c = RotateLeft(c, new_carry);
        setFlagBit(Flags::Zero, c == 0);
        break;
        case 0x02:
        d = RotateLeft(d, new_carry);
        setFlagBit(Flags::Zero, d == 0);
        break;
        case 0x03:
        e = RotateLeft(e, new_carry);
        setFlagBit(Flags::Zero, e == 0);
        break;
        case 0x04:
        h = RotateLeft(h, new_carry);
        setFlagBit(Flags::Zero, h == 0);
        break;
        case 0x05:
        l = RotateLeft(l, new_carry);
        setFlagBit(Flags::Zero, l == 0);
        break;
        case 0x06:
        memory.write(h << 8 | l, RotateLeft(memory.read(h << 8 | l), new_carry));
        setFlagBit(Flags::Zero, memory.read(h << 8 | l) == 0);
        break;
        case 0x07:
        a = RotateLeft(a, new_carry);
        setFlagBit(Flags::Zero, a == 0);
        break;
    }
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
}

void SHARP_LR35902::RRC() 
{
    bool new_carry;
    switch(opcode)
    {
        case 0x08:
        b = RotateRight(b, new_carry);
        setFlagBit(Flags::Zero, b == 0);
        break;
        case 0x09:
        c = RotateRight(c, new_carry);
        setFlagBit(Flags::Zero, c == 0);
        break;
        case 0x0a:
        d = RotateRight(d, new_carry);
        setFlagBit(Flags::Zero, d == 0);
        break;
        case 0x0b:
        e = RotateRight(e, new_carry);
        setFlagBit(Flags::Zero, e == 0);
        break;
        case 0x0c:
        h = RotateRight(h, new_carry);
        setFlagBit(Flags::Zero, h == 0);
        break;
        case 0x0d:
        l = RotateRight(l, new_carry);
        setFlagBit(Flags::Zero, l == 0);
        break;
        case 0x0e:
        memory.write(h << 8 | l, RotateRight(memory.read(h << 8 | l), new_carry));
        setFlagBit(Flags::Zero, memory.read(h << 8 | l) == 0);
        break;
        case 0x0f:
        a = RotateRight(a, new_carry);
        setFlagBit(Flags::Zero, a == 0);
        break;
    }
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
}

void SHARP_LR35902::RL() 
{
    bool new_carry;
    switch(opcode)
    {
        case 0x10:
        b = RotateLeftThroughCarry(b, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, b == 0);
        break;
        case 0x11:
        c = RotateLeftThroughCarry(c, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, c == 0);
        break;
        case 0x12:
        d = RotateLeftThroughCarry(d, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, d == 0);
        break;
        case 0x13:
        e = RotateLeftThroughCarry(e, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, e == 0);
        break;
        case 0x14:
        h = RotateLeftThroughCarry(h, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, h == 0);
        break;
        case 0x15:
        l = RotateLeftThroughCarry(l, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, l == 0);
        break;
        case 0x16:
        memory.write(h << 8 | l, RotateLeftThroughCarry(memory.read(h << 8 | l), getFlagBit(Flags::Carry), new_carry));
        setFlagBit(Flags::Zero, memory.read(h << 8 | l) == 0);
        break;
        case 0x17:
        a = RotateLeftThroughCarry(a, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, a == 0);
        break;
    }
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
}

void SHARP_LR35902::RR()
{
    bool new_carry;
    switch(opcode)
    {
        case 0x18:
        b = RotateRightThroughCarry(b, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, b == 0);
        break;
        case 0x19:
        c = RotateRightThroughCarry(c, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, c == 0);
        break;
        case 0x1a:
        d = RotateRightThroughCarry(d, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, d == 0);
        break;
        case 0x1b:
        e = RotateRightThroughCarry(e, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, e == 0);
        break;
        case 0x1c:
        h = RotateRightThroughCarry(h, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, h == 0);
        break;
        case 0x1d:
        l = RotateRightThroughCarry(l, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, l == 0);
        break;
        case 0x1e:
        memory.write(h << 8 | l, RotateRightThroughCarry(memory.read(h << 8 | l), getFlagBit(Flags::Carry), new_carry));
        setFlagBit(Flags::Zero, memory.read(h << 8 | l) == 0);
        break;
        case 0x1f:
        a = RotateRightThroughCarry(a, getFlagBit(Flags::Carry), new_carry);
        setFlagBit(Flags::Zero, a == 0);
        break;
    }
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
}

void SHARP_LR35902::SLA() 
{
    bool new_carry;
    switch(opcode)
    {
        case 0x20:
        b = ShiftLeft(b, new_carry);
        setFlagBit(Flags::Zero, b == 0);
        break;
        case 0x21:
        c = ShiftLeft(c, new_carry);
        setFlagBit(Flags::Zero, c == 0);
        break;
        case 0x22:
        d = ShiftLeft(d, new_carry);
        setFlagBit(Flags::Zero, d == 0);
        break;
        case 0x23:
        e = ShiftLeft(e, new_carry);
        setFlagBit(Flags::Zero, e == 0);
        break;
        case 0x24:
        h = ShiftLeft(h, new_carry);
        setFlagBit(Flags::Zero, h == 0);
        break;
        case 0x25:
        l = ShiftLeft(l, new_carry);
        setFlagBit(Flags::Zero, l == 0);
        break;
        case 0x26:
        memory.write(h << 8 | l, ShiftLeft(memory.read(h << 8 | l), new_carry));
        setFlagBit(Flags::Zero, memory.read(h << 8 | l) == 0);
        break;
        case 0x27:
        a = ShiftLeft(a, new_carry);
        setFlagBit(Flags::Zero, a == 0);
        break;
    }
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
}

void SHARP_LR35902::SRA() 
{
    bool new_carry;
    switch(opcode)
    {
        case 0x28:
        b = ShiftRightMSBUnchanged(b, new_carry);
        setFlagBit(Flags::Zero, b == 0);
        break;
        case 0x29:
        c = ShiftRightMSBUnchanged(c, new_carry);
        setFlagBit(Flags::Zero, c == 0);
        break;
        case 0x2a:
        d = ShiftRightMSBUnchanged(d, new_carry);
        setFlagBit(Flags::Zero, d == 0);
        break;
        case 0x2b:
        e = ShiftRightMSBUnchanged(e, new_carry);
        setFlagBit(Flags::Zero, e == 0);
        break;
        case 0x2c:
        h = ShiftRightMSBUnchanged(h, new_carry);
        setFlagBit(Flags::Zero, h == 0);
        break;
        case 0x2d:
        l = ShiftRightMSBUnchanged(l, new_carry);
        setFlagBit(Flags::Zero, l == 0);
        break;
        case 0x2e:
        memory.write(h << 8 | l, ShiftRightMSBUnchanged(memory.read(h << 8 | l), new_carry));
        setFlagBit(Flags::Zero, memory.read(h << 8 | l) == 0);
        break;
        case 0x2f:
        a = ShiftRightMSBUnchanged(a, new_carry);
        setFlagBit(Flags::Zero, a == 0);
        break;
    }
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);    
}

void SHARP_LR35902::SRL() 
{
    bool new_carry;
    switch(opcode)
    {
        case 0x38:
        b = ShiftRight(b, new_carry);
        setFlagBit(Flags::Zero, b == 0);
        break;
        case 0x39:
        c = ShiftRight(c, new_carry);
        setFlagBit(Flags::Zero, c == 0);
        break;
        case 0x3a:
        d = ShiftRight(d, new_carry);
        setFlagBit(Flags::Zero, d == 0);
        break;
        case 0x3b:
        e = ShiftRight(e, new_carry);
        setFlagBit(Flags::Zero, e == 0);
        break;
        case 0x3c:
        h = ShiftRight(h, new_carry);
        setFlagBit(Flags::Zero, h == 0);
        break;
        case 0x3d:
        l = ShiftRight(l, new_carry);
        setFlagBit(Flags::Zero, l == 0);
        break;
        case 0x3e:
        memory.write(h << 8 | l, ShiftRight(memory.read(h << 8 | l), new_carry));
        setFlagBit(Flags::Zero, memory.read(h << 8 | l) == 0);
        break;
        case 0x3f:
        a = ShiftRight(a, new_carry);
        setFlagBit(Flags::Zero, a == 0);
        break;
    }
    setFlagBit(Flags::Carry, new_carry);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);       
}

uint8_t SwapNibles(uint8_t n)
{
    return (n << 4) | (n >> 4);
}

void SHARP_LR35902::SWAP() 
{
    switch(opcode)
    {
        case 0x30: 
        b = SwapNibles(b); 
        setFlagBit(Flags::Zero, b == 0);
        break;
        case 0x31: 
        c = SwapNibles(c); 
        setFlagBit(Flags::Zero, c == 0);
        break;
        case 0x32: 
        d = SwapNibles(d); 
        setFlagBit(Flags::Zero, d == 0);
        break;
        case 0x33: 
        e = SwapNibles(e); 
        setFlagBit(Flags::Zero, e == 0);
        break;
        case 0x34: 
        h = SwapNibles(h); 
        setFlagBit(Flags::Zero, h == 0);
        break;
        case 0x35: 
        l = SwapNibles(l); 
        setFlagBit(Flags::Zero, l == 0);
        break;
        case 0x36: 
        memory.write(h << 8 | l, SwapNibles(memory.read(h << 8 | l))); 
        setFlagBit(Flags::Zero, memory.read(h << 8 | l) == 0);
        break;
        case 0x37: 
        a = SwapNibles(a); 
        setFlagBit(Flags::Zero, a == 0);
        break;
    }
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, false);
    setFlagBit(Flags::Carry, false);
}

void SHARP_LR35902::ADD() 
{
    // 16-bit addition.
    if(opcode == 0x09 || opcode == 0x19 || opcode == 0x29 || opcode == 0x39)
    {
        uint32_t sum;
        uint32_t n;
        uint32_t hl = h << 8 | l;
        
        if(opcode == 0x09) n = b << 8 | c;
        else if (opcode == 0x19) n = d << 8 | e;
        else if (opcode == 0x29) n = h << 8 | l;
        else if (opcode == 0x39) n = sp;

        sum = hl + n;
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::HalfCarry, ((hl & 0x00000fff) + (n & 0x00000fff)) > 0x00000fff);
        setFlagBit(Flags::Carry, sum > 0xffff);
        h = (sum & 0xff00) >> 8;
        l = sum & 0x00ff;
        return;
    }
    if(opcode == 0xe8)
    {
        int8_t n = memory.read(pc);
        pc++;
        setFlagBit(Flags::Zero, false);
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::HalfCarry, ((sp & 0x000f) + (n & 0x000f)) > 0xf);
        setFlagBit(Flags::Carry, ((sp & 0x00ff) + ((uint16_t)n & 0x00ff)) > 0xff);
        sp += n;
        return;
    }

    // 8-bit addition.
    uint16_t sum; // Store additions in a 16 bit integer, so carry can be detected.
    uint16_t n;
    switch(opcode)
    {
        case 0x80:
        n = b;
        break;
        case 0x81:
        n = c;
        break;
        case 0x82:
        n = d;
        break;
        case 0x83:
        n = e;
        break;
        case 0x84:
        n = h;
        break;
        case 0x85:
        n = l;
        break;
        case 0x86:
        n = memory.read(h << 8 | l);
        break;
        case 0x87:
        n = a;
        break;
        case 0xc6:
        n = memory.read(pc);
        pc++;
        break;
    }
    sum = (uint16_t)a + n;
    setFlagBit(Flags::Zero, (sum & 0x00ff) == 0);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, ((a & 0xf) + (n & 0xf)) > 0xf); // If the addition of the lower 4 bits of the a and n register result in a carry, half carry is set.
    setFlagBit(Flags::Carry, sum > 0xff);
    a = sum & 0x00ff;
}

void SHARP_LR35902::ADC() 
{
    uint16_t sum;
    uint16_t nn;
    switch(opcode)
    {
        case 0x88:
        nn = b;
        break;
        case 0x89:
        nn = c;
        break;
        case 0x8a:
        nn = d;
        break;
        case 0x8b:
        nn = e;
        break;
        case 0x8c:
        nn = h;
        break;
        case 0x8d:
        nn = l;
        break;
        case 0x8e:
        nn = memory.read(h << 8 | l);
        break;
        case 0x8f:
        nn = a;
        break;
        case 0xce:
        nn = memory.read(pc);
        pc++;
        break;
    }
    sum = (uint16_t)a + nn + (uint16_t)getFlagBit(Flags::Carry);
    setFlagBit(Flags::Zero, (sum & 0x00ff) == 0);
    setFlagBit(Flags::Subtraction, false);
    setFlagBit(Flags::HalfCarry, ((a & 0x0f) + (nn & 0x0f) + getFlagBit(Flags::Carry)) > 0xf);
    setFlagBit(Flags::Carry, sum > 0xff);
    a = sum & 0x00ff;
}

void SHARP_LR35902::SUB() 
{
    uint16_t difference;
    uint16_t subtrahend_reg;

    switch(opcode)
    {
        case 0x90:
            subtrahend_reg = b;
        break;
        case 0x91:
            subtrahend_reg = c;
        break;
        case 0x92:
            subtrahend_reg = d;
        break;
        case 0x93:
            subtrahend_reg = e;
        break;
        case 0x94:
            subtrahend_reg = h;
        break;
        case 0x95:
            subtrahend_reg = l;
        break;
        case 0x96:
            subtrahend_reg = memory.read(h << 8 | l);
        break;
        case 0x97:
            subtrahend_reg = a;
        break;
        case 0xd6:
            subtrahend_reg = memory.read(pc);
            pc++;
        break;
    }
    difference = (uint16_t)a - subtrahend_reg;
    setFlagBit(Flags::Zero, (difference & 0x00ff) == 0);
    setFlagBit(Flags::Subtraction, true);
    setFlagBit(Flags::HalfCarry, (a & 0x0f) < (subtrahend_reg & 0x0f));
    setFlagBit(Flags::Carry, a < subtrahend_reg);
    a = difference & 0x00ff;
}

void SHARP_LR35902::SBC() 
{
    uint8_t difference;
    uint8_t n;

    switch(opcode)
    {
        case 0x98:
            n = b;
        break;
        case 0x99:
            n = c;
        break;
        case 0x9a:
            n = d;
        break;
        case 0x9b:
            n = e;
        break;
        case 0x9c:
            n = h;
        break;
        case 0x9d:
            n = l;
        break;
        case 0x9e:
            n = memory.read(h << 8 | l);
        break;
        case 0x9f:
            n = a;
        break;
        case 0xde:
            n = memory.read(pc);
            pc++;
        break;
    }
    difference = a - n - getFlagBit(Flags::Carry);

    setFlagBit(Flags::Zero, difference == 0);
    setFlagBit(Flags::Subtraction, true);
    setFlagBit(Flags::HalfCarry, ((a & 0x0f) - (n & 0x0f) - getFlagBit(Flags::Carry)) < 0);
    setFlagBit(Flags::Carry, a < (n + getFlagBit(Flags::Carry)));

    a = difference;
}

void SHARP_LR35902::INC() 
{
    uint16_t bc;
    uint16_t de;
    uint16_t hl;

    switch(opcode)
    {
        case 0x03:
        bc = b << 8 | c;
        bc++;
        b = bc >> 8;
        c = bc & 0x00ff;
        break;
        case 0x04:
        setFlagBit(Flags::HalfCarry, ((b & 0xf) + 1) > 0xf); // Note: Half Carry has to be set first. Otherwise b will already be changed.
        b++;
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::Zero, b == 0);
        break;
        case 0x0c:
        setFlagBit(Flags::HalfCarry, ((c & 0xf) + 1) > 0xf);
        c++;
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::Zero, c == 0);
        break;
        case 0x13:
        de = d << 8 | e;
        de++;
        d = de >> 8;
        e = de & 0x00ff;
        break;
        case 0x14:
        setFlagBit(Flags::HalfCarry, ((d & 0xf) + 1) > 0xf);
        d++;
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::Zero, d == 0);
        break;
        case 0x1c:
        setFlagBit(Flags::HalfCarry, ((e & 0xf) + 1) > 0xf);
        e++;
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::Zero, e == 0);
        break;
        case 0x23:
        hl = h << 8 | l;
        hl++;
        h = hl >> 8;
        l = hl & 0x00ff;
        break;
        case 0x24:
        setFlagBit(Flags::HalfCarry, ((h & 0xf) + 1) > 0xf);
        h++;
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::Zero, h == 0);
        break;
        case 0x2c:
        setFlagBit(Flags::HalfCarry, ((l & 0xf) + 1) > 0xf);
        l++;
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::Zero, l == 0);
        break;
        case 0x33:
        sp++;
        break;
        case 0x34:
        operand1 = memory.read(h << 8 | l);
        setFlagBit(Flags::HalfCarry, ((operand1 & 0xf) + 1) > 0xf);
        operand1++;
        memory.write(h << 8 | l, operand1);
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::Zero, operand1 == 0);
        break;
        case 0x3c:
        setFlagBit(Flags::HalfCarry, ((a & 0x0f) + 1) > 0xf);
        a++;
        setFlagBit(Flags::Subtraction, false);
        setFlagBit(Flags::Zero, a == 0);
        break;
    }
}

void SHARP_LR35902::DEC() 
{
    uint16_t bc;
    uint16_t de;
    uint16_t hl;

    switch(opcode)
    {
        case 0x05:
        setFlagBit(Flags::HalfCarry, ((b & 0xf) - 1) < 0);
        b--;
        setFlagBit(Flags::Zero, b == 0);
        setFlagBit(Flags::Subtraction, true);
        break;
        case 0x0b:
        bc = b << 8 | c;
        bc--;
        b = bc >> 8;
        c = bc & 0x00ff;
        break;
        case 0x0d:
        setFlagBit(Flags::HalfCarry, ((c & 0xf) - 1) < 0);
        c--;
        setFlagBit(Flags::Zero, c == 0);
        setFlagBit(Flags::Subtraction, true);
        break;
        case 0x15:
        setFlagBit(Flags::HalfCarry, ((d & 0xf) - 1) < 0);
        d--;
        setFlagBit(Flags::Zero, d == 0);
        setFlagBit(Flags::Subtraction, true);
        break;
        case 0x1b:
        de = d << 8 | e;
        de--;
        d = de >> 8;
        e = de & 0x00ff;
        break;
        case 0x1d:
        setFlagBit(Flags::HalfCarry, ((e & 0xf) - 1) < 0);
        e--;
        setFlagBit(Flags::Zero, e == 0);
        setFlagBit(Flags::Subtraction, true);
        break;
        case 0x25:
        setFlagBit(Flags::HalfCarry, ((h & 0xf) - 1) < 0);
        h--;
        setFlagBit(Flags::Zero, h == 0);
        setFlagBit(Flags::Subtraction, true);
        break;
        case 0x2b:
        hl = h << 8 | l;
        hl--;
        h = hl >> 8;
        l = hl & 0x00ff;
        break;
        case 0x2d:
        setFlagBit(Flags::HalfCarry, ((l & 0xf) - 1) < 0);
        l--;
        setFlagBit(Flags::Zero, l == 0);
        setFlagBit(Flags::Subtraction, true);
        break;
        case 0x35:
        operand1 = memory.read(h << 8 | l);
        setFlagBit(Flags::HalfCarry, ((operand1 & 0xf) - 1) < 0);
        operand1--;
        memory.write(h << 8 | l, operand1);
        setFlagBit(Flags::Zero, operand1 == 0);
        setFlagBit(Flags::Subtraction, true);
        break;
        case 0x3b:
        sp--;
        break;
        case 0x3d:
        setFlagBit(Flags::HalfCarry, ((a & 0xf) - 1) < 0);
        a--;
        setFlagBit(Flags::Zero, a == 0);
        setFlagBit(Flags::Subtraction, true);
        break;
    }
}

void SHARP_LR35902::JR() 
{
    int8_t relative = memory.read(pc);
    pc++;
    switch(opcode)
    {
        case 0x18:
        pc += relative;
        break;
        case 0x20:
        if(!getFlagBit(Flags::Zero))
        {
            pc += relative;
        }
        break;
        case 0x28:
        if(getFlagBit(Flags::Zero))
        {
            pc += relative;
        }
        break;
        case 0x30:
        if(!getFlagBit(Flags::Carry))
        {
            pc += relative;
        }
        break;
        case 0x38:
        if(getFlagBit(Flags::Carry))
        {
            pc += relative;
        }
        break;
    }
}

void SHARP_LR35902::JP() 
{
    if(opcode == 0xe9)
    {
        pc = h << 8 | l;
        return;
    }

    operand1 = memory.read(pc);
    pc++;
    operand2 = memory.read(pc);
    pc++;
    switch(opcode)
    {
        case 0xc2:
        if(!getFlagBit(Flags::Zero))
        {
            pc = operand2 << 8 | operand1; 
        }
        break;
        case 0xc3:
        pc = operand2 << 8 | operand1;
        break;
        case 0xd2:
        if(!getFlagBit(Flags::Carry))
        {
            pc = operand2 << 8 | operand1;  
        }
        break;
        case 0xca:
        if(getFlagBit(Flags::Zero))
        {
            pc = operand2 << 8 | operand1;
        }
        break;
        case 0xda:
        if(getFlagBit(Flags::Carry))
        {
            pc = operand2 << 8 | operand1;  
        }
        break;
    }
}

void SHARP_LR35902::RET() 
{
    switch(opcode)
    {
        case 0xc0:
        if(!getFlagBit(Flags::Zero))
        {
            operand1 = memory.read(sp);
            sp++;
            operand2 = memory.read(sp);
            sp++;
            pc = operand2 << 8 | operand1;
        }
        break;
        case 0xc8:
        if(getFlagBit(Flags::Zero))
        {
            operand1 = memory.read(sp);
            sp++;
            operand2 = memory.read(sp);
            sp++;
            pc = operand2 << 8 | operand1;
        }
        break;
        case 0xc9:
        operand1 = memory.read(sp);
        sp++;
        operand2 = memory.read(sp);
        sp++;
        pc = operand2 << 8 | operand1;        
        break;
        case 0xd0:
        if(!getFlagBit(Flags::Carry))
        {
            operand1 = memory.read(sp);
            sp++;
            operand2 = memory.read(sp);
            sp++;
            pc = operand2 << 8 | operand1;
        }
        break;
        case 0xd8:
        if(getFlagBit(Flags::Carry))
        {
            operand1 = memory.read(sp);
            sp++;
            operand2 = memory.read(sp);
            sp++;
            pc = operand2 << 8 | operand1;
        }
        break;
        case 0xd9:
        operand1 = memory.read(sp);
        sp++;
        operand2 = memory.read(sp);
        sp++;
        pc = operand2 << 8 | operand1;
        EI();
        break;
    }
}

void SHARP_LR35902::CALL() 
{
    // read address to jump to. Even if no jump occurs, the program counter still needs to be increased.
    operand1 = memory.read(pc);
    pc++;
    operand2 = memory.read(pc);
    pc++;

    switch(opcode)
    {
        case 0xc4:
        if(getFlagBit(Flags::Zero)) return;
        break; // If we do not jump, we still have to jump out of the switch statement.
        case 0xd4:
        if(getFlagBit(Flags::Carry)) return;
        break;
        case 0xcc:
        if(!getFlagBit(Flags::Zero)) return;
        break;
        case 0xdc:
        if(!getFlagBit(Flags::Carry)) return;
        break;
    }

    // Push address of next instruction onto stack.
    sp--;
    memory.write(sp, pc >> 8);
    sp--;
    memory.write(sp, pc & 0x00ff);

    // Jump to nn.
    pc = operand2 << 8 | operand1;
}

void SHARP_LR35902::XXX() 
{
    std::cout << "Warning: Read an unimplemented opcode: " << (int)opcode << " at " << (int)pc << "!\n";
}