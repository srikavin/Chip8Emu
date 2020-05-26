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