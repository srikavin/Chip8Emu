#include <iostream>
#include <fstream>
#include <iterator>
#include <unordered_map>

#include "Memory.h"
#include "Cpu.h"
#include "SDL2/SDL.h"

static std::unordered_map<SDL_Keycode, uint8_t> keymap = {
        {SDLK_1, 0},
        {SDLK_2, 1},
        {SDLK_3, 2},
        {SDLK_4, 3},

        {SDLK_q, 4},
        {SDLK_w, 5},
        {SDLK_e, 6},
        {SDLK_r, 7},

        {SDLK_a, 8},
        {SDLK_s, 9},
        {SDLK_d, 10},
        {SDLK_f, 11},

        {SDLK_q, 12},
        {SDLK_w, 13},
        {SDLK_q, 14},
        {SDLK_w, 15}
};

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s [rom-file]", argv[1]);
        return 0;
    }

    FILE *rom = std::fopen(argv[1], "r");

    if (rom == nullptr) {
        printf("%s could not be loaded!", argv[1]);
        return 0;
    }

    uint8_t buffer[4096 - 0x200];
    std::fread(buffer, 4096 - 0x200, sizeof(uint8_t), rom);

    auto memory = Memory();

    for (int i = 0; i < 4096 - 0x200; ++i) {
        memory.memory[0x200 + i] = buffer[i];
        printf("%x ", buffer[i]);
    }

    auto graphics = Graphics(memory);
    auto input = Input();
    Cpu cpu(memory, graphics, input, 0x200);

    for (int i = 0; i < 100; i++) {
        cpu.step();
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Chip 8 Emulator", 0, 0,
                                          1024, 512, SDL_WINDOW_SHOWN);
    SDL_SetWindowBordered(window, SDL_TRUE);

    if (window == nullptr) {
        printf("SDL failed to create window: %s\n", SDL_GetError());
        return 2;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, 1024, 512);

    SDL_Texture *sdlTexture = SDL_CreateTexture(renderer,
                                                SDL_PIXELFORMAT_ARGB8888,
                                                SDL_TEXTUREACCESS_STREAMING,
                                                64, 32);


    SDL_Event event;
    while (true) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }

        switch (event.type) {
            case SDL_KEYDOWN:
                if (keymap.find(event.key.keysym.sym) != keymap.end()) {
                    input.onKeyDown(keymap[event.key.keysym.sym]);
                }
                break;
            case SDL_KEYUP:
                if (keymap.find(event.key.keysym.sym) != keymap.end()) {
                    input.onKeyUp(keymap[event.key.keysym.sym]);
                }
                break;
            default:
                break;
        }

        if (graphics.isDirty()) {
            SDL_UpdateTexture(sdlTexture, nullptr, graphics.buffer, 64 * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sdlTexture, nullptr, nullptr);

            SDL_RenderPresent(renderer);

            graphics.clearDirty();
        }

        cpu.step();
        SDL_Delay(2);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);


    return 0;
}
