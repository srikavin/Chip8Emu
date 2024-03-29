#pragma once

#include <cinttypes>
#include <stack>
#include "Memory.h"
#include "Graphics.h"
#include "Input.h"
#include <random>
#include <set>

enum State {

};

class Cpu {
public:
    Cpu(Memory &memory, Graphics &graphics, Input &input, int starting_addr);
    void step();

    /**
     * The program counter register
     */
    uint16_t pc;

    uint8_t data_registers[16]{};
    uint16_t instruction_register;
private:
    Memory& memory;
    Graphics& graphics;
    Input& input;
    std::stack<uint16_t> stack;

    void opcode_0xxx(uint16_t opcode);
    void opcode_1xxx(uint16_t opcode);
    void opcode_2xxx(uint16_t opcode);
    void opcode_3xxx(uint16_t opcode);
    void opcode_4xxx(uint16_t opcode);
    void opcode_5xxx(uint16_t opcode);
    void opcode_6xxx(uint16_t opcode);
    void opcode_7xxx(uint16_t opcode);
    void opcode_8xxx(uint16_t opcode);
    void opcode_9xxx(uint16_t opcode);
    void opcode_Axxx(uint16_t opcode);
    void opcode_Bxxx(uint16_t opcode);
    void opcode_Cxxx(uint16_t opcode);
    void opcode_Dxxx(uint16_t opcode);
    void opcode_Exxx(uint16_t opcode);
    void opcode_Fxxx(uint16_t opcode);


    /**
     * Indicates if pc should be updated at the end of the cycle.
     * Should be set to false if a jump has occurred.
     */
    bool skip_update_pc;

    std::mt19937 rng;
    std::uniform_int_distribution<std::mt19937::result_type> rng_dist; // distribution in range [1, 6]

    bool waiting_for_key;
    uint8_t waiting_for_key_reg;

    uint8_t delay_timer;
    uint8_t sound_timer;

};
