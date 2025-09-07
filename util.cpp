#include "util.h"

size_t getOffset(segment_t seg, offset_t off) {
    return static_cast<uint32_t>(seg) * 0x10 + off;
}

uint16_t read16(const std::array<uint8_t, 1024 * 1024>& memory, size_t offset) {
    return (static_cast<uint16_t>(memory[offset + 1]) << 8) + memory[offset];
}

uint16_t read16(const std::vector<uint8_t>& memory, size_t offset) {
    return (static_cast<uint16_t>(memory[offset + 1]) << 8) + memory[offset];
}

void write16(std::array<uint8_t, 1024 * 1024>& memory, size_t offset, uint16_t value) {
    uint8_t byte1 = value >> 8;
    uint8_t byte2 = value & 0xff;
    memory[offset] = byte2;
    memory[offset + 1] = byte1;
} 