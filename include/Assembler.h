#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <unordered_map>
#include "../include/Chip8.h"

class Assembler {
private:
    std::unordered_map<std::string, Address> symtab;
    std::string line, label, opcode, operands;

    void get_line(FILE *source);

public:
    bool assemble(const char *file_path, const Address starting_addr);
};

#endif // ASSEMBLER_H