#include <iostream>
#include "../include/Chip8.h"
#include "../include/Emulator.h"

int main() {
    Chip8 chip8;

    // chip8 insts are stored big-endian
    chip8.write(0x200, 0x63);
    chip8.write(0x201, 0xF0);
    chip8.write(0x202, 0x64);
    chip8.write(0x203, 0xE0);
    chip8.write(0x204, 0x83);
    chip8.write(0x205, 0x44);

    chip8.emulate_inst();
    chip8.emulate_inst();
    chip8.emulate_inst();

    return 0;
}