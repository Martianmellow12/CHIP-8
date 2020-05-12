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
	uint16_t *KEY;
} emulator;


void initialize(emulator *e);
int load_program(emulator *e, char* filename, uint16_t start_addr);


uint8_t handler_global(emulator *e);


void op_00E0(emulator *e, uint16_t op);
void op_00EE(emulator *e, uint16_t op);
void op_1nnn(emulator *e, uint16_t op);
void op_2nnn(emulator *e, uint16_t op);
void op_3xkk(emulator *e, uint16_t op);
void op_4xkk(emulator *e, uint16_t op);
void op_5xy0(emulator *e, uint16_t op);
void op_6xkk(emulator *e, uint16_t op);
void op_7xkk(emulator *e, uint16_t op);
void op_8xy0(emulator *e, uint16_t op);
void op_8xy1(emulator *e, uint16_t op);
void op_8xy2(emulator *e, uint16_t op);
void op_8xy3(emulator *e, uint16_t op);
void op_8xy4(emulator *e, uint16_t op);
void op_8xy5(emulator *e, uint16_t op);
void op_8xy6(emulator *e, uint16_t op);
void op_8xy7(emulator *e, uint16_t op);
void op_8xyE(emulator *e, uint16_t op);
void op_9xy0(emulator *e, uint16_t op);
void op_Annn(emulator *e, uint16_t op);
void op_Bnnn(emulator *e, uint16_t op);
void op_Cxkk(emulator *e, uint16_t op);
void op_Dxyn(emulator *e, uint16_t op);
void op_Ex9E(emulator *e, uint16_t op);
void op_ExA1(emulator *e, uint16_t op);
void op_Fx07(emulator *e, uint16_t op);
void op_Fx0A(emulator *e, uint16_t op);
void op_Fx15(emulator *e, uint16_t op);
void op_Fx18(emulator *e, uint16_t op);
void op_Fx1E(emulator *e, uint16_t op);
void op_Fx29(emulator *e, uint16_t op);
void op_Fx33(emulator *e, uint16_t op);
void op_Fx55(emulator *e, uint16_t op);
void op_Fx65(emulator *e, uint16_t op);
