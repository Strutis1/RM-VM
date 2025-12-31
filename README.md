# Operating System Project

A minimal Operating System project implemented in C. This repository contains the OS kernel, device drivers, utilities, source, headers, an `asm/` directory, a `bin/` output directory, and a small test suite. The project is set up with a `Makefile`.

---

## Overview

This project models a simple operating system environment, suitable for learning and experimentation. It includes components for running programs, managing resources, and handling basic input/output. The structure allows easy extension and testing of OS concepts.

---

## Repo layout

```
asm/       ; assembly or bytecode/program inputs for the machines
bin/       ; build artifacts (executables, images)
include/   ; public headers
src/       ; C sources
rm_test/   ; sample programs / fixtures / scripts / kernel implementation
Makefile   ; build targets
```

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

## Features

- Basic OS functionality for running programs
- Sample programs demonstrating OS interactions
- Low-level routines for initialization and I/O
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