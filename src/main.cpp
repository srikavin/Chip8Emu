#include <iostream>
#include "Cpu.h"
#include "Memory.h"

int main() {
    auto memory = Memory();
    auto graphics = Graphics(memory);
    Cpu cpu(memory, graphics, 0x200);
    cpu.step();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
