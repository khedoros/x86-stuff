#pragma once
#include<cstdint>
#include <functional>

class cpu {
public:
    cpu(uint8_t* mem);
    cpu(uint8_t* mem, uint16_t initCs, uint16_t initIp);
    uint64_t step();
private:
    uint16_t ax;
    uint16_t bx;
    uint16_t cx;
    uint16_t dx;

    uint16_t ip;

    uint16_t cs;
    uint16_t ds;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
    uint16_t ss;

    uint16_t flags = 2;

    uint8_t * mem;

    enum flags {
        cf = 1,
        pf = 4,
        af = 0x10,
        zf = 0x40,
        sf = 0x80,
        tf = 0x100,
        intf = 0x200,
        df = 0x400,
        of = 0x800,
    };

    uint16_t read16(size_t offset);
    void write16(size_t offset, uint16_t value);
    /* status flags
    0x1      CF Carry
    0x2      Reserved; always 1?
    0x4      PF Parity
    0x8      Reserved
    0x10     AF Aux Carry
    0x20     Reserved
    0x40     ZF Zero
    0x80     SF Sign
    0x100    TF Trap Flag (single step)
    0x200    IF Interrupt enable
    0x400    DF Direction
    0x800    OF Overflow
    0x3000   I/O privilege level
    0x4000   NT Nested task
    0x8000   MD mode (reserved); always 0
    */
};