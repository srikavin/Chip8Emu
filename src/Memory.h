#pragma once

#include <cinttypes>

class Memory {
public:
    Memory();
    uint8_t memory[4096]{};

    uint8_t& operator[] (uint8_t addr);
};
