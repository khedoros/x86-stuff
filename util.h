#pragma once
#include<cstdint>
#include<array>
#include<vector>

typedef uint16_t segment_t;
typedef uint16_t offset_t;

size_t getOffset(segment_t seg, offset_t off);
uint16_t read16(const std::array<uint8_t, 1024 * 1024>& memory, size_t offset);
uint16_t read16(const std::vector<uint8_t>& memory, size_t offset);
void write16(std::array<uint8_t, 1024 * 1024>& memory, size_t offset, uint16_t value);