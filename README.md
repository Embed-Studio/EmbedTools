# EmbedTools

Small, reusable embedded C utilities. Portable core plus explicitly selected
platform modules. No dynamic allocation. Grows only from code used in real
development.

## Layout

    include/embedtools/   public headers
    src/                  portable code (host-testable)
    platform/arm_cortex_m/ Cortex-M-only code
    tests/                host tests (plain assert, run via ctest)
    docs/                 module notes

## Modules

| Module | Header | Platform |
|---|---|---|
| Cycle arithmetic / conversion | `embedtools/time_cycles.h` | portable |
| DWT cycle counter | `embedtools/time_dwt.h` | ARM Cortex-M (M3/M4/M7/M33/M55) |

See [docs/time_dwt.md](docs/time_dwt.md).

## Use as a submodule

The library has no generated files and needs no defines, so it can be used
from CMake or from an IDE-managed build (STM32CubeIDE, Keil, IAR, ...).

### CMake

    add_subdirectory(extern/EmbedTools)
    target_link_libraries(app PRIVATE embedtools_time_dwt)

Set `-DEMBEDTOOLS_ARM_CORTEX_M=ON` for firmware builds. Without it only the
portable core is built. Host tests are built only when EmbedTools is the
top-level project.

### Managed build (no CMake)

1. Add the include path `extern/EmbedTools/include`.
2. Compile these files (C11):
   - `src/time_cycles.c`
   - `platform/arm_cortex_m/time_dwt.c`
3. `#include "embedtools/time_dwt.h"`.

No HAL, CMSIS, device header or math library is required. Compile only the
platform files you use: sources under `platform/` are target-specific.

## Host tests

    cmake -S . -B build && cmake --build build && ctest --test-dir build
