all:
	gcc -fPIC -c -o chip8lib.o chip8lib.c
	gcc -shared -o chip8lib.so chip8lib.o

clean:
	rm chip8lib.o chip8lib.so
