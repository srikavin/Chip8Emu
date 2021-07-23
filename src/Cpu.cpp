#include "Cpu.h"
#include "Graphics.h"
#include <iostream>

static inline uint8_t getNN(uint16_t opcode) {
    return opcode & 0x00FFu;
}

static inline uint16_t getNNN(uint16_t opcode) {
    return opcode & 0x0FFFu;
}

static inline uint8_t getX(uint16_t opcode) {
    return (opcode & 0x0F00u) >> 8u;
}

static inline uint8_t getY(uint16_t opcode) {
    return (opcode & 0x00F0u) >> 4u;
}

Cpu::Cpu(Memory &memory, Graphics &graphics, Input &input, int starting_addr) : memory(memory), graphics(graphics), input(input) {
    this->pc = starting_addr;
    this->instruction_register = 0;
    this->stack = std::stack<uint16_t>();

    std::random_device dev;
    this->rng = std::mt19937(dev());
    this->rng_dist = std::uniform_int_distribution<std::mt19937::result_type>(0, 0xFF);

    this->skip_update_pc = false;
}

void Cpu::step() {
    if (!(this->pc < 4096 && this->pc >= 512)) {
        std::cerr << "PC register is out of bounds: " << std::hex << this->pc << std::endl;
        exit(2);
    }

    if (waiting_for_key) {
        if (input.triggered()) {
            data_registers[waiting_for_key_reg] = input.triggeredKey();
            waiting_for_key = false;
        }
        return;
    }

    // instructions are stores in big-endian format
    uint16_t inst = ((uint16_t) (this->memory[this->pc] << 8u)) | this->memory[this->pc + 1];
    std::cerr << "Running opcode: " << std::hex << inst << " " << std::hex << this->pc << " " << std::hex
              << ((inst & 0xF000u) >> 12u) << std::endl;

    switch ((inst & 0xF000u) >> 12u) {
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
        case 0xE:
            this->opcode_Exxx(inst);
            break;
        case 0xF:
            this->opcode_Fxxx(inst);
            break;
        default:
            std::cout << std::hex << this->pc << std::endl;
            this->pc += 2;
            break;
    }

    if (this->skip_update_pc) {
        this->skip_update_pc = false;
        return;
    }

    this->pc += 2;
}

