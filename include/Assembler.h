#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <unordered_map>
#include "../include/Chip8.h"

#define MAX_LINE_SIZE 100
#define MAX_WORD_SIZE 30

class Assembler {
private:
    std::unordered_map<std::string, Address> symtab;
    char line[MAX_LINE_SIZE], label[MAX_WORD_SIZE], opcode[MAX_WORD_SIZE], operand[MAX_WORD_SIZE];

    bool get_line(FILE *source);

public:
    bool assemble(const char *file_path, const Address starting_addr);
};

#endif // ASSEMBLER_H