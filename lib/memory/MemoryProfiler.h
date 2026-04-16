#pragma once

#include <Arduino.h>

class MemoryProfiler {
public:
    explicit MemoryProfiler(size_t ramSizeBytes);

    // heap tracking
    size_t getHeapUsed();

    // RAM usage
    size_t getFreeRam() const;
    size_t getUsedRamApprox() const;

    // debug helpers
    size_t getStackPointer() const;
    size_t getHeapEnd() const;

private:
    size_t _ramSize;
};