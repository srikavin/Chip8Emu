#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <Cpu.h>
#include "gtest/gtest.h"

TEST(CPUTest, Construction) {
    auto memory = Memory();
    auto graphics = Graphics(memory);
    auto input = Input();
    Cpu cpu(memory, graphics, input, 0x200);

    EXPECT_EQ(cpu.pc, 0x200);
    cpu.step();
    EXPECT_EQ(cpu.pc, 0x202);
}

TEST(CPUTest, OPCODE_00E0) {
    auto memory = Memory();
    auto graphics = Graphics(memory);
    auto input = Input();
    Cpu cpu(memory, graphics, input, 0x200);

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
    auto input = Input();
    Cpu cpu(memory, graphics, input, 0x200);

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
    auto input = Input();
    Cpu cpu(memory, graphics, input, 0x200);

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
    auto input = Input();
    Cpu cpu(memory, graphics, input, 0x200);

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

TEST(CpuTest, ARITHMETIC) {
    auto memory = Memory();
    auto graphics = Graphics(memory);
    auto input = Input();
    Cpu cpu(memory, graphics, input, 0x200);

    EXPECT_EQ(cpu.pc, 0x200);

    // 0x7[1][05] - Add the value 0x5 to V1
    memory[0x200] = 0x71;
    memory[0x201] = 0x05;
    cpu.data_registers[1] = 0x0;

    cpu.step();
    EXPECT_EQ(cpu.data_registers[1], 0x05);

    // 0x8[1][2]0 - Set V1 = V2
    memory[0x202] = 0x81;
    memory[0x203] = 0x20;
    cpu.data_registers[1] = 5;
    cpu.data_registers[2] = 3;

    cpu.step();
    EXPECT_EQ(cpu.data_registers[1], 0x03);

    // 0x8[5][1]1 - Set V5 |= V1
    memory[0x204] = 0x85;
    memory[0x205] = 0x11;
    cpu.data_registers[5] = 9;
    cpu.data_registers[1] = 1;

    cpu.step();
    EXPECT_EQ(cpu.data_registers[5], 9u | 1u);

    // 0x8[5][1]2 - Set V5 &= V1
    memory[0x206] = 0x85;
    memory[0x207] = 0x12;
    cpu.data_registers[5] = 9;
    cpu.data_registers[1] = 7;

    cpu.step();
    EXPECT_EQ(cpu.data_registers[5], 9u & 7u);

    // 0x8[5][1]3 - Set V5 ^= V1
    memory[0x208] = 0x85;
    memory[0x209] = 0x13;
    cpu.data_registers[5] = 9;
    cpu.data_registers[1] = 7;

    cpu.step();
    EXPECT_EQ(cpu.data_registers[5], 9u ^ 7u);

    // 0x8[5][1]4 - Set V1 += V5; Set VF is carry occurs
    memory[0x20A] = 0x85;
    memory[0x20B] = 0x14;
    cpu.data_registers[5] = 250;
    cpu.data_registers[1] = 100;

    cpu.step();
    EXPECT_EQ(cpu.data_registers[5], (250 + 100) % (0xFF + 1));
    EXPECT_EQ(cpu.data_registers[0xF], 1);

    // 0x8[5][1]5 - Set VX -= VY; Set VF is carry occurs
    memory[0x20C] = 0x85;
    memory[0x20D] = 0x15;
    cpu.data_registers[5] = 100;
    cpu.data_registers[1] = 250;

    cpu.step();
    EXPECT_EQ(cpu.data_registers[5], (uint8_t) ((uint8_t) 100 - (uint8_t) 250));
    EXPECT_EQ(cpu.data_registers[0xF], 0);

    // 0x8[5][1]6 - Store V1 << 1 in V5. Set VF to lowest bit of V1
    memory[0x20E] = 0x85;
    memory[0x20F] = 0x16;
    cpu.data_registers[5] = 4;
    cpu.data_registers[1] = 1;

    cpu.step();
    EXPECT_EQ(cpu.data_registers[5], 1u << 1u);
    EXPECT_EQ(cpu.data_registers[0xF], 1);

    // 0x8[5][1]7 - VX = VY - VX; Set VF if borrow does not occur
    memory[0x210] = 0x85;
    memory[0x211] = 0x17;
    cpu.data_registers[5] = 170;
    cpu.data_registers[1] = 200;

    cpu.step();
    EXPECT_EQ(cpu.data_registers[5], 200u - 170u);
    EXPECT_EQ(cpu.data_registers[0xF], 1);

    // 0x8[5][1]7 - VX = VY - VX; Set VF if borrow does not occur
    memory[0x212] = 0x85;
    memory[0x213] = 0x17;
    cpu.data_registers[5] = 200;
    cpu.data_registers[1] = 170;

    cpu.step();
    EXPECT_EQ(cpu.data_registers[5],  (uint8_t) ((uint8_t) 170 - (uint8_t) 200));
    EXPECT_EQ(cpu.data_registers[0xF], 0);
}
