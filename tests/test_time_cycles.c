#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "embedtools/time_cycles.h"

int main(void)
{
    /* elapsed: plain, zero, and across wraparound */
    assert(et_cycles_elapsed(100u, 350u) == 250u);
    assert(et_cycles_elapsed(7u, 7u) == 0u);
    assert(et_cycles_elapsed(0xFFFFFFF0u, 0x00000010u) == 0x20u);
    assert(et_cycles_elapsed(1u, 0u) == 0xFFFFFFFFu);

    /* us: exact, rounding, zero frequency, saturation */
    assert(et_cycles_to_us_u32(168u, 168000000u) == 1u);
    assert(et_cycles_to_us_u32(168000000u, 168000000u) == 1000000u);
    assert(et_cycles_to_us_u32(83u, 168000000u) == 0u);   /* 0.494 us */
    assert(et_cycles_to_us_u32(85u, 168000000u) == 1u);   /* 0.506 us */
    assert(et_cycles_to_us_u32(0xFFFFFFFFu, 168000000u) == 25565282u);
    assert(et_cycles_to_us_u32(1000u, 0u) == 0u);
    assert(et_cycles_to_us_u32(0xFFFFFFFFu, 1000u) == 0xFFFFFFFFu);

    /* seconds */
    assert(et_cycles_to_s(168000000u, 168000000u) == 1.0f);
    assert(fabsf(et_cycles_to_s(84000u, 168000000u) - 0.0005f) < 1e-9f);
    assert(et_cycles_to_s(1000u, 0u) == 0.0f);

    /* s / ms / ns */
    assert(et_cycles_to_s_u32(252000000u, 168000000u) == 2u);
    assert(et_cycles_to_s_u32(84000000u, 168000000u) == 1u);      /* 0.5 s rounds up */
    assert(et_cycles_to_ms_u32(168000u, 168000000u) == 1u);
    assert(et_cycles_to_ms_u32(1000u, 0u) == 0u);
    assert(et_cycles_to_ns_u32(168u, 168000000u) == 1000u);
    assert(et_cycles_to_ns_u32(168000000u, 168000000u) == 1000000000u);
    assert(et_cycles_to_ns_u32(0xFFFFFFFFu, 168000000u) == 0xFFFFFFFFu);  /* saturates */

    /* scale factor */
    assert(et_cycles_s_per_cycle(0u) == 0.0f);
    assert(fabsf(et_cycles_s_per_cycle(168000000u) * 168000000.0f - 1.0f) < 1e-6f);

    puts("time_cycles: all tests passed");
    return 0;
}
