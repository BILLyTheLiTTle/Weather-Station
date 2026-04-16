#include "MemoryProfiler.h"

// AVR linker symbols
extern int __bss_end;
extern int *__brkval;

// ---------------- constructor ----------------
MemoryProfiler::MemoryProfiler(size_t ramSizeBytes)
    : _ramSize(ramSizeBytes) {}

// ---------------- heap ----------------
// Heap grows from end of .bss upwards
size_t MemoryProfiler::getHeapUsed() {
    if (__brkval == 0) return 0;
    return (size_t)__brkval - (size_t)&__bss_end;
}

// ---------------- free RAM ----------------
// Measures space between heap end and current stack position
size_t MemoryProfiler::getFreeRam() const {
    if (__brkval == 0) {
        return (size_t)SP - (size_t)&__bss_end;
    } else {
        return (size_t)SP - (size_t)__brkval;
    }
}

// ---------------- used RAM (approx) ----------------
// Derived from total SRAM minus free space
size_t MemoryProfiler::getUsedRamApprox() const {
    return _ramSize - getFreeRam();
}

// ---------------- stack pointer (debug helper) ----------------
size_t MemoryProfiler::getStackPointer() const {
    return SP;
}

// ---------------- heap pointer (debug helper) ----------------
size_t MemoryProfiler::getHeapEnd() const {
    return (size_t)__brkval;
}