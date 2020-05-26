#include "Cpu.h"
#include "Graphics.h"
#include <iostream>

Cpu::Cpu(Memory &memory, Graphics &graphics, int starting_addr) : memory(memory), graphics(graphics) {
    this->pc = starting_addr;
    this->instruction_register = 0;
    this->stack = std::stack<uint16_t>();

    std::random_device dev;
    this->rng = std::mt19937(dev());
    this->rng_dist = std::uniform_int_distribution<std::mt19937::result_type>(0, 0xFF);

    this->keys_pressed = std::set<uint8_t>();
}

void Cpu::step() {
    // instructions are stores in big-endian format
    uint16_t inst = ((unsigned) this->memory[this->pc] << (unsigned) 4) | this->memory[this->pc + 1];

    switch (inst & (uint16_t) 0xF000) {
        case 0:
            this->opcode_0xxx(inst);
            break;
        case 1:
            this->opcode_1xxx(inst);
            break;
        case 2:
            this->opcode_2xxx(inst);
            break;
        case 3:
            this->opcode_3xxx(inst);
            break;
        case 4:
            this->opcode_4xxx(inst);
            break;
        case 5:
            this->opcode_5xxx(inst);
            break;
        case 6:
            this->opcode_6xxx(inst);
            break;
        case 7:
            this->opcode_7xxx(inst);
            break;
        case 8:
            this->opcode_8xxx(inst);
            break;
        case 9:
            this->opcode_9xxx(inst);
            break;
        case 0xA:
            this->opcode_Axxx(inst);
            break;
        case 0xB:
            this->opcode_Bxxx(inst);
            break;
        case 0xC:
            this->opcode_Cxxx(inst);
            break;
        case 0xD:
            this->opcode_Dxxx(inst);
            break;
    }

    if (this->update_pc) {
        this->update_pc = false;
        return;
    }

    this->pc += 2;
}

inline void Cpu::opcode_0xxx(uint16_t opcode) {
    if (opcode == 0x00E0) {
        // 00E0 - Clear the screen
        this->graphics.clear();
        this->graphics.setDirty();
    } else if (opcode == 0x00EE) {
        // 00EE - Return from subroutine
        this->pc = this->stack.top();
        this->stack.pop();
    } else {
        std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
    }
}

inline void Cpu::opcode_1xxx(uint16_t opcode) {
    // 1NNN - Jump to address NNN
    this->pc = opcode & (uint16_t) 0x0FFF;
    this->update_pc = false;
}

inline void Cpu::opcode_2xxx(uint16_t opcode) {
    // 2NNN - Execute subroutine at NNN
    this->stack.push(this->pc);
    this->pc = opcode & (uint16_t) 0x0FFF;
    this->update_pc = false;
}

inline void Cpu::opcode_3xxx(uint16_t opcode) {
    // 3XNN - Skip the following instruction if register VX equals NN
    uint8_t reg = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;
    uint16_t val = opcode & (uint16_t) 0x00FF;
    if (this->data_registers[reg] == val) {
        this->pc += 2;
    }
}

inline void Cpu::opcode_4xxx(uint16_t opcode) {
    // 4XNN - Skip the following instruction if register VX does not equals NN
    uint8_t reg = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;
    uint8_t val = opcode & (uint16_t) 0x00FF;
    if (this->data_registers[reg] != val) {
        this->pc += 2;
    }
}

inline void Cpu::opcode_5xxx(uint16_t opcode) {
    // 5XY0 - Skip the following instruction if VX equals XY
    uint8_t regx = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;
    uint8_t regy = (opcode & (unsigned) 0x00F0) >> (unsigned) 4;

    if (data_registers[regx] == data_registers[regy]) {
        this->pc += 2;
    }
}

inline void Cpu::opcode_6xxx(uint16_t opcode) {
    // 6XNN Store number NN in register VX

    uint8_t data = opcode & (uint16_t) 0x00FF;
    uint8_t reg = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;

    data_registers[reg] = data;
}

inline void Cpu::opcode_7xxx(uint16_t opcode) {
    // 7XNN - Add NN to VX
    uint8_t reg = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;
    uint8_t data = opcode & (uint16_t) 0x00FF;

    data_registers[reg] += data;
}

