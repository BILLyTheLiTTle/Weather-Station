#pragma once

struct Temperature {

    enum SensorStatus : uint8_t {
        OK = 0,
        DISCONNECTED,
        SHORT_TO_GND,
        SHORT_TO_VCC,
        INVALID
    };

    int16_t value;
    SensorStatus status;

    static const __FlashStringHelper* getName(SensorStatus s) {
        switch (s) {
            case OK: return F("OK");
            case DISCONNECTED: return F("DISCONNECTED");
            case SHORT_TO_GND: return F("SHORT_TO_GND");
            case SHORT_TO_VCC: return F("SHORT_TO_VCC");
            case INVALID: return F("INVALID");
            default: return F("UNKNOWN");
        }
    }
};