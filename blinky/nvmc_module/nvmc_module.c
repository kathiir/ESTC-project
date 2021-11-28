#include "nvmc_module.h"

#include "nrf_dfu_types.h"

#define BOOTLOADER_ADDR 0x000E0000U

#define ADDRESS (BOOTLOADER_ADDR - NRF_DFU_APP_DATA_AREA_SIZE)

void nvmc_write(uint32_t source) 
{
    APP_ERROR_CHECK(nrfx_nvmc_page_erase(ADDRESS));

    nrfx_nvmc_word_write(ADDRESS, source);
}

uint32_t nvmc_read()
{
    uint32_t * word = (uint32_t *) ADDRESS;

    return word[0];
}


void nvmc_init(void);