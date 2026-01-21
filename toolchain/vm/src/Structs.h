#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH 128

typedef struct {
    char command[10];
    char segment[10];
    char data[MAX_LENGTH];
} Operation;

Operation* create_operation(const char* command, const char* segment, const char* data);

// Command descriptor
typedef void (*Emitter)(FILE* out, const char* arg1, int arg2);

typedef struct {
    const char* mnemonic;
    Emitter emit;
} Command;

// Use
// Operation *op = find_vm_operation(cmd);
// if (!op) error("unknown VM command");
// operation->emit(out, arg1, arg2);

// // Segment Map
// typedef struct {
//     const char *name;  // VM
//     const char *base;  // ASM
//     bool is_pointer;   // LCL/ARG/THIS/THAT vs constant/temp/static
// } Segment;
//
// static const Segment segments[] = {
//     {"local"   , "LCL" , true},   // local i    (RAM[RAM[1]+i])
//     {"argument", "ARG" , true},   // argument i (RAM[RAM[2]+i])
//     {"this"    , "THIS", true},   // pointer 0  (RAM[RAM[3]])
//     {"that"    , "THAT", true},   // pointer 1  (RAM[RAM[4]])
//     {"temp"    , "5"   , false},  // temp i     (RAM[5+i], 0 <= i <= 7, RAM[5..12])
//     {"constant", NULL  , false},  // constant # (SP, RAM[256..2047])
//     {"static"  , NULL  , false},  // static xxx (16, RAM[16..255]) for variables
//     {NULL, NULL, false}
// };

#endif //STRUCTS_H

// Use
// Operation *op = find_vm_operation(cmd);
// if (!op) error("unknown VM command");
// operation->emit(out, arg1, arg2);

// push constant #
//    @#     # in reg A
//    D=A    # in reg D
//    @SP    SP ptr addr in reg A
//    A=M    SP addr in reg A
//    M=D    # in RAM[SP]
//    @SP    SP ptr addr in reg A
//    M=M+1  SP ptr addr ++

// pop
//    @SP
//    AM=M-1 SP ptr addr --
//    D=M    # in reg D

// add
//    @SP
//    AM=M-1 SP ptr addr --
//    D=M    # in reg D
//    @SP
//    AM=M-1
//    M=D+M
//    @SP
//    M=M+1