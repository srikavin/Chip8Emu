#include "Memory.h"

Memory::Memory() {
    for (auto &i : this->memory) {
        i = 0;
    }
};

uint8_t &Memory::operator[](uint16_t addr) {
    return this->memory[addr];
}
