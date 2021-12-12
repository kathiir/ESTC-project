#include "nvmc_module.h"

#include "nrf_dfu_types.h"

#define BOOTLOADER_ADDR 0x000E0000U

#define ADDRESS (BOOTLOADER_ADDR - NRF_DFU_APP_DATA_AREA_SIZE)

pwm_hsv_t * m_pwm_hsv;

void nvmc_write(uint32_t source)
{
    APP_ERROR_CHECK(nrfx_nvmc_page_erase(ADDRESS));

    nrfx_nvmc_word_write(ADDRESS, source);
}

uint32_t nvmc_read(void)
{
    uint32_t * word = (uint32_t *) ADDRESS;

    return *word;
}

void save_state(void)
{
    uint32_t to_write = 0;

    uint8_t hue = m_pwm_hsv->hue / 100;
    uint8_t saturation = m_pwm_hsv->saturation / 100;
    uint8_t value = m_pwm_hsv->value / 100;

    to_write = ((0xFF & hue) << 24)
             + ((0xFF & saturation) << 16)
             + ((0xFF & value) << 8)
             + 0xDE;

    nvmc_write(to_write);

    NRF_LOG_INFO("State saved: %d, %d, %d", hue, saturation, value);
}

void restore_state(void)
{
    uint32_t to_read = nvmc_read();

    if (0xDE == (0xFF & to_read))
    {
        m_pwm_hsv->hue        =  (0xFF & (to_read >> 24)) * 100;
        m_pwm_hsv->saturation =  (0xFF & (to_read >> 16)) * 100;
        m_pwm_hsv->value      =  (0xFF & (to_read >> 8)) * 100;
    }
    else
    {
        m_pwm_hsv->hue        =  0;
        m_pwm_hsv->saturation =  SATURATION_TOP_VALUE;
        m_pwm_hsv->value      =  BRIGHTNESS_TOP_VALUE;
    }

    NRF_LOG_INFO("State restored: %d, %d, %d", m_pwm_hsv->hue, m_pwm_hsv->saturation, m_pwm_hsv->value);
}

void nvmc_init(pwm_hsv_t * hsv)
{
    m_pwm_hsv = hsv;
    restore_state();
}