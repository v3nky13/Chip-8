CC = g++
CFLAGS = -Wall

all: Chip8.o Emulator.o
	$(CC) $(CFLAGS) Chip8.o Emulator.o -o chip8 -lSDL2

Chip8.o: src/Chip8.cpp
	$(CC) $(CFLAGS) src/Chip8.cpp -c -o Chip8.o

Emulator.o: src/Emulator.cpp
	$(CC) $(CFLAGS) src/Emulator.cpp -c -o Emulator.o -lSDL2

clean: Chip8.o Emulator.o chip8
	rm Chip8.o Emulator.o chip8