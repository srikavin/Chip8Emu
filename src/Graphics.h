#pragma once

#include <cstdint>
#include "Memory.h"

class Graphics {
public:
    static const uint8_t font_data[16][5];

    Graphics(Memory& memory);

    uint16_t font_data_addr[16];
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

    uint8_t buffer[64 * 32];

private:
    bool dirty;
};
