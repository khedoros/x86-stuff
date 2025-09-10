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
    uint16_t cx;
    uint16_t dx;
    uint16_t bx;
    uint16_t sp;
    uint16_t bp;
    uint16_t si;
    uint16_t di;

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

/*
one opcode format:
0aaaaads
aaaaa: opcode
d: direction, 0=register to memory, 1=memory to register
s: 0 for 8-bit operands, 1 for 16 or 32-bit operands (32 is default, 16 takes a prefix byte)

1aaaaaxs
aaaaa: opcode
x: 0 indicates const is same size as specified by `s` field, 1 is 8-bit sign-extended
s: 0 for 8-bit, 1 for 32/16-bit
*/

/*
mod r/m byte:
aabbbccc

aa: MOD
00: Register indirect addressing mode or SIB with no displacement (when R/M = 100) or Displacement only addressing mode (when R/M = 101). 
01: One-byte signed displacement follows addressing mode byte(s).
10: Four-byte signed displacement follows addressing mode byte(s).
11: Register addressing mode.

bbb: reg    8-bit 16-bit 32-bit
000         al    ax     eax
001         cl    cx     ecx
010         dl    dx     edx
011         bl    bx     ebx
100         ah    sp     esp
101         ch    bp     ebp
110         dh    si     esi
111         bh    di     edi

r/m: aa ccc: 
     00 000 [ eax ]
     01 000 [ eax + disp8 ]               (1)
     10 000 [ eax + disp32 ]
     11 000 register  ( al / ax / eax )   (2)
     00 001 [ ecx ]
     01 001 [ ecx + disp8 ]
     10 001 [ ecx + disp32 ]
     11 001 register  ( cl / cx / ecx )
     00 010 [ edx ]
     01 010 [ edx + disp8 ]
     10 010 [ edx + disp32 ]
     11 010 register  ( dl / dx / edx )
     00 011 [ ebx ]
     01 011 [ ebx + disp8 ]
     10 011 [ ebx + disp32 ]
     11 011 register  ( bl / bx / ebx )
     00 100 SIB  Mode                     (3)
     01 100 SIB  +  disp8  Mode
     10 100 SIB  +  disp32  Mode
     11 100 register  ( ah / sp / esp )
     00 101 32-bit Displacement-Only Mode (4)
     01 101 [ ebp + disp8 ]
     10 101 [ ebp + disp32 ]
     11 101 register  ( ch / bp / ebp )
     00 110 [ esi ]
     01 110 [ esi + disp8 ]
     10 110 [ esi + disp32 ]
     11 110 register  ( dh / si / esi )
     00 111 [ edi ]
     01 111 [ edi + disp8 ]
     10 111 [ edi + disp32 ]
     11 111 register  ( bh / di / edi )

SIB = scaled index byte
ssiiibbb

ss:
00  index * 1
01  index * 2
10  index * 4
11  index * 8

iii:
000	EAX
001	ECX
010	EDX
011	EBX
100	Illegal
101	EBP
110	ESI
111	EDI

bbb:
000	xx	EAX
001	xx	ECX
010	xx	EDX
011	xx	EBX
100	xx	ESP
101	00	Displacement-only
101 01, 10	EBP
110	xx	ESI
111	xx	EDI 
*/