#include "MemoryProfiler.h"

// AVR linker symbols
extern int __heap_start;
extern int *__brkval;
extern int __bss_end;

// static binding (AVR-specific)
int MemoryProfiler::_heapStart = (int)&__heap_start;
int *MemoryProfiler::_brkval = __brkval;

#define STACK_CANARY 0xAA

// ---------------- constructor ----------------
MemoryProfiler::MemoryProfiler(size_t ramSizeBytes)
    : _ramSize(ramSizeBytes) {}

// ---------------- init ----------------
void MemoryProfiler::begin() {
    // fillWatermark();
}

// ---------------- heap ----------------
size_t MemoryProfiler::getHeapUsed() const {
    return (_brkval == 0)
        ? 0
        : (size_t)_brkval - (size_t)&__heap_start;
}

// ---------------- free RAM ----------------
size_t MemoryProfiler::getFreeRam() const {
    int v;
    return (size_t)&v - (size_t)&__heap_start;
}

// ---------------- stack estimate ----------------
size_t MemoryProfiler::getStackUsedApprox() const {
    uint8_t *p = (uint8_t*)&__heap_start;
    size_t used = 0;

    while (p > (uint8_t*)&__bss_end && *p == STACK_CANARY) {
        p--;
        used++;
    }

    return used;
}

// ---------------- total used ----------------
size_t MemoryProfiler::getUsedRamApprox() const {
    return _ramSize - getFreeRam();
}

// ---------------- watermark ----------------
// void MemoryProfiler::fillWatermark() {
//     uint8_t *p = (uint8_t*)&__bss_end;

//     while (p < (uint8_t*)&__heap_start) {
//         *p++ = STACK_CANARY;
//     }
// }