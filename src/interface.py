# CHIP-8 User Interface Library
# Written by Michael Kersting, Jr.
# May 5, 2020
import pygame


# Interface Config Variables
SCALE = 5
WIDTH = 64 * SCALE
HEIGHT = 32 * SCALE


# Initialization Function
def initialize(win_scale=5):
    # Set up window variables
    SCALE = win_scale
    WIDTH = 64 * win_scale
    HEIGHT = 32 * win_scale

    # Initialize pygame
    pygame.init()
    screen


# Set up pygame for a display
pygame.init()
screen = pygame.display.set_mode([WIDTH, HEIGHT])


# Testing Stuff - Delete When Done

vram = [[0 for i in range(0, 32)] for j in range(0, 64)]

for i in range(0, 64):
    for j in range(0, 32):
        if i==0: vram[i][j] = 1
        if j==0: vram[i][j] = 1
        if i==63: vram[i][j] = 1
        if j==31: vram[i][j] = 1

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    for i in range(0, 64):
        for j in range(0, 32):
            if vram[i][j] == 1:
                pygame.draw.rect(screen, (255, 255, 255), (i*SCALE, j*SCALE, SCALE, SCALE))
            else:
                pygame.draw.rect(screen, (0, 0, 0), (i*SCALE, j*SCALE, SCALE, SCALE))
    pygame.display.flip()
pygame.quit()
