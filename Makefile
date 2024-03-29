CC = g++
CFLAGS = -Wall
LIBS = -lSDL2

all: Chip8.o Emulator.o
	$(CC) $(CFLAGS) Chip8.o Emulator.o -o chip8 $(LIBS)

debug: Chip8_debug.o Emulator.o
	$(CC) $(CFLAGS) Chip8_debug.o Emulator.o -o chip8 $(LIBS)

Chip8.o: src/Chip8.cpp
	$(CC) $(CFLAGS) src/Chip8.cpp -c -o Chip8.o

Chip8_debug.o: src/Chip8.cpp
	$(CC) $(CFLAGS) src/Chip8.cpp -c -o Chip8_debug.o -DDEBUG

Emulator.o: src/Emulator.cpp
	$(CC) $(CFLAGS) src/Emulator.cpp -c -o Emulator.o $(LIBS)

clean:
	rm -f Chip8.o Chip8_debug.o Emulator.o chip8