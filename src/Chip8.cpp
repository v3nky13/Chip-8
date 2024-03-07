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

void Chip8::emulate_inst() {
    // Fetch, decode and execute a chip-8 instruction

    // TODO:
    // DXYN - draw on screen
    // FX0A - wait for key input
    // chip8 variant changes for the following (read guide & queso):
    // 8XY1, 8XY2, 8XY3, 8XY6, 8XYE, BNNN, FX55, FX65

    // fetch
    u16 opcode = (read(PC) << 8) + read(PC + 1);
    PC += 2;
    printf("[%04X]:\n", opcode); // to be removed

    // decode
    inst.kind = opcode >> 12;
    inst.NNN = opcode & 0x0FFF;
    inst.NN = opcode & 0x00FF;
    inst.N = opcode & 0x000F;
    inst.X = (opcode & 0x0F00) >> 8;
    inst.Y = (opcode & 0x00F0) >> 4;

    // execute
    switch (inst.kind) { // switch b/w diff instruction kinds
        case 0x0:
            switch (inst.NNN) {
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
            PC = inst.NNN;
            break;

        // 2NNN - CALL addr
        case 0x2:
            push(PC);
            PC = inst.NNN;
            break;

        // 3XNN - SE Vx, byte
        case 0x3:
            if (V[inst.X] == inst.NN)
                PC += 2;
        
        // 4XNN - SNE Vx, byte
        case 0x4:
            if (V[inst.X] != inst.NN)
                PC += 2;
        
        // 5XY0 - SE Vx, Vy
        case 0x5:
            if (inst.N != 0x0)
                return;
            if (V[inst.X] == V[inst.Y])
                PC += 2;

        // 6XNN - LD Vx, byte
        case 0x6:
            V[inst.X] = inst.NN;
            break;
        
        // 7XNN - ADD Vx, byte
        case 0x7:
            V[inst.X] += inst.NN;
            break;

        case 0x8:
            switch (inst.N) {
                // 8XY0 - LD Vx, Vy
                case 0x0:
                    V[inst.X] = V[inst.Y];
                    break;

                // 8XY1 - OR Vx, Vy
                case 0x1:
                    V[inst.X] |= V[inst.Y];
                    break;

                // 8XY2 - AND Vx, Vy
                case 0x2:
                    V[inst.X] &= V[inst.Y];
                    break;

                // 8XY3 - XOR Vx, Vy
                case 0x3:
                    V[inst.X] ^= V[inst.Y];
                    break;

                // 8XY4 - ADD Vx, Vy
                case 0x4:
                    V[0xF] = V[inst.X] + V[inst.Y] > 255;
                    V[inst.X] += V[inst.Y];
                    break;

                // 8XY5 - SUB Vx, Vy
                case 0x5:
                    V[inst.X] = abs(V[inst.X] - V[inst.Y]);
                    V[0xF] = V[inst.X] >= V[inst.Y];
                    break;

                // 8XY6 - SHR Vx {, Vy}
                case 0x6:
                    V[0xF] = V[inst.X] & 0x01;
                    V[inst.X] >>= 1;
                    break;

                // 8XY7 - SUBN Vx, Vy
                case 0x7:
                    V[inst.X] = abs(V[inst.Y] - V[inst.X]);
                    V[0xF] = V[inst.Y] >= V[inst.X];
                    break;

                // 8XYE - SHL Vx {, Vy}  
                case 0xE:
                    V[0xF] = (V[inst.X] & 0x80) >> 7;
                    V[inst.X] <<= 1;
                    break;
            }
            break;

            // 9XY0 - SNE Vx, Vy
            case 0x9:
                if (inst.N != 0x0)
                    return;
                if (V[inst.X] != V[inst.Y])
                    PC += 2;
                break;

            // ANNN - LD I, addr
            case 0xA:
                I = inst.NNN;
                break;
            
            // BNNN - JP V0, addr
            case 0xB:
                PC = V[0x0] + inst.NNN;
                break;

            // CXNN - RND Vx, byte
            case 0xC:
                V[inst.X] = (rand() % 256) & inst.NN;
                break;

            // DXYN - DRW Vx, Vy, nibble
            case 0xD:

                break;
            
            case 0xE:
                switch (inst.NN) {
                    // EX9E - SKP Vx
                    case 0x9E:
                        if (keypad[V[inst.X]])
                            PC += 2;
                        break;

                    // EXA1 - SKNP Vx
                    case 0xA1:
                        if (!keypad[V[inst.X]])
                            PC += 2;
                        break;
                }
                break;

            case 0xF:
                switch (inst.NN) {
                    // FX07 - LD Vx, DT
                    case 0x07:
                        V[inst.X] = delay_timer;
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
                        delay_timer = V[inst.X];
                        break;
                    
                    // FX18 - LD ST, Vx
                    case 0x18:
                        sound_timer = V[inst.X];
                        break;

                    // FX1E - LD I, Vx
                    case 0x1E:
                        V[0xF] = I + V[inst.X] > 0xFFF; // replicating amiga interpreter (for Spaceflight 2091!)
                        I = (I + V[inst.X]) & 0x0FFF;
                        break;
                    
                    // FX29 - LD F, Vx
                    case 0x29:
                        I = V[inst.X] * 5;
                        break;
                    
                    // FX33 - LD B, Vx
                    case 0x33:
                        ram[I] = V[inst.X] / 100;
                        ram[I + 1] = (V[inst.X] % 100) / 10;
                        ram[I + 2] = V[inst.X] % 10;
                        break;
                    
                    // FX55 - LD [I], Vx
                    case 0x55:
                        for (int i = 0; i <= inst.X; i++)
                            ram[I + i] = V[i];
                        I += inst.X + 1;
                        break;
                    
                    // FX65 - LD Vx, [I]
                    case 0x65:
                        for (int i = 0; i <= inst.X; i++)
                            V[i] = ram[I + i];
                        I += inst.X + 1;
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