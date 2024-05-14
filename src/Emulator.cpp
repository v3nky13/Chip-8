#include <iostream>
#include <cstring>
#include "../include/Chip8.h"
#include "../include/Emulator.h"

// SDL Audio callback
// Fill out stream/audio buffer with data
void audio_callback(void *userdata, u8 *stream, i32 len) {
    config_t *config = (config_t *) userdata;

    i16 *audio_data = (i16 *) stream;
    static u32 running_sample_index = 0;
    const i32 square_wave_period = config->audio_sample_rate / config->square_wave_freq;
    const i32 half_square_wave_period = square_wave_period / 2;

    // We are filling out 2 bytes at a time (i16), len is in bytes,
    //   so divide by 2
    // If the current chunk of audio for the square wave is the crest of the wave, 
    //   this will add the volume, otherwise it is the trough of the wave, and will add
    //   "negative" volume
    for (int i = 0; i < len / 2; i++)
        audio_data[i] = ((running_sample_index++ / half_square_wave_period) % 2) ?
                        config->volume : 
                        -config->volume;
}

// Initialize SDL
bool init_sdl(sdl_t *sdl, config_t *config) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Could not initialize SDL subsystems! %s\n", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow("CHIP8 Emulator", SDL_WINDOWPOS_CENTERED, 
                                   SDL_WINDOWPOS_CENTERED, 
                                   config->window_width * config->scale_factor,
                                   config->window_height * config->scale_factor,
                                   0);
    if (!sdl->window) {
        SDL_Log("Could not create SDL window %s\n", SDL_GetError());
        return false;
    }

    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl->renderer) {
        SDL_Log("Could not create SDL renderer %s\n", SDL_GetError());
        return false;
    }

    // Init Audio stuff
    sdl->want = (SDL_AudioSpec) {
        .freq = 44100,          // 44100hz "CD" quality
        .format = AUDIO_S16LSB, // Signed 16 bit little endian
        .channels = 1,          // Mono, 1 channel
        .samples = 512,
        .callback = audio_callback,
        .userdata = config,     // Userdata passed to audio callback
    };

    sdl->dev = SDL_OpenAudioDevice(NULL, 0, &sdl->want, &sdl->have, 0);

    if (sdl->dev == 0) {
        SDL_Log("Could not get an Audio Device %s\n", SDL_GetError());
        return false;
    }

    if ((sdl->want.format != sdl->have.format) ||
        (sdl->want.channels != sdl->have.channels)) {

        SDL_Log("Could not get desired Audio Spec\n");
        return false;
    }

    return true;    // Success
}

// Set up initial emulator configuration
void init_config(config_t *config) {
    // Set defaults
    *config = (config_t) {
        .window_width  = 64,            // Chip-8 original X resolution
        .window_height = 32,            // Chip-8 original Y resolution
        .fg_color = 0xFFFFFFFF,         // WHITE
        .bg_color = 0x000000FF,         // BLACK
        .scale_factor = 20,             // Default resolution will be 1280x640
        .pixel_outlines = true,         // Draw pixel "outlines" by default
        .insts_per_second = 700,        // Number of instructions to emulate in 1 second (clock rate of CPU)
        .square_wave_freq = 440,        // 440hz for middle A
        .audio_sample_rate = 44100,     // CD quality, 44100hz
        .volume = 3000,                 // INT16_MAX would be max volume
        .current_extension = CHIP8,     // Set default quirks/extension to plain OG Chip-8
    };
}

// Clear screen / SDL Window to background color
void clear_screen(const sdl_t sdl, const config_t config) {
    const uint8_t r = (config.bg_color >> 24) & 0xFF;
    const uint8_t g = (config.bg_color >> 16) & 0xFF;
    const uint8_t b = (config.bg_color >>  8) & 0xFF;
    const uint8_t a = (config.bg_color >>  0) & 0xFF;

    SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
    SDL_RenderClear(sdl.renderer);
}

