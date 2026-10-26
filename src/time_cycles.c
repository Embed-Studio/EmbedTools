#include "embedtools/time_cycles.h"

static uint32_t scaled(uint32_t cycles, uint32_t cpu_hz, uint32_t units_per_s)
{
    if (cpu_hz == 0u)
    {
        return 0u;
    }

    const uint64_t v =
        ((uint64_t)cycles * units_per_s + (uint64_t)(cpu_hz / 2u)) / (uint64_t)cpu_hz;

    return (v > UINT32_MAX) ? UINT32_MAX : (uint32_t)v;
}

uint32_t et_cycles_to_s_u32(uint32_t cycles, uint32_t cpu_hz)  { return scaled(cycles, cpu_hz, 1u); }
uint32_t et_cycles_to_ms_u32(uint32_t cycles, uint32_t cpu_hz) { return scaled(cycles, cpu_hz, 1000u); }
uint32_t et_cycles_to_us_u32(uint32_t cycles, uint32_t cpu_hz) { return scaled(cycles, cpu_hz, 1000000u); }
uint32_t et_cycles_to_ns_u32(uint32_t cycles, uint32_t cpu_hz) { return scaled(cycles, cpu_hz, 1000000000u); }

float et_cycles_to_s(uint32_t cycles, uint32_t cpu_hz)
{
    if (cpu_hz == 0u)
    {
        return 0.0f;
    }

    return (float)cycles / (float)cpu_hz;
}

float et_cycles_s_per_cycle(uint32_t cpu_hz)
{
    if (cpu_hz == 0u)
    {
        return 0.0f;
    }

    return 1.0f / (float)cpu_hz;
}
