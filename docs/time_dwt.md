# Timing: `time_dwt` and `time_cycles`

## What it uses

The ARM Cortex-M **DWT** (Data Watchpoint and Trace) unit's `CYCCNT` register:
a 32-bit counter that increments once per core clock cycle. It uses no
peripheral timer, interrupt or HAL, and reading it is a single load.
Registers are accessed at their architectural addresses, so no CMSIS or device
header is required.

Not available on Cortex-M0/M0+. `et_dwt_init()` returns `false` if the
core reports no cycle counter (`DWT_CTRL.NOCYCCNT`).

## API

| Function | Header | Notes |
|---|---|---|
| `bool et_dwt_init(uint32_t cpu_hz)` | time_dwt.h | enable + zero counter, store frequency |
| `uint32_t et_dwt_now(void)` | time_dwt.h | inline, one load |
| `uint32_t et_dwt_elapsed(uint32_t start)` | time_dwt.h | cycles since `start` |
| `float et_dwt_to_{s,ms,us,ns}(uint32_t cycles)` | time_dwt.h | **preferred**: inline, one multiply by a factor cached at init |
| `uint32_t et_dwt_to_{s,ms,us,ns}_u32(uint32_t cycles)` | time_dwt.h | exact, rounded, 64-bit division; slower, not inline |
| `et_dwt_interval_t`, `_interval_init`, `_interval_update` | time_dwt.h | sample-to-sample cycles, rearms |
| `ET_DWT_START/ELAPSED/RESTART(name)` | time_dwt.h | named stopwatch macros |
| `et_cycles_elapsed(start, end)` | time_cycles.h | portable |
| `et_cycles_to_{s,ms,us,ns}_u32(cycles, cpu_hz)` | time_cycles.h | portable, integer |
| `et_cycles_to_s(cycles, cpu_hz)` | time_cycles.h | portable, float, divides |
| `et_cycles_s_per_cycle(cpu_hz)` | time_cycles.h | portable, 1/hz |

## Initialization

Call `et_dwt_init(hz)` once, after the clock tree is configured, and
pass the frequency `CYCCNT` actually counts at (the core clock, e.g.
`SystemCoreClock` on STM32). No debugger is needed. Calling it again restarts
the counter, which invalidates earlier timestamps; do so after any runtime
change of the core clock.

## CPU frequency

The frequency is only used for conversions; raw cycle counts never depend on
it. A wrong `cpu_hz` gives wrong times, not wrong cycles. With `cpu_hz == 0`
conversions return 0.

## Counter width and wraparound

`CYCCNT` wraps after 2^32 cycles:

| Core clock | Wrap period |
|---|---|
| 16 MHz | 268.4 s |
| 84 MHz | 51.1 s |
| 168 MHz | 25.6 s |
| 480 MHz | 8.9 s |

Elapsed cycles are `end - start` in `uint32_t`. Unsigned arithmetic is defined
modulo 2^32, so the result is correct across a wrap with no special case. Any
interval of 2^32 cycles or longer is ambiguous (aliases); it cannot be detected
from two readings.

## Cycles to time

`us = (cycles * 1e6 + hz/2) / hz`, computed in 64-bit integers, rounded to
nearest, saturating at `UINT32_MAX` (only reachable below ~1 MHz).
The DWT float conversions multiply by a factor cached at init (`1/hz`, `1e6/hz`) to avoid a runtime division; the portable `et_cycles_to_s` divides. `float` has 24 bits of mantissa, so
the relative error is ~6e-8 and cycle counts above 16.7 M are rounded.

## Limitations

- The measurement includes its own cost: back-to-back `now()` calls differ by a
  few cycles. Subtract that overhead for very short intervals.
- Interrupts running during the interval are included in it.
- A timestamp taken inside an ISR carries the interrupt latency of that ISR.
- The counter halts in some low-power modes and some devices gate the DWT
  while sleeping; do not measure across sleep.
- Debuggers may reset or reconfigure DWT; re-init if measurements read 0.
- State is one module-level frequency; there is one DWT per core.
- Not thread-safe during init.
- The host tests cover only the portable arithmetic. The register access in
  `platform/arm_cortex_m/time_dwt.c` is verified on hardware only.
