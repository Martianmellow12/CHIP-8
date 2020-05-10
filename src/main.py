# CHIP-8 Emulator
# Written by Michael Kersting, Jr.
# May 5, 2020
import ctypes
import sys
import os

# Get the location of main.py
PATH = os.path.dirname(os.path.realpath(sys.argv[0]))


# Create variables used for system components, including:
#   4KB of memory (one byte per address)
#   16 8-bit general purpose registers (V0-VF)
#   One 16-bit address register (I)
#   Two 8-bit counter registers (one for delay, one for sound)
#   One 16-bit program counter (PC)
#   One 16-bit stack pointer (SP)
#   A stack consisting of 16 16-bit values
#   2KB of VRAM (not in the spec, but used to allow C control of the display)
#   A 16-bit value used to represent key states
RAM = (ctypes.c_uint8 * 4096)(0)
V = (ctypes.c_uint8 * 16)(0)
I = ctypes.pointer(ctypes.c_uint16(0))
DELAYREG = ctypes.pointer(ctypes.c_uint8(0))
SOUNDREG = ctypes.pointer(ctypes.c_uint8(0))
PC = ctypes.pointer(ctypes.c_uint16(0))
SP = ctypes.pointer(ctypes.c_uint16(0))
STACK = (ctypes.c_uint16 * 16)(0)
VRAM = (ctypes.c_uint8 * 2048)(0)
KEY = ctypes.pointer(ctypes.c_uint16(0))


# Load in the shared library for the CHIP-8 Emulator
chip8lib = ctypes.CDLL(os.path.join(PATH, "chip8lib.so"))


# Set up the class to serve as the emulator struct
class emulator(ctypes.Structure):
    _fields_ = [
        ("RAM", ctypes.POINTER(ctypes.c_uint8)),
        ("V", ctypes.POINTER(ctypes.c_uint8)),
        ("I", ctypes.POINTER(ctypes.c_uint16)),
        ("DELAYREG", ctypes.POINTER(ctypes.c_uint8)),
        ("SOUNDREG", ctypes.POINTER(ctypes.c_uint8)),
        ("PC", ctypes.POINTER(ctypes.c_uint16)),
        ("SP", ctypes.POINTER(ctypes.c_uint16)),
        ("STACK", ctypes.POINTER(ctypes.c_uint16)),
        ("VRAM", ctypes.POINTER(ctypes.c_uint8)),
        ("KEY", ctypes.POINTER(ctypes.c_uint16))
    ]


# Set up the argument types for the global handler
chip8lib.handler_global.argtypes = (ctypes.POINTER(emulator),)


# Create an instance of the emulator and set its fields
e = emulator()
e.RAM = RAM
e.V = V
e.I = I
e.DELAYREG = DELAYREG
e.SOUNDREG = SOUNDREG
e.PC = PC
e.SP = SP
e.STACK = STACK
e.VRAM = VRAM
e.KEY = KEY


########################################
#        Dump Emulator Function        #
########################################

# This function dumps the contents of the emulator to
# the terminal for debugging purposes
def dump_emulator(emu, show_mem=False):
    # First dump all of the register contents
    print("#### REGISTERS ####")
    for i in range(0, 16):
        print("\tV%d: %s (%d)" % (i, hex(V[i]), V[i]))
    print("")
    print("\tI: %s (%d)" % (hex(I.contents.value), I.contents.value))
    print("\tDELAYREG: %s (%d)" % (hex(DELAYREG.contents.value), DELAYREG.contents.value))
    print("\tSOUNDREG: %s (%d)" % (hex(SOUNDREG.contents.value), SOUNDREG.contents.value))
    print("\tPC: %s (%d)" % (hex(PC.contents.value), PC.contents.value))
    print("\tSP: %s (%d)" % (hex(SP.contents.value), SP.contents.value))
    print("")
    
    # Next dump the stack
    print("#### STACK ####")
    for i in range(0, 16):
        print("\tLOC %d: %s (%d)" % (i, hex(STACK[i]), STACK[i]))
    print("")
        
    # Finally, dump memory if we're supposed to
    if (show_mem):
        print("#### RAM ####")
        for i in range(0, 4096):
            print("\tLOC %s: %s (%d)" % (hex(i), hex(RAM[i]), RAM[i]))
        
        
# DEBUG STUFF - REMOVE WHEN DONE
dump_emulator(e)