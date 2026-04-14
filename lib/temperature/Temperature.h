#pragma once

struct Temperature {

    enum SensorStatus {
        OK,
        DISCONNECTED,
        SHORT_TO_GND,
        SHORT_TO_VCC,
        INVALID
    };

    float value;
    SensorStatus status;

    static const char* getName(SensorStatus s) {
        switch (s) {
            case OK: return "OK";
            case DISCONNECTED: return "DISCONNECTED";
            case SHORT_TO_GND: return "SHORT_TO_GND";
            case SHORT_TO_VCC: return "SHORT_TO_VCC";
            case INVALID: return "INVALID";
            default: return "UNKNOWN";
        }
    }
};