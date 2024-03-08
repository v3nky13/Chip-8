#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint16_t Address;

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
    u16 stack[16];

    // hexadecimal input keypad
    u8 keypad[16];

    // display
    bool display[64 * 32];
    u32 pixel_color[64 * 32];

    // Whether screen be updated? (yes/no)
    bool draw;

    // stack operations
    void push(u16 data);
    u16 pop();

public:
    Chip8();

    // memory operations
    void write(Address addr, u8 data);
    u8 read(Address addr);

    void emulate_inst();

    void print_regs();
};

#endif