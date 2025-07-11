#pragma once

#include "Generic/generic.h"

#define FLASH_BASE 0x40023C00UL

typedef struct {
    uint32_t ACR;
    uint32_t KEYR;
    uint32_t OPTKEYR;
    uint32_t SR;
    uint32_t CR;
    uint32_t OPTCR;
} Flash_Typedef;

#define FLASH ((Flash_Typedef *) FLASH_BASE)