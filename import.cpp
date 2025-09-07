#include<iostream>
#include<fstream>
#include<cstdint>
#include<array>
#include<vector>

#include "util.h"
#include "mzHeader.h"
#include "cpu.h"



int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr<<"Provide a single DOS binary\n";
        return 1;
    }
    std::array<uint8_t, 1024 * 1024> memory;
    std::vector<uint8_t> fileTemp;
    std::ifstream exeIn(argv[1]);
    exeIn.seekg(0, std::ios::end);
    size_t exeSize = exeIn.tellg();
    fileTemp.resize(exeSize);
    exeIn.seekg(0, std::ios::beg);
    exeIn.read(reinterpret_cast<char*>(fileTemp.data()), exeSize);
    for(const unsigned int c:fileTemp) {
        // std::cout<<std::hex<<c<<" ";
    }

    // Copied from DosBox loading this game
    const segment_t baseSegment = 0x20e;
    size_t memoryOffset = getOffset(baseSegment, 0);
    mzHeader* header = reinterpret_cast<mzHeader*>(fileTemp.data());
    std::cout<<"Image start offset: " << std::hex << header->headerParaSize * 0x10 << "\nImage stop offset: " << header->totalPages * 0x200 - (0x200 - header->lastPageBytes) << '\n';
    for(size_t offset = header->headerParaSize * 0x10; offset < header->totalPages * 0x200 - (0x200 - header->lastPageBytes); offset++) {
        memory.at(memoryOffset) = fileTemp.at(offset);
        memoryOffset++;
        // std::cout<<"mem[" << memoryOffset << "] = file[" << offset << "]\n";
    }
    std::cout<<"Processing " << std::dec << header->relocEntries << std::hex << " relocation entries\n";
    size_t imageStart = getOffset(baseSegment, 0);
    for(int i = 0; i < header->relocEntries; i++) {
        offset_t offset = read16(fileTemp, header->relocOffset + i * 4);
        segment_t segment = read16(fileTemp, header->relocOffset + i * 4 + 2);
        size_t originalAddr = imageStart + getOffset(segment, offset);
        segment_t original = read16(memory, originalAddr);
        // std::cout<<segment<<":"<<offset<<" was " << original << " now " << original + baseSegment << ", written to " << originalAddr << '\n';
        original += baseSegment;
        write16(memory, originalAddr, original);
    }

    cpu c(memory.data(), baseSegment + header->initCs, header->initIp);

    while(c.step()) {
        std::cout << "Cycle\n";
    }
}