// Update window with any changes
void update_screen(const sdl_t sdl, const config_t config, Chip8 *chip8) {
    SDL_Rect rect = {.x = 0, .y = 0, .w = (i32) config.scale_factor, .h = (i32) config.scale_factor};

    // Grab bg color values to draw outlines
    const u8 bg_r = (config.bg_color >> 24) & 0xFF;
    const u8 bg_g = (config.bg_color >> 16) & 0xFF;
    const u8 bg_b = (config.bg_color >>  8) & 0xFF;
    const u8 bg_a = (config.bg_color >>  0) & 0xFF;

    // Loop through display pixels, draw a rectangle per pixel to the SDL window
    for (u32 i = 0; i < sizeof chip8->display; i++) {
        // Translate 1D index i value to 2D X/Y coordinates
        // X = i % window width
        // Y = i / window width
        rect.x = (i % config.window_width) * config.scale_factor;
        rect.y = (i / config.window_width) * config.scale_factor;

        if (chip8->display[i]) {
            // Pixel is on, draw foreground color
            if (chip8->pixel_color[i] != config.fg_color) {
                chip8->pixel_color[i] = config.fg_color;
            }

            const u8 r = (chip8->pixel_color[i] >> 24) & 0xFF;
            const u8 g = (chip8->pixel_color[i] >> 16) & 0xFF;
            const u8 b = (chip8->pixel_color[i] >>  8) & 0xFF;
            const u8 a = (chip8->pixel_color[i] >>  0) & 0xFF;

            SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
            SDL_RenderFillRect(sdl.renderer, &rect);
        
            if (config.pixel_outlines) {
                // If user requested drawing pixel outlines, draw those here
                SDL_SetRenderDrawColor(sdl.renderer, bg_r, bg_g, bg_b, bg_a);
                SDL_RenderDrawRect(sdl.renderer, &rect);
            }

        } else {
            // Pixel is off, draw background color
            if (chip8->pixel_color[i] != config.bg_color) {
                chip8->pixel_color[i] = config.bg_color;
            }

            const u8 r = (chip8->pixel_color[i] >> 24) & 0xFF;
            const u8 g = (chip8->pixel_color[i] >> 16) & 0xFF;
            const u8 b = (chip8->pixel_color[i] >>  8) & 0xFF;
            const u8 a = (chip8->pixel_color[i] >>  0) & 0xFF;

            SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
            SDL_RenderFillRect(sdl.renderer, &rect);
        }
    }

    SDL_RenderPresent(sdl.renderer);
}

// Handle user input
// CHIP8 Keypad  QWERTY 
// 123C          1234
// 456D          qwer
// 789E          asdf
// A0BF          zxcv
void handle_input(Chip8 *chip8, config_t *config, emu_state_t *state) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                // Exit window; End program
                *state = QUIT; // Will exit main emulator loop
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        // Escape key; Exit window & End program
                        *state = QUIT;
                        break;
                        
                    case SDLK_SPACE:
                        // Space bar
                        if (*state == RUNNING) {
                            *state = PAUSED;  // Pause
                            puts("==== PAUSED ====");
                        } else {
                            *state = RUNNING; // Resume
                        }
                        break;

                    case SDLK_EQUALS:
                        // '=': Reset CHIP8 machine for the current ROM
                        chip8->init_chip8(config, chip8->rom_name);
                        break;

                    case SDLK_j:
                        // 'j': Decrease color lerp rate
                        
                        break;

                    case SDLK_k:
                        // 'k': Increase color lerp rate
                        
                        break;

                    case SDLK_o:
                        // 'o': Decrease Volume
                        if (config->volume > 0)
                            config->volume -= 500;
                        break;

                    case SDLK_p:
                        // 'p': Increase Volume
                        if (config->volume < INT16_MAX)
                            config->volume += 500;
                        break;

                    // Map qwerty keys to CHIP8 keypad
                    case SDLK_1: chip8->keypad[0x1] = true; break;
                    case SDLK_2: chip8->keypad[0x2] = true; break;
                    case SDLK_3: chip8->keypad[0x3] = true; break;
                    case SDLK_4: chip8->keypad[0xC] = true; break;

                    case SDLK_q: chip8->keypad[0x4] = true; break;
                    case SDLK_w: chip8->keypad[0x5] = true; break;
                    case SDLK_e: chip8->keypad[0x6] = true; break;
                    case SDLK_r: chip8->keypad[0xD] = true; break;

                    case SDLK_a: chip8->keypad[0x7] = true; break;
                    case SDLK_s: chip8->keypad[0x8] = true; break;
                    case SDLK_d: chip8->keypad[0x9] = true; break;
                    case SDLK_f: chip8->keypad[0xE] = true; break;

                    case SDLK_z: chip8->keypad[0xA] = true; break;
                    case SDLK_x: chip8->keypad[0x0] = true; break;
                    case SDLK_c: chip8->keypad[0xB] = true; break;
                    case SDLK_v: chip8->keypad[0xF] = true; break;

                    default: break;
                        
                }
                break; 

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    // Map qwerty keys to CHIP8 keypad
                    case SDLK_1: chip8->keypad[0x1] = false; break;
                    case SDLK_2: chip8->keypad[0x2] = false; break;
                    case SDLK_3: chip8->keypad[0x3] = false; break;
                    case SDLK_4: chip8->keypad[0xC] = false; break;

                    case SDLK_q: chip8->keypad[0x4] = false; break;
                    case SDLK_w: chip8->keypad[0x5] = false; break;
                    case SDLK_e: chip8->keypad[0x6] = false; break;
                    case SDLK_r: chip8->keypad[0xD] = false; break;

                    case SDLK_a: chip8->keypad[0x7] = false; break;
                    case SDLK_s: chip8->keypad[0x8] = false; break;
                    case SDLK_d: chip8->keypad[0x9] = false; break;
                    case SDLK_f: chip8->keypad[0xE] = false; break;

                    case SDLK_z: chip8->keypad[0xA] = false; break;
                    case SDLK_x: chip8->keypad[0x0] = false; break;
                    case SDLK_c: chip8->keypad[0xB] = false; break;
                    case SDLK_v: chip8->keypad[0xF] = false; break;

                    default: break;
                }
                break;

            default:
                break;
        }
    }
}

