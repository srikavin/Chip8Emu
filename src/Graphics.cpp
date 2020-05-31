#include "Graphics.h"

const uint32_t UNSET_VAL = 0xFF'00'00'00;
const uint32_t SET_VAL = 0xFF'FF'FF'FF;

const uint8_t Graphics::font_data[16][5] = {
        {0xF0, 0x90, 0x90, 0x90, 0xF0},
        {0x20, 0x60, 0x20, 0x20, 0x70},
        {0xF0, 0x10, 0xF0, 0x80, 0xF0},
        {0xF0, 0x10, 0xF0, 0x10, 0xF0},
        {0x90, 0x90, 0xF0, 0x10, 0x10},
        {0xF0, 0x80, 0xF0, 0x10, 0xF0},
        {0xF0, 0x80, 0xF0, 0x90, 0xF0},
        {0xF0, 0x10, 0x20, 0x40, 0x40},
        {0xF0, 0x90, 0xF0, 0x90, 0xF0},
        {0xF0, 0x90, 0xF0, 0x10, 0xF0},
        {0xF0, 0x90, 0xF0, 0x90, 0x90},
        {0xE0, 0x90, 0xE0, 0x90, 0xE0},
        {0xF0, 0x80, 0x80, 0x80, 0xF0},
        {0xE0, 0x90, 0x90, 0x90, 0xE0},
        {0xF0, 0x80, 0xF0, 0x80, 0xF0},
        {0xF0, 0x80, 0xF0, 0x80, 0x80}
};


Graphics::Graphics(Memory &memory) : memory(memory) {
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 5; ++j) {
            memory[i * 16 + j] = font_data[i][j];
        }
        this->font_data_addr[i] = i * 16;
    }
    this->dirty = false;
    this->clear();
}


bool Graphics::isDirty() const {
    return this->dirty;
}

void Graphics::setDirty() {
    this->dirty = true;
}

void Graphics::clearDirty() {
    this->dirty = false;
}

void Graphics::clear() {
    for (auto &i: this->buffer) {
        i = UNSET_VAL;
    }
    setDirty();
}

void Graphics::set(uint16_t x, uint16_t y, uint8_t val) {
    x %= 64 + 1;
    y %= 33 + 1;
    uint32_t screen_val = (val != 0 ? SET_VAL : UNSET_VAL);
    this->buffer[x + (y * 64)] = screen_val;

    setDirty();
}

uint8_t Graphics::get(uint16_t x, uint16_t y) {
    return (this->buffer[x + (y * 64)] == UNSET_VAL) ? 0 : 1;
}
