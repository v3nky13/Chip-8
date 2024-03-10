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
    SP = 15;        // Empty stack

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

void Chip8::push(u16 data) {
    if (SP == 0)
        return;
    stack[--SP] = data;
}

u16 Chip8::pop() {
    if (SP == 15)
        return 0;
    return stack[SP++];
}

void Chip8::emulate_inst() {
    // Fetch, decode and execute a chip-8 instruction

    // TODO:
    // DXYN - draw on screen
    // FX0A - wait for key input
    // chip8 variant changes for the following (read guide & queso):
    // 8XY1, 8XY2, 8XY3, 8XY6, 8XYE, BNNN, FX55, FX65

    // fetch
    const u16 opcode = (read(PC) << 8) + read(PC + 1);
    PC += 2;
    printf("[%04X]:\n", opcode); // to be removed

    // decode
    u8 category = opcode >> 12;     //  4-bit instruction category
    u16 NNN = opcode & 0x0FFF;      // 12-bit address/constant
    u8 NN = opcode & 0x00FF;        //  8-bit constant
    u8 N = opcode & 0x000F;         //  4-bit constant
    u8 X = (opcode & 0x0F00) >> 8;  //  4-bit register identifier
    u8 Y = (opcode & 0x00F0) >> 4;  //  4-bit register identifier

    // execute
    switch (category) {
        case 0x0:
            switch (NNN) {
                // 00E0 - CLS
                case 0x0E0:
                    memset(display, false, sizeof(display));
                    draw = true;
                    break;
                
                // 00EE - RET
                case 0x0EE:
                    PC = pop();
                    break;
            }
            break;

        // 1NNN - JP addr
        case 0x1:
            PC = NNN;
            break;

        // 2NNN - CALL addr
        case 0x2:
            push(PC);
            PC = NNN;
            break;

        // 3XNN - SE Vx, byte
        case 0x3:
            if (V[X] == NN)
                PC += 2;
        
        // 4XNN - SNE Vx, byte
        case 0x4:
            if (V[X] != NN)
                PC += 2;
        
        // 5XY0 - SE Vx, Vy
        case 0x5:
            if (N != 0x0)
                return;
            if (V[X] == V[Y])
                PC += 2;

        // 6XNN - LD Vx, byte
        case 0x6:
            V[X] = NN;
            break;
        
        // 7XNN - ADD Vx, byte
        case 0x7:
            V[X] += NN;
            break;

        case 0x8:
            switch (N) {
                // 8XY0 - LD Vx, Vy
                case 0x0:
                    V[X] = V[Y];
                    break;

                // 8XY1 - OR Vx, Vy
                case 0x1:
                    V[X] |= V[Y];
                    break;

                // 8XY2 - AND Vx, Vy
                case 0x2:
                    V[X] &= V[Y];
                    break;

                // 8XY3 - XOR Vx, Vy
                case 0x3:
                    V[X] ^= V[Y];
                    break;

                // 8XY4 - ADD Vx, Vy
                case 0x4:
                    V[0xF] = V[X] + V[Y] > 255;
                    V[X] += V[Y];
                    break;

                // 8XY5 - SUB Vx, Vy
                case 0x5:
                    V[X] = abs(V[X] - V[Y]);
                    V[0xF] = V[X] >= V[Y];
                    break;

                // 8XY6 - SHR Vx {, Vy}
                case 0x6:
                    V[0xF] = V[X] & 0x01;
                    V[X] >>= 1;
                    break;

                // 8XY7 - SUBN Vx, Vy
                case 0x7:
                    V[X] = abs(V[Y] - V[X]);
                    V[0xF] = V[Y] >= V[X];
                    break;

                // 8XYE - SHL Vx {, Vy}  
                case 0xE:
                    V[0xF] = (V[X] & 0x80) >> 7;
                    V[X] <<= 1;
                    break;
            }
            break;

            // 9XY0 - SNE Vx, Vy
            case 0x9:
                if (N != 0x0)
                    return;
                if (V[X] != V[Y])
                    PC += 2;
                break;

            // ANNN - LD I, addr
            case 0xA:
                I = NNN;
                break;
            
            // BNNN - JP V0, addr
            case 0xB:
                PC = V[0x0] + NNN;
                break;

            // CXNN - RND Vx, byte
            case 0xC:
                V[X] = (rand() % 256) & NN;
                break;

            // DXYN - DRW Vx, Vy, nibble
            case 0xD: {
                u8 xc = V[X] & 63; // 63 and 31 to be changed to emulator ht and wh
                u8 yc = V[Y] & 31;
                V[0xF] = 0;
                for (u8 i = 0; i < V[X]; i++) {
                    const u8 sprite_data = ram[I + i];
                    
                }
            }
                break;
            
            case 0xE:
                switch (NN) {
                    // EX9E - SKP Vx
                    case 0x9E:
                        if (keypad[V[X]])
                            PC += 2;
                        break;

                    // EXA1 - SKNP Vx
                    case 0xA1:
                        if (!keypad[V[X]])
                            PC += 2;
                        break;
                }
                break;

            case 0xF:
                switch (NN) {
                    // FX07 - LD Vx, DT
                    case 0x07:
                        V[X] = delay_timer;
                        break;
                    
                    // FX0A - LD Vx, K
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

                    // FX15 - LD DT, Vx
                    case 0x15:
                        delay_timer = V[X];
                        break;
                    
                    // FX18 - LD ST, Vx
                    case 0x18:
                        sound_timer = V[X];
                        break;

                    // FX1E - LD I, Vx
                    case 0x1E:
                        V[0xF] = I + V[X] > 0xFFF; // replicating amiga interpreter (for Spaceflight 2091!)
                        I = (I + V[X]) & 0x0FFF;
                        break;
                    
                    // FX29 - LD F, Vx
                    case 0x29:
                        I = V[X] * 5;
                        break;
                    
                    // FX33 - LD B, Vx
                    case 0x33:
                        ram[I] = V[X] / 100;
                        ram[I + 1] = (V[X] % 100) / 10;
                        ram[I + 2] = V[X] % 10;
                        break;
                    
                    // FX55 - LD [I], Vx
                    case 0x55:
                        for (u8 i = 0; i <= X; i++)
                            ram[I + i] = V[i];
                        I += X + 1;
                        break;
                    
                    // FX65 - LD Vx, [I]
                    case 0x65:
                        for (int i = 0; i <= X; i++)
                            V[i] = ram[I + i];
                        I += X + 1;
                        break;
                }
                break;
    }
}

void Chip8::print_regs() {
    printf("[PC]: %03X [SP]: %03X [I]: %03X\n", PC, SP, I);
    for (int i = 0; i < 16; i++)
        printf("[V%X]: %02X ", i, V[i]);
    printf("\n\n");
}