inline void Cpu::opcode_8xxx(uint16_t opcode) {
    uint8_t regx = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;
    uint8_t regy = (opcode & (unsigned) 0x00F0) >> (unsigned) 4;

    switch (opcode & (unsigned) 0x000F) {
        case 0:
            // 8XY0 - Store VY in VX
            data_registers[regx] = data_registers[regy];
            break;
        case 1:
            // 8XY1 - VX = VX | VY
            data_registers[regx] |= data_registers[regy];
            break;
        case 2:
            // 8XY2 - VX = VX & VY
            data_registers[regx] &= data_registers[regy];
            break;
        case 3:
            // 8XY3 - VX = VX ^ VY
            data_registers[regx] ^= data_registers[regy];
            break;
        case 4:
            // VX = VX + VY; Set VF to 0 if carry occurs
            if ((int) data_registers[regx] + (int) data_registers[regy] > 0xFF) {
                // carry
                data_registers[0xF] = 0x01;
            } else {
                data_registers[0xF] = 0x00;
            }
            data_registers[regx] += data_registers[regy];
            break;
        case 5:
            // VX = VX - VY; Set VF to 0 if borrow occurs
            if ((int) data_registers[regx] - (int) data_registers[regy] < 0) {
                // borrow
                data_registers[0xF] = 0x00;
            } else {
                data_registers[0xF] = 0x01;
            }
            data_registers[regx] -= data_registers[regy];
            break;
        case 6:
            // VX = VY >> 1; VF = VY &0x1;
            data_registers[regx] = data_registers[regx] >> (unsigned) 1;
            data_registers[0xf] = data_registers[regy] & (unsigned) 0x1;
            break;
        case 7:
            // VX = VY - VX; Set VF to 0 if borrow occurs
            if ((int) data_registers[regy] - (int) data_registers[regx] < 0) {
                // borrow
                data_registers[0xF] = 0x00;
            } else {
                data_registers[0xF] = 0x01;
            }
            data_registers[regx] = data_registers[regy] - data_registers[regx];
            break;
        case 0xE:
            // VX = VY << 1; VF = VY &0x1;
            data_registers[regx] = data_registers[regx] << (unsigned) 1;
            data_registers[0xf] = data_registers[regy] >> (unsigned) 7;
            break;
    }
}

inline void Cpu::opcode_9xxx(uint16_t opcode) {
    // 9XY0 - Skip next instruction if VX is not equal to VY
    uint8_t regx = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;
    uint8_t regy = (opcode & (unsigned) 0x00F0) >> (unsigned) 4;

    if (data_registers[regx] != data_registers[regy]) {
        this->pc += 2;
    }
}

inline void Cpu::opcode_Axxx(uint16_t opcode) {
    // ANNN - Store memory address NNN in register I
    this->instruction_register = opcode & (unsigned) 0x0FFF;
}

inline void Cpu::opcode_Bxxx(uint16_t opcode) {
    // BNNN - Jump to NNN + V0
    this->pc = (opcode & (unsigned) 0x0FFF) + data_registers[0];
}

inline void Cpu::opcode_Cxxx(uint16_t opcode) {
    // CXNN - Set VX to a random number with mask NN
    uint8_t mask = opcode & (unsigned) 0x00FF;
    uint8_t regx = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;

    uint16_t number = rng_dist(rng);

    data_registers[regx] = number & mask;
}

void Cpu::opcode_Dxxx(uint16_t opcode) {
    // DXYN - Draw a sprite at (VX, VY) with N bytes of sprite data from VI
    // Set VF to 1 if any set pixels are unset
    // Each byte has 8 bits indicating the value of the pixel

    uint8_t regx = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;
    uint8_t regy = (opcode & (unsigned) 0x00F0) >> (unsigned) 4;

    uint8_t x0 = data_registers[regx];
    uint8_t y0 = data_registers[regy];

    uint8_t n = (opcode & (unsigned) 0x000F);

    for (int y = 0; y < n; ++y) {
        for (unsigned int x = 0; x < 8; x++) {
            uint16_t cur = instruction_register + y;
            uint8_t val = (cur >> x);

            uint8_t oldVal = graphics.get(x0 + x, y0 + y);

            if (oldVal != 0) {
                data_registers[0xf] = 1;
            }

            graphics.set(x0 + x, y0 + y, val ^ oldVal);
        }
    }
}

inline void Cpu::opcode_Exxx(uint16_t opcode) {
    uint8_t regx = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;

    switch (opcode & (unsigned) 0x00FF) {
        case 0x9E:
            // EX9E - Skip the next instruction if the key in VX is pressed
            if (keys_pressed.find(data_registers[regx]) != keys_pressed.end()) {
                this->pc += 2;
            }
            break;
        case 0xA1:
            // EXA1 - Skip the next instruction if the key in VX is not pressed
            if (keys_pressed.find(data_registers[regx]) == keys_pressed.end()) {
                this->pc += 2;
            }
            break;
    }
}

void Cpu::opcode_Fxxx(uint16_t opcode) {
    uint8_t regx = (opcode & (unsigned) 0x0F00) >> (unsigned) 8;

    switch (opcode & (unsigned) 0x00FF) {
        case 0x07:
            // FX07 - Store the current value of the delay timer in VX
            data_registers[regx] = delay_timer;
            break;
        case 0x0A:
            // FX0A - Wait for a keypress and store it in VX
            break;
    }
}








