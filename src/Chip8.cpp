#include <iostream>
#include <cstring>
#include "../include/Chip8.h"

Chip8::Chip8() {
    const u8 font_sprites[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
        0xF0, 0x80, 0xF0, 0x80, 0x80,   // F
    };

    // init entire chip8 machine
    std::memset(this, 0, sizeof(Chip8));

    // load font sprites to ram at address 0x050
    std::memcpy(ram + 0x050, font_sprites, sizeof(font_sprites));

    // set chip8 machine defaults
    PC = 0x200;     // Start PC at ROM entry point which is at 0x200
}

void Chip8::write(Address addr, u8 data) {
    if (addr < 0 || addr > 4095)
        return;
    ram[addr] = data;
}

u8 Chip8::read(Address addr) {
    if (addr < 0 || addr > 4095)
        return 0;
    return ram[addr];
}

void Chip8::push(u8 data) {
    if (SP == 0)
        return;
    stack[--SP] = data;
}

u8 Chip8::pop() {
    if (SP == 63)
        return 0;
    return stack[SP++];
}

void Chip8::execute_inst(Instruction inst) {
    switch (inst.opcode) {
        case 0x0:
            switch (inst.N) {
                // 00E0
                case 0x0:
                    break;  //todo
                
                // 00EE
                case 0xE:
                    PC = pop();
                    break;
            }
            break;

        // 1NNN
        case 0x1:
            PC = inst.NNN;
            
            break;

        // 2NNN
        case 0x2:
            push(PC);
            PC = inst.NNN;
            break;

        case 0x3:
            if (V[inst.X] == inst.NN)
                PC = PC + 2;
            
        case 0x4:
            if (V[inst.X] != inst.NN)
                PC = PC + 2;
            
        case 0x5:
            if (V[inst.X] )

        // 6XNN
        case 0x6:
            V[inst.X] = inst.NN;
            break;
        
        //7XNN
        case 0x7:
            V[inst.X] += inst.NN;
            break;
        //8XY0
        case 0x8:
            V[inst.X] = V[inst.Y]


    }
}
