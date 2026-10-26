#include "embedtools/time_dwt.h"

/* ARMv7-M / ARMv8-M architectural register addresses. */
#define DEMCR      (*(volatile uint32_t *)0xE000EDFCUL)
#define DWT_CTRL   (*(volatile uint32_t *)0xE0001000UL)
#define DWT_CYCCNT (*(volatile uint32_t *)ET_DWT_CYCCNT_ADDR)
#define DWT_LAR    (*(volatile uint32_t *)0xE0001FB0UL) /* Cortex-M7 lock */

#define DEMCR_TRCENA        (1UL << 24)
#define DWT_CTRL_CYCCNTENA  (1UL << 0)
#define DWT_CTRL_NOCYCCNT   (1UL << 25)
#define DWT_LAR_KEY         0xC5ACCE55UL

static uint32_t s_cpu_hz;

float et_dwt_scale_s  = 0.0f;
float et_dwt_scale_ms = 0.0f;
float et_dwt_scale_us = 0.0f;
float et_dwt_scale_ns = 0.0f;

bool et_dwt_init(uint32_t cpu_hz)
{
    s_cpu_hz = cpu_hz;
    et_dwt_scale_s  = et_cycles_s_per_cycle(cpu_hz);
    et_dwt_scale_ms = 1.0e3f * et_dwt_scale_s;
    et_dwt_scale_us = 1.0e6f * et_dwt_scale_s;
    et_dwt_scale_ns = 1.0e9f * et_dwt_scale_s;

    DEMCR |= DEMCR_TRCENA;      /* DWT ignores writes without TRCENA */
    DWT_LAR = DWT_LAR_KEY;      /* no-op on M3/M4 */

    if ((DWT_CTRL & DWT_CTRL_NOCYCCNT) != 0u)
    {
        return false;
    }

    DWT_CYCCNT = 0u;
    DWT_CTRL |= DWT_CTRL_CYCCNTENA;

    return cpu_hz != 0u;
}

uint32_t et_dwt_to_s_u32(uint32_t cycles)  { return et_cycles_to_s_u32(cycles, s_cpu_hz); }
uint32_t et_dwt_to_ms_u32(uint32_t cycles) { return et_cycles_to_ms_u32(cycles, s_cpu_hz); }
uint32_t et_dwt_to_us_u32(uint32_t cycles) { return et_cycles_to_us_u32(cycles, s_cpu_hz); }
uint32_t et_dwt_to_ns_u32(uint32_t cycles) { return et_cycles_to_ns_u32(cycles, s_cpu_hz); }
