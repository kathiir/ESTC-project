#ifndef NVMC_MODULE_H
#define NVMC_MODULE_H

#include "nrfx_nvmc.h"

#define MASK (0xFFFFFFFF)

void nvmc_write(uint32_t source);

uint32_t nvmc_read();

void nvmc_init(void);

#endif /* NVMC_MODULE_H */