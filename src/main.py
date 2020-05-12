# CHIP-8 Emulator
# Written by Michael Kersting, Jr.
# May 5, 2020
import ctypes
import sys
import os
import pygame


# Debug flag
DEBUG = False


# Keypad Mapping
KEYPAD_MAP = {
    pygame.K_0:0,
    pygame.K_1:1,
    pygame.K_2:2,
    pygame.K_3:3,
    pygame.K_4:4,
    pygame.K_5:5,
    pygame.K_6:6,
    pygame.K_7:7,
    pygame.K_8:8,
    pygame.K_9:9,
    pygame.K_a:10,
    pygame.K_b:11,
    pygame.K_c:12,
    pygame.K_d:13,
    pygame.K_e:14,
    pygame.K_f:15
}


# Interface Config Variables
UI_SCALE = 8
UI_WIDTH = UI_SCALE * 64
UI_HEIGHT = UI_SCALE * 32
UI_BGCOLOR = (0, 0, 0)
UI_FGCOLOR = (255, 255, 255)
UI_FGPX = pygame.Surface((UI_SCALE, UI_SCALE))
UI_BGPX = pygame.Surface((UI_SCALE, UI_SCALE))
UI_PIXELS = [[0 for i in range(0, 32)] for j in range(0, 64)]
UI_CSTATE = [[0 for i in range(0, 32)] for j in range(0, 64)]


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


########################################
#           Helper Functions           #
########################################

# Get a hex string of the current opcode
def current_op():
    result = "0x"
    msb = RAM[PC.contents.value]
    lsb = RAM[PC.contents.value + 1]
    if len(hex(msb)[2:]) < 2: result += "0"
    result +=hex(msb)[2:]
    if len(hex(lsb)[2:]) < 2: result += "0"
    result += hex(lsb)[2:]
    return result


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
chip8lib.initialize.argtypes = (ctypes.POINTER(emulator),)
chip8lib.load_program.argtypes = (ctypes.POINTER(emulator), ctypes.c_char_p, ctypes.c_uint16)
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


# Initialize the emulator
chip8lib.initialize(ctypes.pointer(e))


########################################
#        Dump Emulator Function        #
########################################

# This function dumps the contents of the emulator to
# the terminal for debugging purposes
def dump_emulator(emu, show_mem=False):
    # Display the current instruction
    print("Current instruction: %s\n" % current_op())

    # Dump all of the register contents
    print("#### REGISTERS ####")
    for i in range(0, 16):
        print("\tV%s: %s (%d)" % (hex(i)[2:], hex(V[i]), V[i]))
    print("")
    print("\tI: %s (%d)" % (hex(I.contents.value), I.contents.value))
    print("\tDELAYREG: %s (%d)" % (hex(DELAYREG.contents.value), DELAYREG.contents.value))
    print("\tSOUNDREG: %s (%d)" % (hex(SOUNDREG.contents.value), SOUNDREG.contents.value))
    print("\tPC: %s (%d)" % (hex(PC.contents.value), PC.contents.value))
    print("\tSP: %s (%d)" % (hex(SP.contents.value), SP.contents.value))
    print("")

    # Dump the stack
    print("#### STACK ####")
    for i in range(0, 16):
        print("\tLOC %d: %s (%d)" % (i, hex(STACK[i]), STACK[i]))
    print("")

    # Dump memory if we're supposed to
    if (show_mem):
        print("#### RAM ####")
        for i in range(0, 4096):
            print("\tLOC %s: %s (%d)" % (hex(i), hex(RAM[i]), RAM[i]))


# This function dumps the contents of VRAM to
# the terminal in a somewhat graphical layout
def dump_vram(emu):
    result = str()

    # 0 - Space, 1 - @
    for i in range(0, 32):
        for j in range(0, 64):
            if VRAM[((64*(i%32)) + (j%64))]:
                result += "@"
            else:
                result += " "
        result += "\n"

    print(result)


########################################
#            Emulation Init            #
########################################

# Load a test program into RAM
#prog_path = str(input("ROM Path > "))
prog_path = sys.argv[1]
prog_path = prog_path.encode("utf-8")
if (chip8lib.load_program(ctypes.pointer(e), prog_path, 512) == 0):
    print("Success")
else:
    print("Failure")


########################################
#            Interface Init            #
########################################

# Init pygame and create the display window
pygame.init()
display = pygame.display.set_mode([UI_WIDTH, UI_HEIGHT])

# Clear the display
pygame.draw.rect(display, UI_BGCOLOR, (0, 0, UI_WIDTH, UI_HEIGHT))
pygame.display.flip()

# Create the pixels
UI_FGPX.fill(UI_FGCOLOR)
UI_BGPX.fill(UI_BGCOLOR)
for x in range(0, 64):
    for y in range(0, 32):
        UI_PIXELS[x][y] = pygame.rect.Rect(x*UI_SCALE, y*UI_SCALE, UI_SCALE, UI_SCALE)
        UI_CSTATE[x][y] = 0;

# Create the clock
clock = pygame.time.Clock()


########################################
#            Emulation Loop            #
########################################
running = True
while (running):
    # Check for user events
    for event in pygame.event.get():
        # Window close event
        if event.type == pygame.QUIT:
            running = False
        # Key pressed
        if event.type == pygame.KEYDOWN:
            if (event.key in KEYPAD_MAP.keys()): KEY[KEYPAD_MAP[event.key]] = 1
        # Key released
        if event.type == pygame.KEYUP:
            if (event.key in KEYPAD_MAP.keys()): KEY[KEYPAD_MAP[event.key]] = 0

    # Perform an emulation cycle
    ret_code = chip8lib.handler_global(e)
    if (ret_code == 1):
        print("INVALID OPCODE ENCOUNTERED")
        print("OPCODE: %s" % current_op())
        print("HALTING EXECUTION\n")
        input("Press return key to exit")
        pygame.quit()
        quit()

    # Update the UI
    for x in range(0, 64):
        for y in range(0, 32):
            if VRAM[(64*y) + x] != 0 and UI_CSTATE[x][y] == 0:
                display.blit(UI_FGPX, (x*UI_SCALE, y*UI_SCALE))
                pygame.display.update(UI_PIXELS[x][y])
                UI_CSTATE[x][y] = 1
            elif VRAM[(64*y) + x] == 0 and UI_CSTATE[x][y] != 0:
                display.blit(UI_BGPX, (x*UI_SCALE, y*UI_SCALE))
                pygame.display.update(UI_PIXELS[x][y])
                UI_CSTATE[x][y] = 0
    clock.tick_busy_loop(400)

    # Pause execution if we're debugging
    if (DEBUG):
        dump_emulator(e)
        resume = False
        print("Paused - Press return to resume execution")
        while (not resume):
            pygame.display.update()

            # Check for events
            for event in pygame.event.get():
                # Window close event
                if event.type == pygame.QUIT:
                    pygame.quit()
                    quit()
                # Enter key pressed
                if event.type == pygame.KEYDOWN and event.key == pygame.K_RETURN:
                    resume = True
