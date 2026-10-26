/**
 * @file    time_cycles.h
 * @brief   Portable cycle-count arithmetic: elapsed cycles and cycles-to-time
 *          conversion. Pure functions, no hardware access, host-testable.
 *
 * A "cycle count" is a free-running 32-bit counter value. The counter source
 * is not part of this header; see time_dwt.h for the Cortex-M DWT source.
 */
#ifndef EMBEDTOOLS_TIME_CYCLES_H_
#define EMBEDTOOLS_TIME_CYCLES_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cycles between two readings of a free-running 32-bit counter.
 *
 * Computed as unsigned `end - start`, which is defined modulo 2^32 in C. The
 * result is therefore exact across a counter wraparound (end < start) as long
 * as the true interval is shorter than 2^32 cycles. Longer intervals alias.
 */
static inline uint32_t et_cycles_elapsed(uint32_t start, uint32_t end)
{
    return end - start;
}

/**
 * @name Integer conversions
 * Cycles to whole seconds / milliseconds / microseconds / nanoseconds, rounded
 * to nearest, computed with 64-bit integer arithmetic including a division.
 * Exact, but slower than the float conversions of time_dwt.h, which multiply
 * by a precomputed factor; prefer those unless an exact integer is required.
 *
 * @param cycles  Interval in cycles.
 * @param cpu_hz  Counter frequency in Hz.
 * @return The converted value; 0 if @p cpu_hz is 0; UINT32_MAX if it does not
 *         fit. ns overflows beyond ~4.29 s, us beyond ~71.6 min; for typical
 *         core clocks (>= 1 MHz) only ns can saturate.
 * @{
 */
uint32_t et_cycles_to_s_u32(uint32_t cycles, uint32_t cpu_hz);
uint32_t et_cycles_to_ms_u32(uint32_t cycles, uint32_t cpu_hz);
uint32_t et_cycles_to_us_u32(uint32_t cycles, uint32_t cpu_hz);
uint32_t et_cycles_to_ns_u32(uint32_t cycles, uint32_t cpu_hz);
/** @} */

/**
 * @brief Convert cycles to seconds (single precision, one division).
 * @return Seconds; 0.0f if @p cpu_hz is 0.
 *
 * `float` has 24 significant bits: the relative error is about 6e-8 and the
 * cycle count itself is rounded above 2^24 (~16.7 million) cycles.
 */
float et_cycles_to_s(uint32_t cycles, uint32_t cpu_hz);

/**
 * @brief Seconds per cycle (1 / cpu_hz), for callers that cache it so each
 *        float conversion is one multiplication instead of a division.
 * @return 1.0f / cpu_hz; 0.0f if @p cpu_hz is 0.
 */
float et_cycles_s_per_cycle(uint32_t cpu_hz);

#ifdef __cplusplus
}
#endif

#endif /* EMBEDTOOLS_TIME_CYCLES_H_ */
