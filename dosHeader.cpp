#include<iostream>
#include<fstream>
#include<stdint.h>
#include<assert.h>
#include<vector>

#include "mzHeader.h"

using namespace std;

int main(int argc, char *argv[]) {
    assert(sizeof(mzHeader) == 32);
    ifstream in;
    in.open(argv[1]);
    in.seekg(0,ios::end);
    size_t size = in.tellg();
    in.seekg(0,ios::beg);
    cout<<"Filename: "<<argv[1]<<endl<<"Filesize: "<<size<<endl;
    mzHeader h;
    in.read(reinterpret_cast<char *>(&h), sizeof(mzHeader));
    vector<uint16_t> relocTable;
    relocTable.resize(h.relocEntries*2);
    in.seekg(h.relocOffset,ios::beg);
    in.read(reinterpret_cast<char *>(&(relocTable[0])), h.relocEntries*4);
    printf("Bytes on last page: %04x\nTotal pages: %04x\nRelocation entries: %d\nHeader size in paragraphs: %04x\nMin mem needed after exe: %04x\nMax mem needed after exe: %04x\nSS offset: %04x\nInitial SP: %04x\nChecksum: %04x\nEntry point: %04x:%04x\nOffset of relocation table: %04x\nOverlay number (expect 0): %04x\nCould be ID: %02x %02x %02x %02x\n",h.lastPageBytes,h.totalPages,h.relocEntries,h.headerParaSize,h.minParasAfterCode,h.maxParasAfterCode,h.stackSegOffset,h.initialSp,h.chksum,h.initCs,h.initIp,h.relocOffset,h.overlayNum,h.possId[0],h.possId[1],h.possId[2],h.possId[3]);
    if(h.possId[0] == 0x01 && h.possId[1] == 0x00 && h.possId[2] == 0xfb) {
        std::cout<<"Probably Borland TLink, version "<<h.possId[3]/16<<endl;
    }
    cout<<"Relocation table: "<<h.relocEntries<<" entries"<<endl;
    bool odd=false;
    uint16_t offset=0;
    for(uint16_t off:relocTable) {
        if(odd)
            printf("%04x:%04x\n",off,offset);
        else
            offset = off;
        odd=!odd;
    }
    return 0;
}
