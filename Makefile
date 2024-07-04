CC = g++
CFLAGS = -Wall
LIBS = -lSDL2

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source and object files
SRCS = $(SRC_DIR)/Chip8.cpp $(SRC_DIR)/Emulator.cpp $(SRC_DIR)/Assembler.cpp $(SRC_DIR)/ini.c $(SRC_DIR)/INIReader.cpp 
OBJS = $(BUILD_DIR)/Chip8.o $(BUILD_DIR)/Emulator.o $(BUILD_DIR)/Assembler.o $(BUILD_DIR)/ini.o $(BUILD_DIR)/INIReader.o

# Default target
all: $(BUILD_DIR) chip8

# Ensure the build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build the main program
chip8: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BUILD_DIR)/chip8 $(LIBS)

# Create all object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -c -o $@

# Clean up build directory
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/out.ch8 $(BUILD_DIR)/chip8