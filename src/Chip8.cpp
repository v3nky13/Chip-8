#include <iostream>
#include <cstring>
#include <time.h>
#include "../include/Chip8.h"

Chip8::Chip8(const char *rom_loc) {
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

    const u16 entry_point = 0x200;

    FILE *rom = fopen(rom_loc, "rb");

    if (!rom) {
        printf("Rom file at %s is invalid or doesn't exist\n", rom_loc);
    }

    fseek(rom, 0, SEEK_END);
    const size_t rom_size = ftell(rom);
    rewind(rom);
    
    if (fread(&ram[entry_point], rom_size, 1, rom) != 1)
        printf("Couldn't load rom file into memory");

    if (rom) fclose(rom);

    // set chip8 machine defaults
    PC = entry_point;   // Start PC at ROM entry point
    SP = 15;            // Empty stack

    srand(time(NULL));
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
    // fetch, decode and execute a chip-8 instruction

    // TODO:
    // FX0A - wait for key input
    // chip8 variant changes for the following (read guide & queso):
    // 8XY1, 8XY2, 8XY3, 8XY6, 8XYE, BNNN, FX55, FX65

    // fetch
    inst.opcode = (ram[PC] << 8) + ram[PC + 1];
    PC += 2;

    // decode
    inst.category = inst.opcode >> 12;
    inst.NNN = inst.opcode & 0x0FFF;
    inst.NN = inst.opcode & 0x00FF;
    inst.N = inst.opcode & 0x000F;
    inst.X = (inst.opcode & 0x0F00) >> 8;
    inst.Y = (inst.opcode & 0x00F0) >> 4;

    // if (draw) {
    //     draw = false;
    //     std::system("clear");
    //     for (int i = 0; i < 32; i++) {
    //         for (int j = 0; j < 64; j++)
    //             printf("%c ", display[i * 64 + j] ? '*': ' ');
    //         printf("\n");
    //     }
    // }

    #ifdef DEBUG
    debug_inst();
    #endif

    // execute
    switch (inst.category) {
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
            break;
        
        // 4XNN - SNE Vx, byte
        case 0x4:
            if (V[inst.X] != inst.NN)
                PC += 2;
            break;
        
        // 5XY0 - SE Vx, Vy
        case 0x5:
            if (inst.N != 0x0)
                return;
            if (V[inst.X] == V[inst.Y])
                PC += 2;
            break;

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

        // ANNN- LD I, addr
        case 0xA:
            I = inst.NNN;
            break;
        
        // BNNN- JP V0, addr
        case 0xB:
            PC = V[0x0] + inst.NNN;
            break;

        // CXNN - RND Vx, byte
        case 0xC:
            V[inst.X] = (rand() % 256) & inst.NN;
            break;

        // DXYN - DRW Vx, Vy, nibble
        case 0xD: {
            u8 xc = V[inst.X] % 64; // 64 and 32 to be changed to emulator ht and wh
            u8 yc = V[inst.Y] % 32;
            const u8 org_xc = xc;

            V[0xF] = 0;

            for (u8 i = 0; i < inst.N; i++) {
                const u8 sprite_data = ram[I + i];
                xc = org_xc;
                
                for (i8 j = 7; j >= 0; j--) {
                    bool *pixel = &display[yc * 64 + xc];
                    const bool sprite_bit = sprite_data & (1 << j);

                    if (sprite_bit && *pixel)
                        V[0xF] = 1;
                    
                    *pixel ^= sprite_bit;

                    if (++xc >= 64) break;
                }

                if (++yc >= 32) break;
            }

            draw = true;
        }
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
                    printf("this wont work dumbass\n");

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
                    for (u8 i = 0; i <= inst.X; i++)
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

#ifdef DEBUG
void Chip8::debug_inst() {
    bool invalid_opcode = false;
    const char *light_red = "\033[1;31m";
    const char *light_green = "\033[1;32m";
    const char *light_blue = "\033[1;34m";
    const char *light_yellow = "\033[1;33m";
    const char *reset_color = "\033[0m";

    printf("[%s0x%03X%s]: %s%04X%s -> %s", light_yellow, PC - 2, reset_color, light_blue, inst.opcode, reset_color, light_green);

    switch (inst.category) {
        case 0x0:
            switch (inst.NNN) {
                // 00E0 - CLS
                case 0x0E0:
                    printf("cls");
                    break;
                
                // 00EE - RET
                case 0x0EE:
                    printf("ret");
                    break;

                default:
                    invalid_opcode = true;
            }
            break;

        // 1NNN - JP addr
        case 0x1:
            printf("jp %03x", inst.NNN);
            break;

        // 2NNN - CALL addr
        case 0x2:
            printf("call %03x", inst.NNN);
            break;

        // 3XNN - SE Vx, byte
        case 0x3:
            printf("se v%01x, %02x", inst.X, inst.NN);
            break;
        
        // 4XNN - SNE Vx, byte
        case 0x4:
            printf("sne v%01x, %02x", inst.X, inst.NN);
            break;
        
        // 5XY0 - SE Vx, Vy
        case 0x5:
            printf("se v%01x, v%01x", inst.X, inst.Y);
            break;

        // 6XNN - LD Vx, byte
        case 0x6:
            printf("ld v%01x, %02x", inst.X, inst.NN);
            break;
        
        // 7XNN - ADD Vx, byte
        case 0x7:
            printf("add v%01x, %02x", inst.X, inst.NN);
            break;
        
        case 0x8:
            switch (inst.N) {
                // 8XY0 - LD Vx, Vy
                case 0x0:
                    printf("ld v%01x, v%01x", inst.X, inst.Y);
                    break;

                // 8XY1 - OR Vx, Vy
                case 0x1:
                    printf("or v%01x, v%01x", inst.X, inst.Y);
                    break;

                // 8XY2 - AND Vx, Vy
                case 0x2:
                    printf("and v%01x, v%01x", inst.X, inst.Y);
                    break;

                // 8XY3 - XOR Vx, Vy
                case 0x3:
                    printf("xor v%01x, v%01x", inst.X, inst.Y);
                    break;

                // 8XY4 - ADD Vx, Vy
                case 0x4:
                    printf("add v%01x, v%01x", inst.X, inst.Y);
                    break;

                // 8XY5 - SUB Vx, Vy
                case 0x5:
                    printf("sub v%01x, v%01x", inst.X, inst.Y);
                    break;

                // 8XY6 - SHR Vx {, Vy}
                case 0x6:
                    
                    break;

                // 8XY7 - SUBN Vx, Vy
                case 0x7:
                    printf("subn v%01x, v%01x", inst.X, inst.Y);
                    break;

                // 8XYE - SHL Vx {, Vy}
                case 0xE:
                    
                    break;
                
                default:
                    invalid_opcode = true;
            }
            break;

        // 9XY0 - SNE Vx, Vy
        case 0x9:
            printf("sne V%01x, V%01x", inst.X, inst.Y);
            break;

        // ANNN- LD I, addr
        case 0xA:
            printf("ld %03x, %03x", I, inst.NNN);
            break;
        
        // BNNN- JP V0, addr
        case 0xB:
            printf("jp v0, %03x", inst.NNN);
            break;

        // CXNN - RND Vx, byte
        case 0xC:
            printf("rnd %01x, %02x", inst.X, inst.NN);
            break;

        // DXYN - DRW Vx, Vy, nibble
        case 0xD:
            printf("drw V%01x, V%01x, %01x", inst.X, inst.Y, inst.N);
            break;
        
        case 0xE:
            switch (inst.NN) {
                // EX9E - SKP Vx
                case 0x9E:
                    printf("skp v%01x", inst.X );
                    break;

                // EXA1 - SKNP Vx
                case 0xA1:
                    printf("sknp v%01x", inst.X);
                    break;
                
                default:
                    invalid_opcode = true;
            }
            break;

        case 0xF:
            switch (inst.NN) {
                // FX07 - LD Vx, DT
                case 0x07:
                    printf("ld v%01x, %02x", inst.X, delay_timer);
                    break;
                
                // FX0A - LD Vx, K
                case 0x0A:
                    printf("ld v%01x, k", inst.X);
                    break;

                // FX15 - LD DT, Vx
                case 0x15:
                    printf("ld %02x, v%01x", delay_timer, inst.X);
                    break;
                
                // FX18 - LD ST, Vx
                case 0x18:
                    printf("ld %02x, v%01x", sound_timer, inst.X);
                    break;

                // FX1E - LD I, Vx
                case 0x1E:
                    printf("ld %03x, v%01x", I, inst.X);
                    break;
                
                // FX29 - LD F, Vx
                case 0x29:
                    printf("ld f, v%01x", inst.X);
                    break;
                
                // FX33 - LD B, Vx
                case 0x33:
                    printf("ld b, v%01x", inst.X);
                    break;
                
                // FX55 - LD [I], Vx
                case 0x55:
                    printf("ld [%03x], v%01x", I, inst.X);
                    break;
                
                // FX65 - LD Vx, [I]
                case 0x65:
                    printf("ld v%01x, [%03x]", inst.X, I);
                    break;
                
                default:
                    invalid_opcode = true;
            }
            break;
            
        default:
            invalid_opcode = true;
    }

    if (invalid_opcode)
        printf("%sInvalid opcode", light_red);
    printf("%s\n", reset_color);
}
#endif