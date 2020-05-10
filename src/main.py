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
RAM = (ctypes.c_uint8 * 4096)(0)
V = (ctypes.c_uint8 * 16)(0)
I = ctypes.pointer(ctypes.c_uint16(0))
DELAYREG = ctypes.pointer(ctypes.c_uint8(0))
SOUNDREG = ctypes.pointer(ctypes.c_uint8(0))
PC = ctypes.pointer(ctypes.c_uint16(0))
SP = ctypes.pointer(ctypes.c_uint16(0))
STACK = (ctypes.c_uint16 * 16)(0)
VRAM = (ctypes.c_uint8 * 2048)(0)


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
	("VRAM", ctypes.POINTER(ctypes.c_uint8))
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

I.contents.value = 65533
chip8lib.handler_global(e)
print(I.contents.value)
