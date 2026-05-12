#include "ButtonSensor.h"

ButtonSensor::ButtonSensor(uint8_t pin,
               uint16_t debounceMs,     // debounce window: suppresses signal bouncing after state change
               uint16_t doubleClickMs,  // time window to detect second press for double click
               uint16_t longClickMs)    // press duration threshold for long click event
: _pin(pin),
  _debounceMs(debounceMs),
  _doubleClickMs(doubleClickMs),
  _longClickMs(longClickMs),
  _lastRawState(HIGH),
  _stableState(HIGH),
  _lastChangeTime(0),
  _pressTime(0),
  _releaseTime(0),
  _lastClickTime(0),
  _waitingSecondClick(false),
  _event(BUTTON_NONE)
{
    pinMode(_pin, INPUT_PULLUP); // Button not pressed -> HIGH, Button pressed -> LOW
}

void ButtonSensor::update() {
    uint32_t now = millis();
    bool raw = digitalRead(_pin);

    // --- debounce ---
    if (raw != _lastRawState) {
        _lastChangeTime = now;
        _lastRawState = raw;
    }

    if ((now - _lastChangeTime) > _debounceMs) {
        if (_stableState != raw) {
            _stableState = raw;

            // pressed (LOW because pullup)
            if (_stableState == LOW) {
                _pressTime = now;
            } 
            else {
                // released (HIGH because pullup)
                _releaseTime = now;
                uint32_t pressDuration = _releaseTime - _pressTime;

                // LONG has priority
                if (pressDuration >= _longClickMs) {
                    setEvent(BUTTON_LONG_CLICK);
                    _waitingSecondClick = false;
                }
                else {
                    // handle click / double click
                    if (_waitingSecondClick &&
                        (now - _lastClickTime) <= _doubleClickMs) {

                        setEvent(BUTTON_DOUBLE_CLICK);
                        _waitingSecondClick = false;
                    }
                    else {
                        _waitingSecondClick = true;
                        _lastClickTime = now;
                    }
                }
            }
        }
    }

    // --- resolve single click after timeout ---
    if (_waitingSecondClick) {
        if ((now - _lastClickTime) > _doubleClickMs) {
            setEvent(BUTTON_CLICK);
            _waitingSecondClick = false;
        }
    }
}

ButtonEvent ButtonSensor::getEvent() const {
    return _event;
}

void ButtonSensor::clearEvent() {
    _event = BUTTON_NONE;
}

bool ButtonSensor::isPressed() const {
    return _stableState == LOW;
}

void ButtonSensor::setEvent(ButtonEvent e) {
    _event = e;
}