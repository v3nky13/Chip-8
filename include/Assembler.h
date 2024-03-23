#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <fstream>
#include "../include/Chip8.h"

class Assembler {
    bool get_line(FILE *source);
    void assemble_load(FILE *source, Address *ram_load_addr);
};

#endif