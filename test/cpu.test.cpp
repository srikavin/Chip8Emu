#include <Cpu.h>
#include "gtest/gtest.h"

TEST(CPUTest, Construction) {
    auto memory = Memory();
    auto graphics = Graphics(memory);
    Cpu cpu(memory, graphics, 0x200);

    EXPECT_EQ(cpu.pc, 0x200);
    cpu.step();
    EXPECT_EQ(cpu.pc, 0x202);
}

TEST(CPUTest, OPCODE_00E0) {
    auto memory = Memory();
    auto graphics = Graphics(memory);
    Cpu cpu(memory, graphics, 0x200);

    // 0x00E0
    memory[0x200] = 0x00;
    memory[0x201] = 0xE0;

    graphics.set(10, 10, 1);
    EXPECT_EQ(graphics.get(10, 10), 1);

    cpu.step();

    EXPECT_EQ(graphics.get(10, 10), 0);
    EXPECT_EQ(graphics.get(1, 5), 0);
}

TEST(CPUTest, ROUTINES) {
    auto memory = Memory();
    auto graphics = Graphics(memory);
    Cpu cpu(memory, graphics, 0x200);

    // 0x2205 - Execute subroutine at 0x208
    memory[0x200] = 0x22;
    memory[0x201] = 0x08;

    cpu.step();

    EXPECT_EQ(cpu.pc, 0x208);


    // 0x00EE - Return from subroutine
    memory[0x208] = 0x00;
    memory[0x209] = 0xEE;

    cpu.step();

    EXPECT_EQ(cpu.pc, 0x202);
}

TEST(CPUTest, JUMPS) {
    auto memory = Memory();
    auto graphics = Graphics(memory);
    Cpu cpu(memory, graphics, 0x200);

    // 0x1[210] - Jump to address 0x210
    memory[0x200] = 0x12;
    memory[0x201] = 0x10;

    // 0xB[210] - Jump to address 0x216 + V0
    memory[0x210] = 0xB2;
    memory[0x211] = 0x16;

    cpu.data_registers[0] = 10;

    EXPECT_EQ(cpu.pc, 0x200);

    cpu.step();

    EXPECT_EQ(cpu.pc, 0x210);

    cpu.step();

    EXPECT_EQ(cpu.pc, 0x216 + 10);
}

