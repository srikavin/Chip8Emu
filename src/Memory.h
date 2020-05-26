#pragma once

#include <cinttypes>

class Memory {
public:
    Memory();
    uint8_t memory[4096]{0};

    uint8_t& operator[] (uint16_t addr);
};
