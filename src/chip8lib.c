// CHIP-8 Emulation Library
// Written by Michael Kersting, Jr.
// May 5, 2020
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "chip8lib.h"


/************************************************
 *         Cotntrol Functions/Macros            *
 ************************************************/


// Increment the program counter
#define	PC_INC(E)		(*(E->PC) += 2)			// Increments the PC (I got tired of typing the code)
#define INDEX(X,Y)		((64*(Y%32)) + (X%64))	// It gets an index for VRAM given an X and Y coordinate


// Initialize various emulator functions (must be called once before emulation takes place)
void initialize(emulator *e) {
	// Init the random number generator
	srand(time(NULL));

  // Set the program counter
  *(e->PC) = 0x200;

	// Set the 0 sprite in RAM
	e->RAM[0x000] = 0xF0;
	e->RAM[0x001] = 0x90;
	e->RAM[0x002] = 0x90;
	e->RAM[0x003] = 0x90;
	e->RAM[0x004] = 0xF0;

	// Set the 1 sprite in RAM
	e->RAM[0x005] = 0x20;
	e->RAM[0x006] = 0x60;
	e->RAM[0x007] = 0x20;
	e->RAM[0x008] = 0x20;
	e->RAM[0x009] = 0x70;

	// Set the 2 sprite in RAM
	e->RAM[0x00A] = 0xF0;
	e->RAM[0x00B] = 0x10;
	e->RAM[0x00C] = 0xF0;
	e->RAM[0x00D] = 0x80;
	e->RAM[0x00E] = 0xF0;

	// Set the 3 sprite in RAM
	e->RAM[0x00F] = 0xF0;
	e->RAM[0x010] = 0x10;
	e->RAM[0x011] = 0xF0;
	e->RAM[0x012] = 0x10;
	e->RAM[0x013] = 0xF0;

	// Set the 4 sprite in RAM
	e->RAM[0x014] = 0x90;
	e->RAM[0x015] = 0x90;
	e->RAM[0x016] = 0xF0;
	e->RAM[0x017] = 0x10;
	e->RAM[0x018] = 0x10;

	// Set the 5 sprite in RAM
	e->RAM[0x019] = 0xF0;
	e->RAM[0x01A] = 0x80;
	e->RAM[0x01B] = 0xF0;
	e->RAM[0x01C] = 0x10;
	e->RAM[0x01D] = 0xF0;

	// Set the 6 sprite in RAM
	e->RAM[0x01E] = 0xF0;
	e->RAM[0x01F] = 0x80;
	e->RAM[0x020] = 0xF0;
	e->RAM[0x021] = 0x90;
	e->RAM[0x022] = 0xF0;

	// Set the 7 sprite in RAM
	e->RAM[0x023] = 0xF0;
	e->RAM[0x024] = 0x10;
	e->RAM[0x025] = 0x20;
	e->RAM[0x026] = 0x40;
	e->RAM[0x027] = 0x40;

	// Set the 8 sprite in RAM
	e->RAM[0x028] = 0xF0;
	e->RAM[0x029] = 0x90;
	e->RAM[0x02A] = 0xF0;
	e->RAM[0x02B] = 0x90;
	e->RAM[0x02C] = 0xF0;

	// Set the 9 sprite in RAM
	e->RAM[0x02D] = 0xF0;
	e->RAM[0x02E] = 0x90;
	e->RAM[0x02F] = 0xF0;
	e->RAM[0x030] = 0x10;
	e->RAM[0x031] = 0xF0;

	// Set the A sprite in RAM
	e->RAM[0x032] = 0xF0;
	e->RAM[0x033] = 0x90;
	e->RAM[0x034] = 0xF0;
	e->RAM[0x035] = 0x90;
	e->RAM[0x036] = 0x90;

	// Set the B sprite in RAM
	e->RAM[0x037] = 0xE0;
	e->RAM[0x038] = 0x90;
	e->RAM[0x039] = 0xE0;
	e->RAM[0x03A] = 0x90;
	e->RAM[0x03B] = 0xE0;

	// Set the C sprite in RAM
	e->RAM[0x03C] = 0xF0;
	e->RAM[0x03D] = 0x80;
	e->RAM[0x03E] = 0x80;
	e->RAM[0x03F] = 0x80;
	e->RAM[0x040] = 0xF0;

	// Set the D sprite in RAM
	e->RAM[0x041] = 0xE0;
	e->RAM[0x042] = 0x90;
	e->RAM[0x043] = 0x90;
	e->RAM[0x044] = 0x90;
	e->RAM[0x045] = 0xE0;

	// Set the E sprite in RAM
	e->RAM[0x046] = 0xF0;
	e->RAM[0x047] = 0x80;
	e->RAM[0x048] = 0xF0;
	e->RAM[0x049] = 0x80;
	e->RAM[0x04A] = 0xF0;

	// Set the F sprite in RAM
	e->RAM[0x04B] = 0xF0;
	e->RAM[0x04C] = 0x80;
	e->RAM[0x04D] = 0xF0;
	e->RAM[0x04E] = 0x80;
	e->RAM[0x04F] = 0x80;
}


