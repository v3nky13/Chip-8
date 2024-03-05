#include <iostream>
#include "../include/Chip8.h"
#include "../include/Emulator.h"

int main() {
    Chip8 chip8;

    u16 sample_insts[] = {0x63F0, 0x64E0, 0x8344};

    for (auto inst: sample_insts) {
        printf("%04X:\n", inst);

        Instruction curr_inst = {
            (u8) ((inst & 0xF000) >> 12),
            (u8) ((inst & 0x0FFF) >>  0),
            (u8) ((inst & 0x00FF) >>  0),
            (u8) ((inst & 0x000F) >>  0),
            (u8) ((inst & 0x0F00) >>  8),
            (u8) ((inst & 0x00F0) >>  4)
        };

        chip8.execute_inst(curr_inst);
    }

    return 0;
}