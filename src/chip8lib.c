// CHIP-8 Emulation Library
// Written by Michael Kersting, Jr.
// May 5, 2020
#include "chip8lib.h"


/************************************************
 *          Helper Functions/Macros             *
 ************************************************/
 
 
 // Increment the program counter
 #define	PC_INC(E)	(*(E->PC) += 1)


/************************************************
 *               Opcode Handling                *
 *						                        *
 * Opcodes are passed to the global handler,    *
 * which then passes it to the proper handler.  *
 * Once the specific handler has completed the  *
 * execution, the global handler will return    *
 * control to main.py by returning a code.	    *
 *						                        *
 ************************************************/


// Global Handler
// Fetches op, executes op (instruction handlers increment the PC)
uint8_t handler_global(emulator *e) {
    // Fetch the current opcode
	uint16_t op = e->RAM[*(e->PC)];
	
	// Pass the opcode to the correct handler
	// TODO
	
	// Return OK status code (0)
    return 0;
}


// Opcode 0nnn - Jump to machine code routine at nnn
void op_0nnn(emulator *e, uint16_t op) {
	// Ignored by modern interpreters, do nothing
	PC_INC(e);
	return;
}


// Opcode 00E0 - Clear the display
void op_00E0(emulator *e, uint16_t op) {
	// Set all VRAM locations to 0
	for (int i=0; i<2048; i++) e->VRAM[i] = 0;
	PC_INC(e);
	return;
}


// Opcode 00EE - Return from a subroutine
void op_00EE(emulator *e, uint16_t op) {
	// Set the program counter to the address at the top of the stack
	*(e->PC) = e->STACK[*(e->SP)];
	
	// Decrement the stack pointer
	*(e->SP) -= 1;
	
	// DON'T INCREMENT PC
	return;
}


// Opcode 1nnn - Jump to location nnn
void op_1nnn(emulator *e, uint16_t op) {
	// Set the program counter to nnn (last 12 bits of op)
	*(e->PC) = op & 0x0FFF;
	
	// DON'T INCREMENT PC
	return;
}


// Opcode 2nnn - Call subroutine at nnn
void op_2nnn(emulator *e, uint16_t op) {
	// Increment the stack pointer and put the PC on the top of the stack
	*(e->SP) += 1;
	e->STACK[*(e->SP)] = *(e->PC);
	
	// Set the PC to nnn
	*(e->PC) = op & 0x0FFF;
	
	// DON'T INCREMENT PC
	return;
}


// Opcode 3xkk - Skip the next instruction if Vx == kk
void op_3xkk(emulator *e, uint16_t op) {
	// Get x and kk
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t kk = (op & 0x00FF);
	
	// Compare Vx and kk, incrementing the PC if they're equal
	if (e->V[x] == kk) PC_INC(e);
	
	PC_INC(e);
	return;
}


// Opcode 4xkk - Skip the next instruction if Vx != kk
void op_4xkk(emulator *e, uint16_t op) {
	// Get x and kk
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t kk = (op & 0x00FF);
	
	// Compare Vx and kk, incrementing the PC if they're not equal
	if (e->V[x] != kk) PC_INC(e);
	
	PC_INC(e);
	return;
}


// Opcode 5xy0 - Skip the next instruction if Vx == Vy
void op_5xy0(emulator *e, uint16_t op) {
	// Get x and y
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t y = (op & 0x00F0) >> 4;
	
	// Compare Vx and Vy, incrementing the PC if they're equal
	if (e->V[x] == e->V[y]) PC_INC(e);
	
	PC_INC(e);
	return;
}


// Opcode 6xkk - Set Vx = kk
void op_6xkk(emulator *e, uint16_t op) {
	// Get x and kk
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t kk = (op & 0x00FF);
	
	// Put kk into Vx
	e->V[x] = kk;
	
	PC_INC(e);
	return;
}


// Opcode 7xkk - Set Vx = Vx + kk
void op_7xkk(emulator *e, uint16_t op) {
	// Get x and kk
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t kk = (op & 0x00FF);
	
	// Add kk to Vx
	e->V[x] += kk;
	
	PC_INC(e);
	return;
}


// Opcode 8xy0 - Set Vx = Vy
void op_8xy0(emulator *e, uint16_t op) {
	// Get x and y
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t y = (op & 0x00F0) >> 4;
	
	// Store Vy into Vx
	e->V[x] = e->V[y];
	
	PC_INC(e);
	return;
}


// Opcode 8xy1 - Set Vx = Vx OR Vy
void op_8xy1(emulator *e, uint16_t op) {
	// Get x and y
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t y = (op & 0x00F0) >> 4;
	
	// Store Vx OR Vy into Vx
	e->V[x] = (e->V[x] | e->V[y]);
	
	PC_INC(e);
	return;
}


// Opcode 8xy2 - Set Vx = Vx AND Vy
void op_8xy2(emulator *e, uint16_t op) {
	// Get x and y
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t y = (op & 0x00F0) >> 4;
	
	// Store Vx AND Vy into Vx
	e->V[x] = (e->V[x] & e->V[y]);
	
	PC_INC(e);
	return;
}


// Opcode 8xy3 - Set VX = Vx XOR Vy
void op_8xy3(emulator *e, uint16_t op) {
	// Get x and y
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t y = (op & 0x00F0) >> 4;
	
	// Store Vx XOR Vy into Vx
	e->V[x] = (e->V[x] ^ e->V[y]);
	
	PC_INC(e);
	return;
}


// Opcode 8xy4 - Set Vx = Vx + Vy, and set VF = carry
void op_8xy4(emulator *e, uint16_t op) {
	// Get x and y
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t y = (op & 0x00F0) >> 4;
	
	// Set the result
	uint16_t result = e->V[x] + e->V[y];
	e->V[x] = (result & 0x00FF);
	
	// Set the carry flag
	if (result > 0xFF) e->V[0xF] = 1;
	else e->V[0xF] = 0;
	
	PC_INC(e);
	return;
}