inline void Cpu::opcode_0xxx(uint16_t opcode) {
    if (opcode == 0x00E0) {
        // 00E0 - Clear the screen
        this->graphics.clear();
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
    this->pc = getNNN(opcode);
    this->skip_update_pc = true;
}

inline void Cpu::opcode_2xxx(uint16_t opcode) {
    // 2NNN - Execute subroutine at NNN
    this->stack.push(this->pc);
    this->pc = getNNN(opcode);
    this->skip_update_pc = true;
}

inline void Cpu::opcode_3xxx(uint16_t opcode) {
    // 3XNN - Skip the following instruction if register VX equals NN
    uint8_t reg = getX(opcode);
    uint16_t val = getNN(opcode);

    if (this->data_registers[reg] == val) {
        this->pc += 2;
    }
}

inline void Cpu::opcode_4xxx(uint16_t opcode) {
    // 4XNN - Skip the following instruction if register VX does not equals NN
    uint8_t reg = getX(opcode);
    uint8_t val = getNN(opcode);

    if (this->data_registers[reg] != val) {
        this->pc += 2;
    }
}

inline void Cpu::opcode_5xxx(uint16_t opcode) {
    // 5XY0 - Skip the following instruction if VX equals XY
    uint8_t regx = getX(opcode);
    uint8_t regy = getY(opcode);

    if (data_registers[regx] == data_registers[regy]) {
        this->pc += 2;
    }
}

inline void Cpu::opcode_6xxx(uint16_t opcode) {
    // 6XNN Store number NN in register VX

    uint8_t data = getNN(opcode);
    uint8_t reg = getX(opcode);

    data_registers[reg] = data;
}

inline void Cpu::opcode_7xxx(uint16_t opcode) {
    // 7XNN - Add NN to VX
    uint8_t reg = getX(opcode);
    uint8_t data = getNN(opcode);

    data_registers[reg] += data;
}

inline void Cpu::opcode_8xxx(uint16_t opcode) {
    uint8_t regx = getX(opcode);
    uint8_t regy = getY(opcode);

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
    uint8_t regx = getX(opcode);
    uint8_t regy = getY(opcode);

    if (data_registers[regx] != data_registers[regy]) {
        this->pc += 2;
    }
}

inline void Cpu::opcode_Axxx(uint16_t opcode) {
    // ANNN - Store memory address NNN in register I
    this->instruction_register = opcode & 0x0FFFu;
}

inline void Cpu::opcode_Bxxx(uint16_t opcode) {
    // BNNN - Jump to NNN + V0
    this->pc = getNNN(opcode) + data_registers[0];
    this->skip_update_pc = true;
}

inline void Cpu::opcode_Cxxx(uint16_t opcode) {
    // CXNN - Set VX to a random number with mask NN
    uint8_t mask = getNN(opcode);
    uint8_t regx = getX(opcode);

    uint16_t number = rng_dist(rng);

    data_registers[regx] = number & mask;
}

void Cpu::opcode_Dxxx(uint16_t opcode) {
    // DXYN - Draw a sprite at (VX, VY) with N bytes of sprite data from VI
    // Set VF to 1 if any set pixels are unset
    // Each byte has 8 bits indicating the value of the pixel

    uint8_t regx = getX(opcode);
    uint8_t regy = getY(opcode);

    uint8_t x0 = data_registers[regx];
    uint8_t y0 = data_registers[regy];

    uint8_t n = (opcode & 0x000Fu);

    data_registers[0xf] = 0;
    for (int y = 0; y < n; ++y) {
        uint8_t cur = memory[instruction_register + y];
        for (int x = 0; x < 8; ++x) {
            uint8_t val = ((cur) & (0x80 >> x)) ? 1 : 0;

            uint8_t oldVal = graphics.get(x0 + x, y0 + y);

            if (oldVal == 0) {
                data_registers[0xf] = 1;
            }

            graphics.set(x0 + x, y0 + y, val);
        }
    }
}

inline void Cpu::opcode_Exxx(uint16_t opcode) {
    uint8_t regx = getX(opcode);

    switch (opcode & (unsigned) 0x00FF) {
        case 0x9E:
            // EX9E - Skip the next instruction if the key in VX is pressed
            if (input.keys[data_registers[regx]]) {
                this->pc += 2;
            }
            break;
        case 0xA1:
            // EXA1 - Skip the next instruction if the key in VX is not pressed
            if (!input.keys[data_registers[regx]]) {
                this->pc += 2;
            }
            break;
    }
}

void Cpu::opcode_Fxxx(uint16_t opcode) {
    uint8_t regx = getX(opcode);

    switch (opcode & (unsigned) 0x00FF) {
        case 0x07:
            // FX07 - Store the current value of the delay timer in VX
            data_registers[regx] = delay_timer;
            break;
        case 0x0A:
            // FX0A - Wait for a keypress and store it in VX
            input.clearTriggered();
            waiting_for_key = true;
            waiting_for_key_reg = regx;
            break;
        case 0x15:
            // FX15 - Set delay timer to VX
            // TODO:
            break;
        case 0x18:
            // FX18 - Set sound timer to VX
            // TODO:
            break;
        case 0x1E:
            // FX15 - Add VX to I.
            instruction_register += data_registers[regx];
            break;
        case 0x29:
            // FX29 - Sets I to location of character sprite in VX
            instruction_register = regx * 0x5;
            break;
        case 0x33:
            // FX33 - Stores BCD representation of VX at I
            memory[instruction_register] = data_registers[regx] / 100;
            memory[instruction_register + 1] = (data_registers[regx] / 10) % 10;
            memory[instruction_register + 2] = data_registers[regx] % 10;
            break;
        case 0x55:
            // FX55 - Stores V0 to Vx in memory starting at I
            for (uint8_t i = 0; i < regx + 1; ++i) {
                memory[instruction_register + i] = data_registers[i];
            }
            break;
        case 0x65:
            // FX65 - Fills V0 to VX from memory starting at I
            for (uint8_t i = 0; i < regx + 1; ++i) {
                data_registers[i] = memory[instruction_register + i];
            }
            break;
    }
}