// Update CHIP8 delay and sound timers every 60hz
void update_timers(const sdl_t sdl, Chip8 *chip8) {
    if (chip8->delay_timer > 0) 
        chip8->delay_timer--;

    if (chip8->sound_timer > 0) {
        chip8->sound_timer--;
        SDL_PauseAudioDevice(sdl.dev, 0); // Play sound
    } else {
        SDL_PauseAudioDevice(sdl.dev, 1); // Pause sound
    }
}

// Final cleanup
void final_cleanup(const sdl_t sdl) {
    SDL_DestroyRenderer(sdl.renderer);
    SDL_DestroyWindow(sdl.window);
    SDL_CloseAudioDevice(sdl.dev);
    SDL_Quit(); // Shut down SDL subsystem
}

int main(int argc, char *argv[]) {
    // Default usage message for args
    if (argc < 2) {
       fprintf(stderr, "Usage: %s <rom_name>\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    // Initialize emulator state
    emu_state_t state = RUNNING;

    // Initialize emulator configuration/options
    config_t config = {0};
    init_config(&config);

    // Initialize SDL
    sdl_t sdl = {0};
    if (!init_sdl(&sdl, &config))
        exit(EXIT_FAILURE);

    // Initialize CHIP8 machine
    Chip8 chip8;
    const char *file_path = argv[1];
    if (!chip8.init_chip8(&config, file_path))
        exit(EXIT_FAILURE);

    // Initial screen clear to background color
    clear_screen(sdl, config);

    // Seed random number generator for a chip-8 inst
    srand(time(NULL));

    // Main emulator loop
    while (state != QUIT) {
        // Handle user input
        handle_input(&chip8, &config, &state);

        if (state == PAUSED) continue;

        // Get time before running instructions 
        const uint64_t start_frame_time = SDL_GetPerformanceCounter();
        
        // Emulate CHIP8 Instructions for this emulator "frame" (60hz)
        for (uint32_t i = 0; i < config.insts_per_second / 60; i++)
            chip8.emulate_inst(config);

        // Get time elapsed after running instructions
        const uint64_t end_frame_time = SDL_GetPerformanceCounter();

        // Delay for approximately 60hz/60fps (16.67ms) or actual time elapsed
        const double time_elapsed = (double) ((end_frame_time - start_frame_time) * 1000) / SDL_GetPerformanceFrequency();

        SDL_Delay(16.67f > time_elapsed ? 16.67f - time_elapsed : 0);

        // Update window with changes every 60hz
        if (chip8.draw) {
          update_screen(sdl, config, &chip8);
          chip8.draw = false;
        }
        
        // Update delay & sound timers every 60hz
        update_timers(sdl, &chip8);
    }

    // Final cleanup
    final_cleanup(sdl); 

    exit(EXIT_SUCCESS);
}