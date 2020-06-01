#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

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

TEST(CPUTest, CONDITIONALS) {
    auto memory = Memory();
    auto graphics = Graphics(memory);
    Cpu cpu(memory, graphics, 0x200);

    EXPECT_EQ(cpu.pc, 0x200);

    // 0x3[0][12] - Skip the next instruction if V0 equals 0x12
    memory[0x200] = 0x30;
    memory[0x201] = 0x12;
    cpu.data_registers[0] = 0x12;

    cpu.step();
    EXPECT_EQ(cpu.pc, 0x204);

    // 0x3[1][12] - Skip the next instruction if V1 equals 12
    memory[0x204] = 0x31;
    memory[0x205] = 0x12;
    cpu.data_registers[1] = 0x5;

    cpu.step();
    EXPECT_EQ(cpu.pc, 0x206);

    // 0x4[1][03] - Skip the next instruction if V1 is not equal to 3
    memory[0x206] = 0x41;
    memory[0x207] = 0x03;
    cpu.data_registers[1] = 0x5;

    cpu.step();
    EXPECT_EQ(cpu.pc, 0x20A);

    // 0x4[1][03] - Skip the next instruction if V1 is not equal to 3
    memory[0x208] = 0x41;
    memory[0x209] = 0x03;
    cpu.data_registers[1] = 0x3;

    cpu.step();
    EXPECT_EQ(cpu.pc, 0x20C);

    // 0x5[4][5]0 - Skip the next instruction if V4 is equal to V5
    memory[0x20C] = 0x54;
    memory[0x20D] = 0x50;
    cpu.data_registers[5] = 0x2;
    cpu.data_registers[4] = 0x2;

    cpu.step();
    EXPECT_EQ(cpu.pc, 0x210);

    // 0x5[6][5]0 - Skip the next instruction if V6 is equal to V5
    memory[0x210] = 0x56;
    memory[0x211] = 0x50;
    cpu.data_registers[6] = 0x9;
    cpu.data_registers[5] = 0x7;

    cpu.step();
    EXPECT_EQ(cpu.pc, 0x212);

    // 0x9[4][5]0 - Skip the next instruction if V4 is not equal to V5
    memory[0x212] = 0x94;
    memory[0x213] = 0x50;
    cpu.data_registers[5] = 0x2;
    cpu.data_registers[4] = 0x3;

    cpu.step();
    EXPECT_EQ(cpu.pc, 0x216);

    // 0x9[6][5]0 - Skip the next instruction if V6 is not equal to V5
    memory[0x216] = 0x96;
    memory[0x217] = 0x50;
    cpu.data_registers[6] = 0x7;
    cpu.data_registers[5] = 0x7;

    cpu.step();
    EXPECT_EQ(cpu.pc, 0x218);
}