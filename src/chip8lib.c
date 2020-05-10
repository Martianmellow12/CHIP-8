// CHIP-8 Emulation Library
// Written by Michael Kersting, Jr.
// May 5, 2020
#include "chip8lib.h"


/************************************************
 *               Opcode Handling                *
 *						*
 * Opcodes are passed to the global handler,    *
 * which then passes it to the proper handler.  *
 * Once the specific handler has completed the  *
 * execution, the global handler will return    *
 * control to main.py by returning a code.	*
 *						*
 ************************************************/


// Global Handler
// Requires access to all hardware components
uint8_t handler_global(emulator *e) {
    // TODO
    *(e->I) += 4;
    return 0;
}