// Load a program into RAM from a filelength
int load_program(emulator *e, char* filename, uint16_t start_addr) {
	// Variables
	uint8_t byte;
	uint16_t addr = start_addr;

	// Open the file
	FILE *filein = fopen(filename, "rb");
	if (filein == NULL) {
		return 1;
	}

	// Read the file's contents into RAM
	while (fscanf(filein, "%c", &byte) != EOF) {
		e->RAM[addr] = byte;
		addr++;
	}

	// Return successfully (0)
	return 0;
}


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
	uint16_t op = (e->RAM[*(e->PC)] << 8) | (e->RAM[*(e->PC) + 1]);

	// Pass the opcode to the correct handler
	switch ((op & 0xF000) >> 12) {
		case 0x0:
			switch (op & 0x00FF) {
				case 0xE0:
					op_00E0(e, op);
					break;

				case 0xEE:
					op_00EE(e, op);
					break;

				default:
					// Unknown opcode, return 1
					return 1;
			}
			break;

		case 0x1:
			op_1nnn(e, op);
			break;

		case 0x2:
			op_2nnn(e, op);
			break;

		case 0x3:
			op_3xkk(e, op);
			break;

		case 0x4:
			op_4xkk(e, op);
			break;

		case 0x5:
			op_5xy0(e, op);
			break;

		case 0x6:
			op_6xkk(e, op);
			break;

		case 0x7:
			op_7xkk(e, op);
			break;

		case 0x8:
			switch (op & 0x000F) {
				case 0x0:
					op_8xy0(e, op);
					break;

				case 0x1:
					op_8xy1(e, op);
					break;

				case 0x2:
					op_8xy2(e, op);
					break;

				case 0x3:
					op_8xy3(e, op);
					break;

				case 0x4:
					op_8xy4(e, op);
					break;

				case 0x5:
					op_8xy5(e, op);
					break;

				case 0x6:
					op_8xy6(e, op);
					break;

				case 0x7:
					op_8xy7(e, op);
					break;

				case 0xE:
					op_8xyE(e, op);
					break;

				default:
					// Unknown opcode, return 1
					return 1;
			}
			break;

		case 0x9:
			op_9xy0(e, op);
			break;

		case 0xA:
			op_Annn(e, op);
			break;

		case 0xB:
			op_Bnnn(e, op);
			break;

		case 0xC:
			op_Cxkk(e, op);
			break;

		case 0xD:
			op_Dxyn(e, op);
			break;

		case 0xE:
			switch(op & 0x00FF) {
				case 0x9E:
					op_Ex9E(e, op);
					break;

				case 0xA1:
					op_ExA1(e, op);
					break;

				default:
					// Unknown opcode, return 1
					return 1;
			}
			break;

		case 0xF:
			switch (op & 0x00FF) {
				case 0x07:
					op_Fx07(e, op);
					break;

				case 0x0A:
					op_Fx0A(e, op);
					break;

				case 0x15:
					op_Fx15(e, op);
					break;

				case 0x18:
					op_Fx18(e, op);
					break;

				case 0x1E:
					op_Fx1E(e, op);
					break;

				case 0x29:
					op_Fx29(e, op);
					break;

				case 0x33:
					op_Fx33(e, op);
					break;

				case 0x55:
					op_Fx55(e, op);
					break;

				case 0x65:
					op_Fx65(e, op);
					break;

				default:
					// Unknown opcode, return 1
					return 1;
			}
			break;

		default:
			// Unknown opcode, return 1
			return 1;
	}

	// Decrement the two timer registers
	if (*(e->DELAYREG) != 0) *(e->DELAYREG) -= 1;
	if (*(e->SOUNDREG) != 0) *(e->SOUNDREG) -= 1;

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
  // Decrement the stack pointer
	*(e->SP) -= 1;

	// Set the program counter to the address at the top of the stack
	*(e->PC) = e->STACK[*(e->SP)];

	PC_INC(e);
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
	// Put the PC on top of the stack, then increment the stack pointer
	e->STACK[*(e->SP)] = *(e->PC);
  *(e->SP) += 1;

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


