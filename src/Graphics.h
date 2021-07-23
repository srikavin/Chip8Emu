#pragma once

#include <cstdint>
#include "Memory.h"

class Graphics {
public:
    Graphics(Memory& memory);

    Memory& memory;

    /**
     * Returns true if the buffer should be re-rendered
     */
    bool isDirty() const;

    /**
     * Sets the dirty flag to true
     */
    void setDirty();

    /**
     * Sets the dirty flag to false
     */
    void clearDirty();

    /**
     * Clears the entire screen
     */
    void clear();

    void set(uint16_t x, uint16_t y, uint8_t val);
    uint8_t get(uint16_t x, uint16_t y);

    uint32_t buffer[64 * 32];

private:
    bool dirty;
};
