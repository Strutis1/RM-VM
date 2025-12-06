# RM-VM Architecture & Usage (loader/scheduler intentionally excluded)

## Repository map (only the parts described here)
- `src/main.c` — entry point and demo program installation.
- `src/utils/` — logging and interactive startup configuration.
- `src/rm/` — Real Machine: CPU, memory, interrupts, timer, disk, channel device, IO facade.
- `src/vm/` — Virtual Machine: VM CPU, VM memory, VM/channel glue, VM lifecycle.
- `include/` — shared constants, structs, and opcode/interrupt definitions.
- `asm/` — placeholders (empty) for startup/interrupt/context-switch assembly.
- `Makefile` — builds into `bin/rmvm`, with objects in `build/`.
- `NUL`, `rm_test`, `README.md` — empty placeholders at the root.

## Core constants and invariants (`include/common.h`, `include/instructions.h`, `include/interrupts.h`)
- Word/register size: 16-bit; `REG_COUNT = 8` general registers.
- Physical memory: `MEMORY_SIZE = 512` words, split into:
  - User space: `0..255` (`USER_MEMORY_START/END`).
  - Supervisor space: `256..511` (`SUPERVISOR_MEMORY_START/END`).
  - Any address outside these ranges raises `PI_INVALID_ADDRESS`.
- VM memory: `VM_MEMORY_SIZE = 0xFF` bytes, independent of physical memory layout.
- CPU modes: `MODE_USER` and `MODE_SUPERVISOR`. Supervisor gates privileged ops and supervisor memory access.
- Timer: `TICK_LIMIT = 10` simulated ticks before raising a timer interrupt.
- Flags: `FLAG_ZERO`, `FLAG_NEGATIVE`, `FLAG_OVERFLOW`, `FLAG_CARRY` share the single `SF` field.
- Interrupt codes:
  - Program interrupts (PI): invalid opcode/address/register, div-by-zero, privilege violation, overflow.
  - System interrupts (SI): read/write/halt/sys requests.
  - Timer interrupts (TI): `TI_EXPIRED` when the counter hits its limit.

## System walk-through (from process start to halt)
1. `main()` (`src/main.c`) calls `generateConfig()` to select logging behavior before any logging occurs.
2. `initRealMachine()` (`src/rm/rm.c`) zeros physical memory, initializes CPU registers and flags, zeroes disk, and primes the IO channel.
3. Demo program is written directly into `physicalMemory.cells[0..3]` as raw instruction words; CPU mode is set to supervisor, `IC` is reset.
4. `execCycle()` (`src/rm/cpu.c`) loops: `fetch` → `decode` → `execute` until an interrupt is latched.
5. On interrupt, `handleInterrupts()` logs/clears PI/SI/TI; `SI_HALT` ends the loop, stopping the CPU.
6. VM creation is not currently wired into `main`; VM paths run only when `createVM()` is called manually by another component.

## Real Machine: data structures and lifecycles
- `CPU` (`include/registers.h`): instruction counter `IC`, eight general registers `R[]`, status flags `SF`, privilege `MODE`, interrupt latches `PI/SI/TI`, page-table pointer `PTR`.
- `Memory` (`include/memory.h`): 512-word array. Accessors (`read`, `write`) enforce region + privilege rules; `readUser`/`writeUser` and `readSupervisor`/`writeSupervisor` bypass checks for channel ops.
- `HardDisk` (`include/disk.h`): 128 sectors × 16 bytes; `busy`, `ready`, `interruptFlag`, and `head` track device state.
- `ChannelDevice` (`include/channel_device.h`): describes a transfer using selectors `ST` (source type) and `DT` (destination type), bases `SB/DB`, `COUNT`, `OFFSET`, `DATA`, and `busy`.
- `Timer` (`include/timer.h`): simple counter with `limit`, `active`, and `interruptFlag`.

### CPU pipeline (`src/rm/cpu.c`)
- `initCPU`: resets registers, flags, sets supervisor mode, clears interrupts, sets `PTR` to `PTR_START`.
- `fetch`: reads a word from `physicalMemory` at `IC` (respecting privilege rules), increments `IC`, stops on address faults.
- `decode`: unpacks `[15:12]=opcode`, `[11:9]=regA`, `[8:7]=mode`, `[6:0]=operand` (or `regB` for ALU ops, `addr` for jumps). Validation sets `PI` on any illegal opcode/register/address/mode.
- `execute`: semantics by opcode (all operate on `realCPU` and `physicalMemory`):
  - `LOAD`: immediate, direct, or double-indirect.
  - `STORE`: direct or double-indirect.
  - `ADD/SUB/MUL/DIV/CMP`: register-register ALU with flag updates; overflow/div-zero set PI.
  - `JMP/JZ/JNZ`: direct/indirect/double-indirect control flow; `JZ/JNZ` test `FLAG_ZERO`.
  - `READ/WRITE/HALT`: supervisor-only; user attempts raise `PI_INVALID_OPCODE`; on success set SI.
  - `NOP/SYS`: `SYS` sets `SI_SYS`; `NOP` does nothing.
