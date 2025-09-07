#include<iostream>

#include "cpu.h"
#include "util.h"

cpu::cpu(uint8_t* memory) : mem(memory) {}
cpu::cpu(uint8_t* memory, uint16_t initCs, uint16_t initIp) : mem(memory), cs(initCs), ip(initIp) {}
uint64_t cpu::step() {
    uint8_t op = mem[getOffset(cs, ip)];
    uint8_t op1 = mem[getOffset(cs, ip + 1)];
    uint8_t op2 = mem[getOffset(cs, ip + 2)];
    std::cout << std::hex << cs << ":" << ip << "\t";
    switch(op) {
        case 0xba: // mov $0x1234, %dx
            dx = op2 << 8 | op1;
            std::cout<<"mov $0x" << uint16_t(op2) << uint16_t(op1) << ", %dx" <<'\n';
            ip += 3;
            return 1;
        default:
            std::cout << uint16_t(op) << " " << uint16_t(op1) << " " << uint16_t(op2) << '\n';
            return 0;
    }
}

uint16_t cpu::read16(size_t offset) {
    return (static_cast<uint16_t>(mem[offset + 1]) << 8) + mem[offset];
}

void cpu::write16(size_t offset, uint16_t value) {
    uint8_t byte1 = value >> 8;
    uint8_t byte2 = value & 0xff;
    mem[offset] = byte2;
    mem[offset + 1] = byte1;
} 