// Opcode 8xy5 - Set Vx = Vx - Vy, and set VF = NOT borrow
void op_8xy5(emulator *e, uint16_t op) {
	// Get x and y
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t y = (op & 0x00F0) >> 4;

	// Set the borrow flag
	if (e->V[x] > e->V[y]) e->V[0xF] = 1;
	else e->V[0xF] = 0;

	// Set the result
	e->V[x] -= e->V[y];

	PC_INC(e);
	return;
}


// Opcode 8xy6 - Shift Vx right by 1 bit, and set VF = LSB of Vx pre-shift
void op_8xy6(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// Set VF
	e->V[0xF] = (e->V[x] & 0x01);

	// Set Vx
	e->V[x] = (e->V[x] >> 1);

	PC_INC(e);
	return;
}


// Opcode 8xy7 - Set Vx = Vy - Vx, and set VF = NOT borrow
void op_8xy7(emulator *e, uint16_t op) {
	// Get x and y
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t y = (op & 0x00F0) >> 4;

	// Set the borrow flag
	if (e->V[y] > e->V[x]) e->V[0xF] = 1;
	else e->V[0xF] = 0;

	// Set the result
	e->V[x] = e->V[y] - e->V[x];

	PC_INC(e);
	return;
}


// Opcode 8xyE - Shift Vx left by 1 bit, and set VF = MSB of Vx pre-shift
void op_8xyE(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// Set VF
	e->V[0xF] = (e->V[x] & 0x80);

	// Set Vx
	e->V[x] = (e->V[x] << 1);

	PC_INC(e);
	return;
}


// Opcode 9xy0 - Skip the next instruction if Vx != Vy
void op_9xy0(emulator *e, uint16_t op) {
	// Get x and y
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t y = (op & 0x00F0) >> 4;

	// Increment the PC if Vx != Vy
	if (e->V[x] != e->V[y]) PC_INC(e);

	PC_INC(e);
	return;
}


// Opcode Annn - Set I = nnn
void op_Annn(emulator *e, uint16_t op) {
	// Set I equal to nnn
	*(e->I) = (op & 0x0FFF);

	PC_INC(e);
	return;
}


// Opcode Bnnn - Jump to location nnn + V0
void op_Bnnn(emulator *e, uint16_t op) {
	// Set the PC = nnn + V0
	*(e->PC) = (op & 0x0FFF) + (e->V[0x0]);

	// DON'T INCREMENT PC
	return;
}


// Opcode Cxkk - Set Vx = randint_8bit AND kk
void op_Cxkk(emulator *e, uint16_t op) {
	// Get x and kk
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t kk = (op & 0x00FF);

	// Generate a random integer from 0 to 255 inclusive
	// then AND it with kk, storing the result in Vx
	e->V[x] = ((rand() % 0xFF) & kk);

	PC_INC(e);
	return;
}


// Opcode Dxyn - Display an n-byte sprite at (x, y)
void op_Dxyn(emulator *e, uint16_t op) {
	// Variables we'll use to copy the sprite
	uint8_t sprite[15] = {0};
	uint16_t cur_addr = *(e->I);

	// Get x, y, and n (x and y are the coords)
	uint8_t x = e->V[(op & 0x0F00) >> 8];
	uint8_t y = e->V[(op & 0x00F0) >> 4];
	uint8_t n = (op & 0x000F);

	// Copy the sprite into our array from RAM
	// The sprite's location starts at I
	for (uint16_t i = 0; i<n; i++) {
		sprite[i] = e->RAM[cur_addr];
		cur_addr++;
	}

	// XOR the sprite into VRAM
	for (uint8_t ypos=y; ypos<y+n; ypos++) {
    for (uint8_t xpos=x; xpos<x+8; xpos++) {
      // Get the new and old pixels ready
      uint8_t new = (sprite[ypos-y] >> (7-(xpos-x))) & 0x01;
      e->VRAM[INDEX(xpos, ypos)] &= 0x01;

      // If they're both 1s, set VF
      if ((e->VRAM[INDEX(xpos, ypos)] == 1) && (new == 1)) e->V[0xF] = 0x01;

      // Set the new pixel
      e->VRAM[INDEX(xpos, ypos)] ^= new;
    }
  }

	PC_INC(e);
	return;
}


