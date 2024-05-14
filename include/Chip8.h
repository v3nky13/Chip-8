#ifndef CHIP8_H
#define CHIP8_H

#include <SDL2/SDL.h>
#include "Emulator.h"
#include "types.h"

class Chip8 {
public:
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

    // Currently running ROM/Program
    const char *rom_name;

    // currently executing instruction
    struct Instruction {
        u16 opcode;     // 16-bit instruction
        u8 category;    //  4-bit instruction category
        u16 NNN;        // 12-bit address/constant
        u8 NN;          //  8-bit constant
        u8 N;           //  4-bit constant
        u8 X;           //  4-bit register identifier
        u8 Y;           //  4-bit register identifier
    } inst;

    // stack operations
    void push(u16 data);
    u16 pop();

    // log debugging info abt currently executing inst to window
    void debug_inst();

    // Initialize CHIP8 machine
    bool init_chip8(const config_t *config, const char *rom_name);

    // Whether screen be updated? (yes/no)
    bool draw;

    // fetch, decode and execute a chip-8 instruction
    void emulate_inst(const config_t &config);
};

#endif // CHIP8_H