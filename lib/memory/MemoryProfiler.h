/*
====================================================
 AVR MEMORY MODEL (SIMPLIFIED BUT CORRECT)
====================================================

==========================
 FLASH (PROGRAM MEMORY)
==========================
- Non-volatile memory (code storage)
- CPU executes from here

  .text
    -> compiled program instructions

  .rodata
    -> constant data (string literals, const globals)
    -> e.g. "hello", lookup tables

NOTE:
- Flash is NOT directly RAM-accessible as normal pointers
- Special instructions are used to read it

==========================
 SRAM (NORMAL RAM - 2KB on ATmega328P)
==========================

LOW MEMORY (0x0000)
|
|  .data
|    -> global/static variables WITH initial values
|    -> copied from Flash at startup
|
|  .bss
|    -> global/static variables initialized to 0
|    -> cleared at startup
|
|  HEAP
|    -> dynamic memory (malloc/free)
|    -> grows upward
|
|  FREE RAM
|    -> unused SRAM between heap and stack
|
|  STACK
|    -> function calls + local variables
|    -> grows downward
|
HIGH MEMORY (end of SRAM)

==========================
 POINTER MODEL IN SRAM
==========================
- normal pointers live in SRAM:
    char*, int*, struct*
- they point to SRAM addresses by default

IMPORTANT:
- Flash pointers are NOT normal pointers
- they require special handling (e.g. F(), pgm_read)

==========================
 LINKER SYMBOLS
==========================
__bss_start
    -> start of .bss section

__bss_end
    -> end of .data + .bss
    -> start of heap region

__heap_start
    -> first address available for heap allocation

__brkval
    -> current heap end (0 if heap unused)

==========================
 IMPORTANT RULES
==========================

1. FLASH holds code + constants (.text, .rodata)
2. SRAM holds runtime state (.data, .bss, heap, stack)
3. .data is copied from FLASH into SRAM at startup
4. heap and stack share same SRAM space (grow towards each other)
5. stack overflow can corrupt heap and globals
6. memory is not protected (no MMU on AVR)
*/

#pragma once

#include <Arduino.h>

class MemoryProfiler {
public:
    explicit MemoryProfiler(size_t ramSizeBytes);

    void begin();   // initialize watermark

    size_t getHeapUsed() const;
    size_t getFreeRam() const;
    size_t getStackUsedApprox() const;

    size_t getUsedRamApprox() const;

// private:
    // void fillWatermark();

private:
    size_t _ramSize;

    // linker symbols (AVR). Not the actual linker symbol, but my reference to them!
    static int _heapStart;
    static int *_brkval;
};