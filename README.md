
# Memory Allocator in C

A dynamic memory allocator implemented from scratch in C, based on CS:APP Chapter 9.

## What it does
Manages raw heap memory the same way malloc/free works at low level in real systems.

## Implementation
- Implicit free list with boundary tags (header + footer per block)
- First-fit placement method is used
- Immediate coalescing across all 4 neighbor cases
- Built on top of mem_sbrk (simulated heap)

## Performance
- Phase 1 (implicit free list) i got " 1272750 ops/sec"

## Build and Run
gcc -Wall -O2 -m32 -o test test.c mm.c memlib.c
./test

## Reference
Theory from Bryant and O'Hallaron, Computer Systems Book
