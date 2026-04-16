/*
==========================
 AVR MEMORY LAYOUT (SIMPLIFIED)
==========================

LOW MEMORY (0x0000)
|
|  .data
|    -> initialized global variables
|    -> copied from flash to RAM at startup
|
|  .bss
|    -> uninitialized global/static variables
|    -> automatically zeroed at startup
|
|  __bss_end
|    -> end of all global/static RAM usage
|    -> start of "free RAM area"
|
|  FREE RAM
|    -> used by heap + stack
|
|  HEAP (malloc)
|    -> grows upward (towards higher addresses)
|    -> starts at __heap_start
|
|  STACK
|    -> grows downward (towards lower addresses)
|    -> contains function frames + local variables
|
HIGH MEMORY (RAM end)

==========================
 LINKER SYMBOLS
==========================

__bss_start
    -> start of .bss section

__bss_end
    -> end of .data + .bss
    -> start boundary for dynamic memory area

__heap_start
    -> first address available for heap allocation

__brkval
    -> current end of heap (0 if heap unused)

==========================
 IMPORTANT RULES
==========================

1. Globals are allocated BEFORE main() runs.
2. Heap and stack share remaining RAM.
3. Stack overflow can overwrite heap and globals.
4. Heap fragmentation is possible over time.
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