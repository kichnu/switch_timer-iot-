#ifndef SWITCH_TIMER_H
#define SWITCH_TIMER_H

#include <Arduino.h>
#include "../conf.h"

enum class OutputState {
    IDLE,
    ACTIVE,
    PERMANENT
};

enum class InputEvent {
    NONE,
    START,
    STOP
};

class SwitchTimer {
public:
    void init();
    void update();

    // State queries
    OutputState getOutputState() const;
    unsigned long getRemainingMs() const;
    const char* getOutputStateString() const;
    bool isRelayOn() const;

    // Web control methods
    void webAddTime();
    void webForceOn();
    void webForceOff();

private:
    InputEvent processInput();
    void handleEvent(InputEvent event);
    void activateOutput();
    void deactivateOutput();

    // Output state
    OutputState _outputState = OutputState::IDLE;
    unsigned long _cycleEndTime = 0;

    // Input debounce
    bool _lastStableState = INPUT_INACTIVE_STATE;
    bool _debouncing = false;
    unsigned long _debounceStartTime = 0;
    bool _pendingState = INPUT_INACTIVE_STATE;

    // Press duration tracking
    bool _inputHeld = false;
    unsigned long _pressStartTime = 0;
};

#endif
