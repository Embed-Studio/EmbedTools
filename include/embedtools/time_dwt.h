/**
 * @file    time_dwt.h
 * @brief   Cycle timestamps from the ARM Cortex-M DWT cycle counter (CYCCNT).
 *
 * PLATFORM: ARM Cortex-M with a DWT cycle counter (M3, M4, M7, M33, M55 when
 * implemented). Not available on Cortex-M0/M0+ or on a host. Not STM32
 * specific: no HAL, no CMSIS, no device header is needed.
 *
 * Typical use:
 * @code{.c}
 * et_dwt_init(SystemCoreClock);          // once, after clock setup
 *
 * uint32_t t0 = et_dwt_now();
 * do_work();
 * uint32_t cycles = et_dwt_elapsed(t0);
 * uint32_t us     = et_dwt_to_us_u32(cycles);
 *
 * ET_DWT_START(read_sensor);             // same, with a named stopwatch
 * read_sensor();
 * uint32_t us2 = et_dwt_to_us_u32(ET_DWT_ELAPSED(read_sensor));
 *
 * static et_dwt_interval_t sample;       // sample-to-sample interval
 * et_dwt_interval_init(&sample);
 * float dt = et_dwt_to_s(et_dwt_interval_update(&sample));
 * @endcode
 *
 * Not thread-safe or reentrant: call init before any concurrent use.
 */
#ifndef EMBEDTOOLS_TIME_DWT_H_
#define EMBEDTOOLS_TIME_DWT_H_

#include <stdbool.h>
#include <stdint.h>

#include "embedtools/time_cycles.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Architecturally fixed address of DWT->CYCCNT on ARMv7-M / ARMv8-M. */
#define ET_DWT_CYCCNT_ADDR 0xE0001004UL

/**
 * @brief Enable and zero the DWT cycle counter and record the CPU frequency.
 *
 * Enables DEMCR.TRCENA, unlocks the DWT (Cortex-M7 software lock; ignored on
 * M3/M4), clears CYCCNT and sets CYCCNTENA. No debugger is required.
 * Calling it again restarts the counter and invalidates earlier timestamps.
 * Call it again after changing the core clock.
 *
 * @param cpu_hz  Frequency in Hz at which CYCCNT counts (the core clock).
 *                Used only by the conversion functions.
 * @return true on success; false if @p cpu_hz is 0 or the core has no cycle
 *         counter (DWT_CTRL.NOCYCCNT). On false, timestamps read as 0.
 */
bool et_dwt_init(uint32_t cpu_hz);

/** @brief Current CYCCNT value. A single load; wraps every 2^32 cycles. */
static inline uint32_t et_dwt_now(void)
{
    return *(volatile const uint32_t *)ET_DWT_CYCCNT_ADDR;
}

/**
 * @brief Cycles since @p start (a value from et_dwt_now()).
 *
 * Wraparound-safe for intervals shorter than 2^32 cycles, see
 * et_cycles_elapsed().
 */
static inline uint32_t et_dwt_elapsed(uint32_t start)
{
    return et_cycles_elapsed(start, et_dwt_now());
}

/**
 * @name Float conversions (preferred)
 * One multiplication by a factor computed in et_dwt_init(): no division, no
 * 64-bit arithmetic, inline. Much faster than the integer conversions below,
 * at the cost of float precision (24-bit mantissa, relative error ~6e-8; see
 * docs/time_dwt.md). Return 0.0f until et_dwt_init() has run.
 * @{
 */
extern float et_dwt_scale_s;  /**< Read-only: seconds per cycle. */
extern float et_dwt_scale_ms; /**< Read-only: milliseconds per cycle. */
extern float et_dwt_scale_us; /**< Read-only: microseconds per cycle. */
extern float et_dwt_scale_ns; /**< Read-only: nanoseconds per cycle. */

static inline float et_dwt_to_s(uint32_t cycles)  { return (float)cycles * et_dwt_scale_s; }
static inline float et_dwt_to_ms(uint32_t cycles) { return (float)cycles * et_dwt_scale_ms; }
static inline float et_dwt_to_us(uint32_t cycles) { return (float)cycles * et_dwt_scale_us; }
static inline float et_dwt_to_ns(uint32_t cycles) { return (float)cycles * et_dwt_scale_ns; }
/** @} */

/**
 * @name Integer conversions
 * Rounded to nearest, exact, saturating at UINT32_MAX (ns overflows beyond
 * ~4.29 s). Each call performs a 64-bit division, so these are slower than
 * the float conversions above and are deliberately not inline (the
 * frequency is private to time_dwt.c and the division code is large). Use
 * them when an exact integer is needed. Return 0 until et_dwt_init() has run.
 * @{
 */
uint32_t et_dwt_to_s_u32(uint32_t cycles);
uint32_t et_dwt_to_ms_u32(uint32_t cycles);
uint32_t et_dwt_to_us_u32(uint32_t cycles);
uint32_t et_dwt_to_ns_u32(uint32_t cycles);
/** @} */

/** @brief Interval helper state: timestamp of the previous update. */
typedef struct
{
    uint32_t last;
} et_dwt_interval_t;

/** @brief Arm the helper at the current timestamp. @p t must not be NULL. */
static inline void et_dwt_interval_init(volatile et_dwt_interval_t *t)
{
    t->last = et_dwt_now();
}

/**
 * @brief Cycles since the previous update (or init), then rearm.
 *
 * Takes one timestamp, so successive results tile the timeline with no gaps
 * or drift: they sum to the total elapsed time.
 */
static inline uint32_t et_dwt_interval_update(volatile et_dwt_interval_t *t)
{
    const uint32_t now = et_dwt_now();
    const uint32_t elapsed = et_cycles_elapsed(t->last, now);

    t->last = now;
    return elapsed;
}

/** @brief Declare a local stopwatch @p name started at the current timestamp. */
#define ET_DWT_START(name)   uint32_t et_sw_##name = et_dwt_now()

/** @brief Cycles since the matching ET_DWT_START; stopwatch keeps running. */
#define ET_DWT_ELAPSED(name) et_dwt_elapsed(et_sw_##name)

/** @brief Restart a stopwatch at the current timestamp. */
#define ET_DWT_RESTART(name) (et_sw_##name = et_dwt_now())

#ifdef __cplusplus
}
#endif

#endif /* EMBEDTOOLS_TIME_DWT_H_ */