- `execCycle`: loops fetch/decode/execute while no interrupts are latched; ticks a software timer (`TICK_LIMIT`); exits when `handleInterrupts` returns 0 (halt).
- `handleInterrupts`: prints SI/PI/TI conditions, clears them, and terminates when `SI_HALT` is set.

### Memory rules (`src/rm/memory.c`)
- Supervisor-only addresses (`>=256`) raise `PI_INVALID_ADDRESS` when accessed in user mode.
- No paging/translation is active despite `PTR` existing; addresses are direct.
- Helper accessors without privilege checks are used only by the channel layer to avoid recursive faults.

### Interrupt plumbing (`src/rm/interrupt.c`)
- `raiseProgramInterrupt`, `raiseTimerInterrupt`, `raiseSystemInterrupt` set `realCPU.PI/TI/SI` respectively (with bounds checks).
- `hasPendingInterrupt` is a quick “is anything latched?” helper; `clearAllInterrupts` zeros all latches.

### Timer model (`src/rm/timer.c`)
- `initTimer` arms a timer with `limit`; `tickTimer` increments and sets `interruptFlag` at limit, then rolls over to 0.
- `checkTimerInterrupt` returns true once per limit interval; `resetTimer` clears both counter and flag.

### Disk model (`src/rm/disk.c`)
- Sector-level API: `readDisk`/`writeDisk` copy full 16-byte sectors, toggle `busy`, and set `interruptFlag` to signal completion.
- `diskReadWord` is a convenience: returns the first 16-bit word of a sector (used by channel transfers).
- `simulateDiskInterrupt` clears the disk’s `interruptFlag`; no asynchronous signaling is present.

### Channel device and IO facade
- `ChannelDevice` selectors (`include/channel_device.h`):
  - Sources: `CH_SRC_USER_MEM`, `CH_SRC_SUPER_MEM`, `CH_SRC_DISK`, `CH_SRC_IO`, `CH_SRC_CPU_REG`.
  - Destinations: `CH_DST_USER_MEM`, `CH_DST_SUPER_MEM`, `CH_DST_DISK`, `CH_DST_IO`, `CH_DST_CPU_REG`.
- `channelXCHG` (`src/rm/channel_device.c`) behavior:
  - User→Supervisor: copy `COUNT` words from user to supervisor addresses.
  - User→IO: print each user word as hex; set `cpu->SI = SI_WRITE`.
  - Supervisor→User: copy `COUNT` words from supervisor to user addresses.
  - Disk→Supervisor: copy `COUNT` words via `diskReadWord`; set `cpu->SI = SI_READ`.
  - IO→CPU register: prompt stdin, store hex word into `cpu->R[0]`; set `cpu->SI = SI_READ`.
  - Invalid `ST` sets `cpu->PI = PI_INVALID_ADDRESS`; `busy` wraps the entire transfer.
- IO facade (`src/rm/io.c`):
  - `IOinit` initializes a static channel + disk (and zeroes disk contents).
  - `IOread`: configures disk→supervisor transfer and raises `SI_READ` on success.
  - `IOwrite`: configures supervisor→disk transfer and raises `SI_WRITE` on success.
  - `IOcheckInterrupts`: clears `realCPU.SI` when the channel is idle (polling hook for schedulers).

## Virtual Machine layer
- Structures:
  - `VirtualMachine` (`src/vm/vm.h`): holds a `Channel*`, `VM_MEMORY*`, and `VM_CPU*`.
  - `VM_CPU` (`include/registers.h`): 8 general registers, `PC`, `SF`, `DS/CS`, `PTR`, `SI`.
  - `VM_MEMORY` (`src/vm/vm_memory.h`): flat byte buffer of `VM_MEMORY_SIZE`.
- VM lifecycle (`src/vm/vm.c`):
  - `createVM(channel)`: allocates `VirtualMachine`; initializes VM memory/CPU; reads one disk sector through the provided channel into supervisor memory; copies that sector into VM memory; calls `runOperations(vm)` immediately.
  - No scheduler integration yet; execution is synchronous and blocking.

### VM memory and loaders (`src/vm/vm_memory.c`)
- `VMinitMemory` is currently broken: it uses an uninitialized pointer; it must allocate (e.g., `calloc(1, sizeof(VM_MEMORY))`) before `memset`.
- `loadProgram` writes an `Instruction[]` into consecutive 9-byte slots until it encounters `OP_HALT`; returns 1 on overflow beyond `VM_MEMORY_SIZE`.
- `stuffInstruction` (unused helper) decodes a 16-bit word into `Instruction` fields and validates opcode/register/mode limits.

### VM instruction frame and execution (`src/vm/vm_cpu.c`)
- Instruction storage (per 9-byte slot):
  ```
  [0] opcode
  [1] regA
  [2] regB
  [3] mode
  [4] operand high byte
  [5] operand low byte
  [6] raw high byte
  [7] raw low byte
  [8] length (always 9 in current code)
  ```
