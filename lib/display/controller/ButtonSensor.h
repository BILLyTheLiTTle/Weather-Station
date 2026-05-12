#pragma once

#include <Arduino.h>

enum ButtonEvent {
    BUTTON_NONE = 0,
    BUTTON_CLICK,
    BUTTON_DOUBLE_CLICK,
    BUTTON_LONG_CLICK
};

class ButtonSensor {
public:
    ButtonSensor(uint8_t pin,
           uint16_t debounceMs = 20,
           uint16_t doubleClickMs = 250,
           uint16_t longClickMs = 600);

    void update();

    ButtonEvent getEvent() const;
    void clearEvent();

    bool isPressed() const;

private:
    uint8_t _pin;               // input pin where the button is connected

    uint16_t _debounceMs;       // debounce time (ms) to filter mechanical bouncing
    uint16_t _doubleClickMs;    // max interval (ms) between two clicks for double click detection
    uint16_t _longClickMs;      // minimum press duration (ms) to trigger a long click

    // raw / stable
    bool _lastRawState;         // last immediate (raw) reading from the pin
    bool _stableState;          // debounced (stable) button state

    uint32_t _lastChangeTime;   // timestamp of last raw state change (used for debounce)

    // click logic
    uint32_t _pressTime;        // timestamp when button was pressed
    uint32_t _releaseTime;      // timestamp when button was released
    uint32_t _lastClickTime;    // timestamp of last click (used for double click detection)

    bool _waitingSecondClick;   // flag indicating we're waiting for a second click

    ButtonEvent _event;         // last detected event (click / double / long)

    void setEvent(ButtonEvent e);
};