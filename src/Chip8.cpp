#include <iostream>
#include <cstring>
#include <time.h>
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
    memset(this, 0, sizeof(Chip8));

    // load font sprites to ram at address 0x000
    memcpy(ram, font_sprites, sizeof(font_sprites));

    // set chip8 machine defaults
    PC = 0x200;     // Start PC at ROM entry point which is at 0x200
    SP = 63;        // Empty stack

    srand(time(NULL));
}

void Chip8::write(Address addr, u8 data) {
    if (addr < 0x000 || addr > 0xFFF)
        return;
    ram[addr] = data;
}

u8 Chip8::read(Address addr) {
    if (addr < 0x000 || addr > 0xFFF)
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

void Chip8::fetch() {
    inst.opcode = (read(PC) << 8) + read(PC + 1);
    inst.NNN = inst.opcode & 0x0FFF;
    inst.NN = inst.opcode & 0x00FF;
    inst.N = inst.opcode & 0x000F;
    inst.X = (inst.opcode & 0x0F00) >> 8;
    inst.Y = (inst.opcode & 0x00F0) >> 4;
    PC += 2;
    printf("[%04X]:\n", inst.opcode);
}

void Chip8::print_regs() {
    printf("PC: %03X SP: %03X I: %03X\n", PC, SP, I);
    for (int i = 0; i < 16; i++)
        printf("V%X: %02X ", i, V[i]);
    printf("\n");
}

void Chip8::execute() {
    // This function both decodes and well as executes current inst

    // TODO:
    // DXYN - draw on screen
    // FX0A - wait for key input

    // switch b/w diff opcode categories
    switch (inst.opcode >> 12) {
        case 0x0:
            switch (inst.NNN) {
                // 00E0
                case 0x0E0:
                    memset(display, false, sizeof(display));
                    draw = true;
                    break;
                
                // 00EE
                case 0x0EE:
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

        // 3XNN
        case 0x3:
            if (V[inst.X] == inst.NN)
                PC += 2;
        
        // 4XNN
        case 0x4:
            if (V[inst.X] != inst.NN)
                PC += 2;
        
        // 5XY0
        case 0x5:
            if (inst.N != 0x0)
                return;
            if (V[inst.X] == V[inst.Y])
                PC += 2;

        // 6XNN
        case 0x6:
            V[inst.X] = inst.NN;
            break;
        
        // 7XNN
        case 0x7:
            V[inst.X] += inst.NN;
            break;

        case 0x8:
            switch (inst.N) {
                // 8XY0
                case 0x0:
                    V[inst.X] = V[inst.Y];
                    break;

                // 8XY1
                case 0x1:
                    V[inst.X] |= V[inst.Y];
                    break;

                // 8XY2
                case 0x2:
                    V[inst.X] &= V[inst.Y];
                    break;

                // 8XY3
                case 0x3:
                    V[inst.X] ^= V[inst.Y];
                    break;

                // 8XY4
                case 0x4:
                    V[0xF] = V[inst.X] + V[inst.Y] > 255;
                    V[inst.X] += V[inst.Y];
                    break;

                // 8XY5
                case 0x5:
                    V[inst.X] = abs(V[inst.X] - V[inst.Y]);
                    V[0xF] = V[inst.X] >= V[inst.Y];
                    break;

                // 8XY6
                case 0x6:
                    V[0xF] = V[inst.X] & 0x1;
                    V[inst.X] >>= 1;
                    break;

                // 8XY7
                case 0x7:
                    V[inst.X] = abs(V[inst.Y] - V[inst.X]);
                    V[0xF] = V[inst.Y] >= V[inst.X];
                    break;

                // 8XYE                
                case 0xE:
                    V[0xF] = (V[inst.X] & 0x80) >> 7;
                    V[inst.X] <<= 1;
                    break;
            }
            break;

            // 9XY0
            case 0x9:
                if (inst.N != 0x0)
                    return;
                if (V[inst.X] != V[inst.Y])
                    PC += 2;
                break;

            // ANNN
            case 0xA:
                I = inst.NNN;
                break;
            
            // BNNN
            case 0xB:
                PC = V[0x0] + inst.NNN;
                break;

            // CXNN
            case 0xC:
                V[inst.X] = (rand() % 256) & inst.NN;
                break;

            // DXYN
            case 0xD:

                break;
            
            case 0xE:
                switch (inst.NN) {
                    // EX9E
                    case 0x9E:
                        if (keypad[V[inst.X]])
                            PC += 2;
                        break;

                    // EXA1
                    case 0xA1:
                        if (!keypad[V[inst.X]])
                            PC += 2;
                        break;
                }
                break;

            case 0xF:
                switch (inst.NN) {
                    // FX07
                    case 0x07:
                        V[inst.X] = delay_timer;
                        break;
                    
                    // FX0A
                    case 0x0A:

                        // --- A brain rotten idea for this part ---

                        // func (v *VM) insLDxK(reg uint8) {
                        //     if len(v.Keys) > 0 {
                        //         // Get last key pressed if there are multiple and exit the PC loop
                        //         v.registers[reg] = v.Keys[0]
                        //         return
                        //     }
                        //     // Madness, *decrement* the PC to keep the fetch loop waiting here
                        //     v.pc -= 2
                        // }

                        break;

                    // FX15
                    case 0x15:
                        delay_timer = V[inst.X];
                        break;
                    
                    // FX18
                    case 0x18:
                        sound_timer = V[inst.X];
                        break;

                    // FX1E
                    case 0x1E:
                        I += V[inst.X];
                        break;
                    
                    // FX29
                    case 0x29:
                        I = V[inst.X] * 5;
                        break;
                    
                    // FX33
                    case 0x33:
                        ram[I] = V[inst.X] / 100;
                        ram[I + 1] = V[inst.X] % 100 / 10;
                        ram[I + 2] = V[inst.X] % 100;
                        break;
                    
                    // FX55
                    case 0x55:
                        for (int i = 0; i <= inst.X; i++)
                            ram[I + i] = V[i];
                        I += inst.X + 1;
                        break;
                    
                    // FX65
                    case 0x65:
                        for (int i = 0; i <= inst.X; i++)
                            V[i] = ram[I + i];
                        I += inst.X + 1;
                        break;
                }
                break;
    }

    print_regs();
    printf("\n");
}
