#ifndef EMULATOR_H
#define EMULATOR_H

#include <SDL2/SDL.h>
#include "types.h"

// SDL Container object
struct sdl_t {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioSpec want, have;
    SDL_AudioDeviceID dev;
};

// Emulator states
enum emu_state_t {
    QUIT,
    RUNNING,
    PAUSED,
};

// CHIP-8 extensions/quirks support
enum extension_t {
    CHIP8,
    SUPERCHIP8,
};

// Emulator configuration object
struct config_t {
    u32 window_width;                   // SDL window width
    u32 window_height;                  // SDL window height
    u32 fg_color;                       // Foreground color RGBA8888
    u32 bg_color;                       // Background color RGBA8888
    u32 scale_factor;                   // Amount to scale a CHIP8 pixel by e.g. 20x will be a 20x larger window
    bool pixel_outlines;                // Draw pixel "outlines" yes/no
    u32 insts_per_second;               // CHIP8 CPU "clock rate" or hz
    u32 square_wave_freq;               // Frequency of square wave sound e.g. 440hz for middle A
    u32 audio_sample_rate;              // Sample rate of audio e.g. 44100hz for CD quality
    i16 volume;                         // How loud or not is the sound
    extension_t current_extension;      // Current quirks/extension support for e.g. CHIP8 vs. SUPERCHIP
    u8 refresh_rate;                    // refresh rate of screen
    // Debug logs
    bool instruction_execution;
    bool register_changes;
    bool memory_access;
    bool stack_operations;
    bool input_keys;
    bool timers;
    bool performance_metrics;
};

#endif // EMULATOR_H