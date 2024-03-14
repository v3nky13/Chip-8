#include <iostream>
#include "../include/Chip8.h"
#include "../include/Emulator.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./Chip8 <rom-path>");
        exit(EXIT_FAILURE);
    }

    Chip8 chip8(argv[1]);

    while (true) {
        chip8.emulate_inst();
    }

    return 0;
}