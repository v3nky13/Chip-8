# Compiler and flags
CC = g++
CFLAGS = -Wall
LIBS = -lSDL2

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source and object files
SRCS = $(SRC_DIR)/Chip8.cpp $(SRC_DIR)/Emulator.cpp $(SRC_DIR)/Assembler.cpp
OBJS = $(BUILD_DIR)/Chip8.o $(BUILD_DIR)/Emulator.o $(BUILD_DIR)/Assembler.o
DEBUG_OBJS = $(BUILD_DIR)/Chip8_debug.o $(BUILD_DIR)/Emulator.o $(BUILD_DIR)/Assembler.o

# Default target
all: $(BUILD_DIR) chip8

# Ensure the build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build the main program
chip8: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BUILD_DIR)/chip8 $(LIBS)

# Build the debug version of the program
debug: $(DEBUG_OBJS)
	$(CC) $(CFLAGS) $(DEBUG_OBJS) -o $(BUILD_DIR)/chip8 $(LIBS)

# Pattern rule for object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -c -o $@

# Special rule for the Chip8_debug object file
$(BUILD_DIR)/Chip8_debug.o: $(SRC_DIR)/Chip8.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -DDEBUG $< -c -o $@

# Clean up build artifacts
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/chip8

.PHONY: all clean debug
