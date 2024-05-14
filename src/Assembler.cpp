#include "../include/Assembler.h"

void Assembler::get_line(FILE *source) {

}

bool Assembler::assemble(const char *file_path, const Address starting_addr) {
    FILE *source = fopen(file_path, "r");
    FILE *interm = fopen("interm.txt", "w+");
    FILE *output = fopen("out.ch8", "wb");

    Address LOCCTR = starting_addr;


    if (source) fclose(source);
    if (interm) { fclose(interm); remove("interm.txt"); }
    if (output) fclose(output);

    return true;    // Success
}