#pragma once

#include "common.h"

// ============================================================
// Real Machine CPU registers
// ============================================================

typedef struct {
    int IC;     // Instruction Counter
    int R;      // General purpose register
    int SP;     // Stack Pointer
    int SF;     // Status Flags (Zero, Negative, Overflow bits)
    int MODE;   // Mode: USER or SUPERVISOR
    int PI;     // Program Interrupt
    int SI;     // System Interrupt
    int TI;     // Timer Interrupt
    int PTR;    // Page Table Register
} CPU;

// ============================================================
// Virtual Machine registers
// ============================================================

typedef struct {
    int RAX;    // General-purpose register A
    int RBX;    // General-purpose register B
    int PC;     // Program Counter
    int SF;     // Status Flags
    int DS;     // Data Segment
    int CS;     // Code Segment
    int SS;     // Stack Segment
    int PTR;    // Page Table Register (mapped to RM)
} VM_CPU;

