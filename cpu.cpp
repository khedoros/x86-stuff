#include<array>
#include<iostream>
#include <sys/types.h>

#include "cpu.h"
#include "util.h"

cpu::cpu(uint8_t* memory) : mem(memory) {}
cpu::cpu(uint8_t* memory, uint16_t initCs, uint16_t initIp) : mem(memory), cs(initCs), ip(initIp) {}
uint64_t cpu::step() {
    uint8_t op = mem[getOffset(cs, ip)];
    uint8_t op1 = mem[getOffset(cs, ip + 1)];
    uint8_t op2 = mem[getOffset(cs, ip + 2)];

    std::cout << std::hex << cs << ":" << ip << "\t";
    std::cout << uint16_t(op) << " " << uint16_t(op1) << " " << uint16_t(op2);
    uint8_t argSize = 32;
    uint16_t segment = ds;
    std::array<uint16_t*, 8> reg16{&ax, &cx, &dx, &bx, &sp, &bp, &si, &di};
    const std::array<std::string, 8> reg16Names{"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
    std::array<segment_t*, 4> seg{&es, &cs, &ss, &ds};
    std::array<std::string, 4> segNames{"es", "cs", "ss", "ds"};
    while (true) {
        switch(op) {
            case 0x26: // ES sement override    00100
            case 0x2e: // CS segment override   00101
            case 0x36: // SS segment override   00110
            case 0x3e: // DS segment override   00111
                segment = *seg[(op>>3)&3];
                ip++;
                op = mem[getOffset(cs, ip)];
                break;
            case 0xb8: // ax
            case 0xb9: // cx
            case 0xba: // mov $0x1234, %dx
            case 0xbb: // bx
            case 0xbc: // sp
            case 0xbd: // bp
            case 0xbe: // si
            case 0xbf: // di
                *reg16[op&0x7] = op2 << 8 | op1;
                std::cout << "\tmov $0x" << uint16_t(op2) << uint16_t(op1) << ", %" << reg16Names[op&0x07] << '\n';
                ip += 3;
                return 1;
            default:
                std::cout << '\n';
                return 0;
        }
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
