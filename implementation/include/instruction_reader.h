#ifndef INSTRUCTION_READER_H
#define INSTRUCTION_READER_H

#include "class_file.h"
#include "code_attribute.h"

void read_instructions(
    ClassFile* cf,
    Code_attribute* code
);

#endif