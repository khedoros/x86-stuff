#pragma once

#include<cstdint>

typedef struct {
    char magic[2];
    uint16_t lastPageBytes;
    uint16_t totalPages;
    uint16_t relocEntries;
    uint16_t headerParaSize;
    uint16_t minParasAfterCode;
    uint16_t maxParasAfterCode;
    uint16_t stackSegOffset;
    uint16_t initialSp;
    uint16_t chksum;
    uint16_t initIp;
    uint16_t initCs;
    uint16_t relocOffset;
    uint16_t overlayNum;
    uint8_t possId[4];
} mzHeader;

