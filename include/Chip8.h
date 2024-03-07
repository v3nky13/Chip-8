#ifndef CHIP_H
#define CHIP_H

#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint16_t Address;

struct Instruction {
    u16 opcode; // 16-bit opcode
    u16 NNN;    // 12-bit address/constant
    u8 NN;      //  8-bit constant
    u8 N;       //  4-bit constant
    u8 X;       //  4-bit register identifier
    u8 Y;       //  4-bit register identifier
};

class Chip8 {
private:
    // registers
    Address PC;
    Address SP;
    Address I;
    u8 V[16];

    // timers
    u8 sound_timer;
    u8 delay_timer;

    // memory
    u8 ram[4096];
    u8 stack[64];

    // hexadecimal input keypad
    u8 keypad[16];

    // display
    bool display[64 * 32];
    u32 pixel_color[64 * 32];

    // currently running rom
    const char* rom_name;

    // currently executing instruction
    Instruction inst;

    // Whether screen be updated? (yes/no)
    bool draw;

public:
    Chip8();
    void write(Address addr, u8 data);
    u8 read(Address addr);
    void push(u8 data);
    u8 pop();
    void print_regs();
    void fetch();
    void execute();
};

#endif