- `fetch` grabs a 9-byte frame at `PC`; out-of-bounds sets `PI_INVALID_ADDRESS` and aborts.
- Addressing in `runOperations`:
  - `LOAD` sources: immediate operand; absolute memory word; register value; register+displacement.
  - `STORE` destinations: absolute memory; register+displacement.
  - `ADD/SUB/MUL/DIV/CMP` are register-register; flags come from `sf_set` plus carry/overflow bits when relevant.
  - `JMP/JZ/JNZ` set `PC` to `operand * 9` so the operand indexes instructions, not bytes.
  - `READ/WRITE/SYS/HALT` raise system interrupts via RM helpers and return to caller.
- Memory helpers:
  - `vm_mem_read16`/`vm_mem_write16` enforce bounds and operate on byte-packed words.
  - `sf_set` sets zero/negative; carry/overflow are added explicitly by arithmetic handlers.

### VM channel bridge (`src/vm/vm_channel.c`)
- `Channel` embeds a `ChannelDevice` plus bookkeeping:
  - `dst_base`: supervisor memory base where disk sectors land (default `SUPERVISOR_MEMORY_START`).
  - `last_buffer`/`last_qword_count`: cache of most recent transfer, freed on next read.
- `readChannel`: configures disk→supervisor transfer, copies `DISK_SECTOR_SIZE` bytes into a heap buffer (as `uint64_t[]`), caches, returns pointer.
- `writeChannel`: writes cached buffer back to disk via supervisor→disk transfer. Fails if no cached buffer exists.

## Instruction reference (RM vs VM)
- RM opcodes (16-bit words):
  - `HALT`: supervisor-only; sets `SI_HALT`.
  - `LOAD/STORE`: honor addressing mode; invalid mode raises `PI_INVALID_OPCODE`.
  - `ADD/SUB/MUL/DIV/CMP`: operate on `R[regA]` and `R[regB]`; overflow/div-zero latch PI.
  - `JMP/JZ/JNZ`: operand is word address; indirect forms reread memory.
  - `READ/WRITE`: supervisor-only; set `SI_READ/SI_WRITE`.
  - `NOP`, `SYS`: `SYS` sets `SI_SYS`.
- VM opcodes (9-byte frames):
  - Same logical set, but all operands/addresses are byte-based; jumps multiply operand by 9 to land on instruction boundaries.
  - System calls and HALT abort the VM loop after raising SI in the RM.

## Logging and startup configuration (`src/utils/logger.c`, `src/utils/utils.h`, `src/main.c`)
- `_log` routes through `logFuncPtr`, defaulting to stdout. `g_fname` holds the current log filename (max 16 chars).
- `generateConfig()` prompt options:
  1. stdout only
  2. custom file only (prompts for name; rejects names longer than 16 or containing spaces/newlines)
  3. default file only (`default_log.log`)
  4. stdout + default file
  5. stdout + custom file (prompts for name)
- Invalid selection or invalid file name falls back to the default file and logs an error via `_log`.
- `main()` calls `generateConfig` before any other logging to ensure `_log` has a destination.

## Demo program (RM-side) in `src/main.c`
- Installs four instruction words directly into `physicalMemory`:
  - `0x1001`: `LOAD R0, #1` (immediate)
  - `0x2002`: `LOAD R1, #2` (immediate)
  - `0x3001`: `ADD R0, R1`
  - `0x0000`: `HALT`
- Puts CPU into supervisor mode to allow HALT, zeroes `IC`, and runs `execCycle()` until `SI_HALT` stops the loop. Demonstrates fetch/decode/execute and interrupt handling.

## Build, run, and observability
- Build: `make` → `bin/rmvm`; intermediates in `build/`; `make clean` purges both.
- Runtime: binary prompts for logging mode, initializes RM hardware emulation, runs the demo.
- Debug aids:
  - `cpu.c` prints each fetch (`[DEBUG] Fetch @...`) and interrupt handling messages.
  - `channel_device.c` logs each transfer with source/destination selectors and counts.
  - `io.c` logs disk read/write completion and raised interrupts.

## Limitations, hazards, and TODOs
- VM memory initialization bug: `VMinitMemory` dereferences an uninitialized pointer; must allocate before use or every VM instantiation will crash.
- VM is not integrated into `main` or any scheduler; creating a VM must be done manually by new code.
- Interrupt handling is minimal: SI_READ/SI_WRITE are only logged; timer and disk interrupt flags are not surfaced to higher-level schedulers or handlers.
- `IOinit` reinitializes the disk (zeroing storage); calling it after writing data wipes the disk image.
- Channel transfers assume full-sector moves; partial-length or offset transfers are unsupported.
- Privilege enforcement only covers memory and certain opcodes; no paging/translation yet despite `PTR` field.
- Assembly stubs in `asm/` are empty placeholders; no boot or context-switch code is active.
