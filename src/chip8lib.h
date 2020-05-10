// CHIP-8 Emulation Library
// Written by Michael Kersting, Jr.
// May 5, 2020
#include <stdint.h>


// Emulator struct; holds pointers to memory and registers
typedef struct {
    uint8_t *RAM;
    uint8_t *V;
    uint16_t *I;
    uint8_t *DELAYREG;
    uint8_t *SOUNDREG;
    uint16_t *PC;
    uint16_t *SP;
    uint16_t *STACK;
    uint8_t *VRAM;
} emulator;


uint8_t handler_global(emulator *e);
