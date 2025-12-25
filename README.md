# RM-VM

Minimal “real machine” and a matching virtual machine implemented in C. The project includes source, headers, an `asm/` directory, a `bin/` output directory, and a small test suite. It’s set up with a `Makefile`. 

---

## Overview

This repository models a simple hardware-like **Real Machine (RM)** and a software **Virtual Machine (VM)** that executes the same instruction set. The VM emulates the RM’s behavior for easier debugging and experimentation, while the RM version represents the “bare-metal” implementation path. Folder layout and languages are visible in the repo tree. 

---

## Repo layout

```
asm/       ; assembly or bytecode/program inputs for the machines
bin/       ; build artifacts (executables, images)
include/   ; public headers
src/       ; C sources
rm_test/   ; sample programs / fixtures / scripts
Makefile   ; build targets
```

Folder names and presence are taken from the repository browser. 

---

## Prerequisites

* A POSIX-like environment (Linux/macOS or WSL)
* `make`
* `gcc` or `clang`

---

## Build

```bash
make
```

Common targets you can expect in a project like this:

```bash
make all
make clean
make debug
```

(Exact target names depend on the `Makefile` in this repo.) 

## Instruction set & I/O

Document here:

* Word size, endianness
* Register file (names, sizes, caller/callee convention if any)
* Condition codes / flags
* Supported addressing modes
* Syscalls / memory-mapped I/O
* Trap/interrupt behavior
* Program image format (loader expectations)

---

## Development

* Code lives in `src/`, public headers in `include/`.
* Keep the VM and RM behavior bit-for-bit compatible where possible.
* Add new opcodes by updating:

  1. the shared ISA definition,
  2. the RM execution path, and
  3. the VM interpreter/JIT path.


---

## License

MIT License — fork, modify, and contribute freely.

---