#include <iostream>
#include <fstream>
#include <iterator>
#include "Cpu.h"
#include "Memory.h"
#include "SDL2/SDL.h"

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
    Cpu cpu(memory, graphics, 0x200);

    for (int i = 0; i < 100; i++) {
        cpu.step();
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Chip 8 Emulator", 0, 0,
                                          1024, 512, SDL_WINDOW_SHOWN);

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


    while (true) {
        SDL_UpdateTexture(sdlTexture, nullptr, graphics.buffer, 64 * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, sdlTexture, nullptr, nullptr);

        SDL_RenderPresent(renderer);

        cpu.step();
        SDL_Delay(15);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);


    return 0;
}
