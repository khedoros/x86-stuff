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
    uint8_t modrm = 0;
    uint8_t mod = 0;
    uint8_t modreg = 0;
    uint8_t rm = 0;
    uint8_t val8 = 0;

    std::cout << std::hex << cs << ":" << ip << "\t";
    std::cout << uint16_t(op) << " " << uint16_t(op1) << " " << uint16_t(op2);
    uint8_t argSize = 32;
    uint16_t segment = ds;
    std::string segName = "ds";
    uint16_t offset = 0;

    uint16_t zero = 0;
    std::array<uint16_t*, 8> reg16{&ax, &cx, &dx, &bx, &sp, &bp, &si, &di};
    const std::array<std::string, 8> reg16Names{"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

    std::array<uint16_t*, 8> reg8{&ax, &cx, &dx, &bx, &ax, &cx, &dx, &bx};
    const std::array<std::string, 8> reg8Names{"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

    std::array<uint16_t*, 8> modReg16_1{&bx, &bx, &bp, &bp, &si, &di, &bp, &bx};
    const std::array<std::string, 8> modReg16Names_1{"bx", "bx", "bp", "bp", "si", "di", "bp", "bx"};
    std::array<uint16_t*, 8> modReg16_2{&si, &di, &si, &di, &zero, &zero, &zero, &zero};
    const std::array<std::string, 8> modReg16Names_2{"si", "di", "si", "di", "", "", "", ""};

    std::array<segment_t*, 4> seg{&es, &cs, &ss, &ds};
    std::array<std::string, 4> segNames{"es", "cs", "ss", "ds"};
    while (true) {
        switch(op) {
            case 0x26: // ES sement override    00100
            case 0x2e: // CS segment override   00101
            case 0x36: // SS segment override   00110
            case 0x3e: // DS segment override   00111
                segment = *seg[(op>>3)&3];
                segName = segNames[(op>>3)&3];
                ip++;
                op = mem[getOffset(cs, ip)];
                break;
            case 0x89: // MOV
                modrm = mem[getOffset(cs,ip+1)];
                mod = modrm >> 6;
                modreg = (modrm >> 3) & 7;
                rm = modrm & 7;
                ip+=2;
                std::cout << "\t mov " << reg16Names[modreg] << ", ";
                switch(mod) {
                    case 0: // r/m: 000 = [BX + SI]          001 = [BX + DI]          010 = [BP + SI]          011 = [BP + DI]          100 = [SI]          101 = [DI]          110 = [disp16]      111 = [BX]
                        if(rm == 6) {
                            offset = read16(getOffset(cs, ip));
                            std::cout<<segName<<":"<<offset<<'\n';
                            ip+=2;
                        }
                        else {
                            offset = *modReg16_1[rm] + *modReg16_2[rm];
                            std::cout << segName << ":[" << modReg16Names_1[rm] << " + " << modReg16Names_2[rm] << "]\n";
                        }
                        break;
                    case 1: //            [BX + SI + disp8]        [BX + DI + disp8]        [BP + SI + disp8]        [BP + DI + disp8]        [SI + disp8]        [DI + disp8]        [BP + disp8]        [BX + disp8]
                        offset = mem[getOffset(cs, ip)];
                        std::cout << segName << ":[" << modReg16Names_1[rm] << " + " << modReg16Names_2[rm] << " + " << offset << "]\n";
                        offset += *modReg16_1[rm] + *modReg16_2[rm];
                        ip++;
                        break;
                    case 2: //            [BX + SI + disp16]       [BX + DI + disp16]       [BP + SI + disp16]       [BP + DI + disp16]       [SI + disp16]       [DI + disp16]       [BP + disp16]       [BX + disp16]
                        offset = read16(getOffset(cs, ip));
                        std::cout << segName << ":[" << modReg16Names_1[rm] << " + " << modReg16Names_2[rm] << " + " << offset << "]\n";
                        offset += *modReg16_1[rm] + *modReg16_2[rm];
                        ip+=2;
                        break;
                    case 3: // r/m TODO: Figure this out
                        return 0;
                }
                *reg16[modreg] = read16(getOffset(segment, offset));
                return 1;
            case 0xb0: // al
            case 0xb1: // cl
            case 0xb2: // dl
            case 0xb3: // bl
            case 0xb4: // ah
            case 0xb5: // ch
            case 0xb6: // dh
            case 0xb7: // mov $0x12, %bh
                val8 = mem[getOffset(cs, ip + 1)];
                if((op & 0x7) < 4) {
                    *reg8[op&7] &= 0xff00;
                    *reg8[op&7] |= val8;
                }
                else {
                    *reg8[op&7] &= 0x00ff;
                    *reg8[op&7] |= (val8<<8);
                }
                ip += 2;
                std::cout<<"mov " << reg8Names[op & 0x7] << ", " << uint16_t(val8) << '\n';
                return 1;
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