// Opcode Ex9E - Skip the next instruction if the key with value Vx is pressed
void op_Ex9E(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// If the key with value Vx is pressed, increment the PC
	if (e->KEY[e->V[x]] != 0) PC_INC(e);

	PC_INC(e);
	return;
}


// Opcode ExA1 - Skip the next instruction if the key with value Vx is not pressed
void op_ExA1(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// If the key with value Vx is not pressed, increment the PC
	if (e->KEY[e->V[x]] == 0) PC_INC(e);

	PC_INC(e);
	return;
}


// Opcode Fx07 - Set Vx = delay timer value
void op_Fx07(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// Set Vx = DELAYREG
	e->V[x] = *(e->DELAYREG);

	PC_INC(e);
	return;
}


// Opcode Fx0A - Wait for a keypress, then store the value of the key in Vx
void op_Fx0A(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// Check for pressed keys; if there are none,
	// don't increment the PC (repeat this instruction
	// until a key is pressed)
	for (uint8_t i=0; i<16; i++) {
		if (e->KEY[i] != 0) {
			PC_INC(e);
			break;
		}
	}

	// DON'T INCREMENT PC
	return;
}


// Opcode Fx15 - Set the delay timer = Vx
void op_Fx15(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// Set the delay timer
	*(e->DELAYREG) = e->V[x];

	PC_INC(e);
	return;
}


// Opcode Fx18 - Set the sound timer
void op_Fx18(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// Set the sound timer
	*(e->SOUNDREG) = e->V[x];

	PC_INC(e);
	return;
}


// Opcode Fx1E - Set I = I + Vx
void op_Fx1E(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// Set I = I + Vx
	*(e->I) += e->V[x];

	PC_INC(e);
	return;
}


// Opcode Fx29 - Set I = location of sprite for digit Vx
void op_Fx29(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// The characters are stored starting at memory
	// location 0x000, so multiply Vx by 5 (the
	// height of each char) to get it's address
	*(e->I) = 5*(e->V[x]);

	PC_INC(e);
	return;
}


// Opcode Fx33 - Store BCD representation of Vx in I (hundreds), I+1 (tens), and I+2 (ones)
void op_Fx33(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// Store the hundreds digit
	e->RAM[*(e->I)] = ((e->V[x]) / 100) % 10;

	// Store the tens digit
	e->RAM[*(e->I) + 1] = ((e->V[x]) / 10) % 10;

	// Store the ones digit
	e->RAM[*(e->I) + 2] = (e->V[x]) % 10;

	PC_INC(e);
	return;
}


// Opcode Fx55 - Store V0 through Vx in RAM starting at the address stored in I, and set I = I+x+1
void op_Fx55(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// Store each register from V0 to Vx (inclusive)
	for (uint8_t i=0; i<=x; i++) {
		e->RAM[*(e->I) + i] = e->V[i];
	}

  // Modify I (I found different opinions about this online)
  *(e->I) = *(e->I) + x + 1;

	PC_INC(e);
	return;
}


// Opcode Fx65 - Read V0 through Vx from RAM starting at the address stored in I, and set I = I+x+1
void op_Fx65(emulator *e, uint16_t op) {
	// Get x
	uint8_t x = (op & 0x0F00) >> 8;

	// Store each register from V0 to Vx (inclusive)
	for (uint8_t i=0; i<=x; i++) {
		e->V[i] = e->RAM[*(e->I) + i];
	}

  // Modify I (I found different opinions about this online)
  *(e->I) = *(e->I) + x + 1;

	PC_INC(e);
	return;
}
