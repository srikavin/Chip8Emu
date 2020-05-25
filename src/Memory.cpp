#include "Memory.h"

Memory::Memory() {
    for (auto &i : this->memory) {
        i = 0;
    }
};

uint8_t &Memory::operator[](uint8_t addr) {
    return this->memory[addr];
}
