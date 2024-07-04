#include "../include/Assembler.h"

bool Assembler::get_line(FILE *source) {
    line[0] = label[0] = opcode[0] = operand[0] = '\0';

    fgets(line, MAX_LINE_SIZE, source); // get a line from source file

    if (line[0] == '\0') return false;  // end of input
    
    if (line[0] == ' ' || line[0] == '\t') 
        // if there is no label get only opcode and operands
        sscanf(line, "%s %s\n", opcode, operand);
    else
        // if there is label, get all of them
        sscanf(line, "%s %s %s\n", label, opcode, operand);
    
    label[strlen(label)-1] = '\0';  // remove ":" at the end of label

    return true;
}

// checks if a is of the for v0 to vf
bool is_vx(char *a) {
    return a[0] == 'v' && ((a[1] >= '0' && a[1] <= '9') || (a[1] >= 'a' && a[1] <= 'f'));
}

bool is_nibble(char *a) {
    return (a[0] >= '0' && a[0] <= '9') || (a[0] >= 'a' && a[0] <= 'f');
}

bool is_byte(char *a) {
    return ((a[0] >= '0' && a[0] <= '9') || (a[0] >= 'a' && a[0] <= 'f')) &&
           ((a[1] >= '0' && a[1] <= '9') || (a[1] >= 'a' && a[1] <= 'f'));
}

u8 hextodec(char *a) {
    if (a[0] >= '0' && a[0] <= '9')
        return a[0] - '0';
    return a[0] - 'a' + 10;
}

bool Assembler::assemble(const char *file_path, const Address starting_addr) {
    FILE *source = fopen(file_path, "r");
    FILE *output = fopen("build/out.ch8", "wb");

    Address LOCCTR = starting_addr;
    u32 line_no = 1;
    
    // pass 1
    while (get_line(source)) {
        if (label[0] != '\0') {
            if (symtab.find(label) != symtab.end()) {
                SDL_Log("line %d: Duplicate label used: %s\n", line_no, label);
                return false;
            }
            symtab[label] = LOCCTR;
        }
        LOCCTR += 2;
        line_no++;
    }

    rewind(source);
    line_no = 1;

    // pass 2
    while (get_line(source)) {
        u16 inst;

        if (!strcmp(opcode, "cls")) {
            inst = 0x00E0;
        } else if (!strcmp(opcode, "ret")) {
            inst = 0x00EE;
        } else if (!strcmp(opcode, "jp")) {
            auto it = symtab.find(operand);
            if (it != symtab.end()) {
                inst = 0x1000 + it->second;
            } else {
                SDL_Log("line %d: Undefined label used: %s\n", line_no, operand);
                return false;
            }
        } else if (!strcmp(opcode, "jpr")) {
            if (strlen(operand) > 3 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
            auto it = symtab.find(operand + 3);
            if (it != symtab.end()) {
                inst = 0x1000 + it->second;
            } else {
                SDL_Log("line %d: Undefined label used: %s\n", line_no, operand);
                return false;
            }
        } else if (!strcmp(opcode, "call")) {
            auto it = symtab.find(operand);
            if (it != symtab.end()) {
                inst = 0x2000 + it->second;
            } else {
                SDL_Log("line %d: Undefined label used: %s\n", line_no, operand);
                return false;
            }
        } else if (!strcmp(opcode, "se")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x5000 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else if (is_byte(operand + 3)) {
                inst = 0x3000 + (hextodec(operand + 1) << 8) + (hextodec(operand + 3) << 4) + hextodec(operand + 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "sne")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x9000 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else if (is_byte(operand + 3)) {
                inst = 0x4000 + (hextodec(operand + 1) << 8) + (hextodec(operand + 3) << 4) + hextodec(operand + 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "add")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x8004 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else if (is_byte(operand + 3)) {
                inst = 0x7000 + (hextodec(operand + 1) << 8) + (hextodec(operand + 3) << 4) + hextodec(operand + 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "addi")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xF01E + (hextodec(operand + 1) << 8);
        } else if (!strcmp(opcode, "sub")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x8005 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "subn")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x8007 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "or")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x8001 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "and")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x8002 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "xor")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x8003 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "shr")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x8006 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "shl")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x800E + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "rnd")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_byte(operand + 3)) {
                inst = 0xC000 + (hextodec(operand + 1) << 8) + (hextodec(operand + 3) << 4) + hextodec(operand + 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "skp")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xE09E + (hextodec(operand + 1) << 8);
        } else if (!strcmp(opcode, "sknp")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xE0A1 + (hextodec(operand + 1) << 8);
        } else if (!strcmp(opcode, "drw")) {
            if (strlen(operand) != 7 || !is_vx(operand) || operand[2] != ',' ||
                    !is_vx(operand + 3) || operand[5] != ',' || !is_nibble(operand + 6)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xD000 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4) + hextodec(operand + 6);
        } else if (!strcmp(opcode, "ld")) {
            if (strlen(operand) != 5 || !is_vx(operand) || operand[2] != ',') {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            if (is_vx(operand + 3)) {
                inst = 0x8000 + (hextodec(operand + 1) << 8) + (hextodec(operand + 4) << 4);
            } else if (is_byte(operand + 3)) {
                inst = 0x6000 + (hextodec(operand + 1) << 8) + (hextodec(operand + 3) << 4) + hextodec(operand + 4);
            } else {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }
        } else if (!strcmp(opcode, "ldi")) {
            auto it = symtab.find(operand);
            if (it != symtab.end()) {
                inst = 0xA000 + it->second;
            } else {
                SDL_Log("line %d: Undefined label used: %s\n", line_no, operand);
                return false;
            }
        } else if (!strcmp(opcode, "ldd")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xF015 + (hextodec(operand + 1) << 8);
        } else if (!strcmp(opcode, "lds")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xF018 + (hextodec(operand + 1) << 8);
        } else if (!strcmp(opcode, "std")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xF007 + (hextodec(operand + 1) << 8);
        } else if (!strcmp(opcode, "wait")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xF00A + (hextodec(operand + 1) << 8);
        } else if (!strcmp(opcode, "sprite")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xF029 + (hextodec(operand + 1) << 8);
        } else if (!strcmp(opcode, "bcd")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xF033 + (hextodec(operand + 1) << 8);
        } else if (!strcmp(opcode, "read")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xF065 + (hextodec(operand + 1) << 8);
        } else if (!strcmp(opcode, "write")) {
            if (strlen(operand) != 2 || !is_vx(operand)) {
                SDL_Log("line %d: Invalid operand(s)\n", line_no);
                return false;
            }

            inst = 0xF055 + (hextodec(operand + 1) << 8);
        } else {
            SDL_Log("line %d: Invalid opcode(s)\n", line_no);
            return false;
        }

        u16 rev = ((inst & 0x00FF) << 8) + ((inst & 0xFF00) >> 8);
        fwrite(&rev, 1, 2, output);
        line_no++;
    }

    if (source) fclose(source);
    if (output) fclose(output);

    return true;    // Success
}