// CHIP-8 ROM Dump Tool
// Written by Michael Kersting, Jr.
// May 10, 2020
#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	uint16_t op;
	uint8_t byte;
	
	// Make sure our number of args is correct
	if (argc != 2) {
		printf("Incorrect number of arguments!\n");
		return 0;
	}
	
	// Create the file pointer
	FILE *filein = fopen(argv[1], "rb");
	if (filein == NULL) {
		printf("Unable to open file\n");
		return 0;
	}
	
	// Dump the contents to the terminal
	while (fscanf(filein, "%c", &byte) != EOF) {
		op = byte << 8;
		fscanf(filein, "%c", &byte);
		op = op | byte;
		printf("%04X\n", op);
	}
	
	fclose(filein);
	return 0;
}