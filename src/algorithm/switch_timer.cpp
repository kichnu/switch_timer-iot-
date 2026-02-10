#include "switch_timer.h"
#include "../core/logging.h"

void SwitchTimer::init() {
    pinMode(INPUT_PIN, INPUT_PULLUP);
    pinMode(OUTPUT_PIN, OUTPUT);
    digitalWrite(OUTPUT_PIN, OUTPUT_IDLE_STATE);

    _outputState = OutputState::IDLE;
    _cycleEndTime = 0;
    _lastStableState = INPUT_INACTIVE_STATE;
    _debouncing = false;
    _inputHeld = false;

    LOG_INFO("SwitchTimer initialized - INPUT: GPIO%d, OUTPUT: GPIO%d", INPUT_PIN, OUTPUT_PIN);
    LOG_INFO("Cycle time: %ds, Start max: %dms, Stop min: %dms", CYCLE_TIME_S, START_MAX_MS, STOP_MIN_MS);
}

void SwitchTimer::update() {
    // Process physical button input
    InputEvent event = processInput();
    if (event != InputEvent::NONE) {
        handleEvent(event);
    }

    // Check timer expiry for ACTIVE state
    if (_outputState == OutputState::ACTIVE && millis() >= _cycleEndTime) {
        LOG_INFO("Timer expired - switching to IDLE");
        deactivateOutput();
        _outputState = OutputState::IDLE;
    }
}

// ============================================
// INPUT PROCESSING (debounce + classification)
// ============================================

InputEvent SwitchTimer::processInput() {
    bool currentRead = digitalRead(INPUT_PIN);

    // Start debounce on state change
    if (currentRead != _lastStableState && !_debouncing) {
        _debouncing = true;
        _debounceStartTime = millis();
        _pendingState = currentRead;
        return InputEvent::NONE;
    }

    // Wait for debounce interval
    if (_debouncing) {
        if (millis() - _debounceStartTime < DEBOUNCE_INTERVAL_MS) {
            return InputEvent::NONE;
        }

        // Confirm debounce
        bool confirmRead = digitalRead(INPUT_PIN);
        _debouncing = false;

        if (confirmRead != _pendingState) {
            return InputEvent::NONE; // Noise, discard
        }

        _lastStableState = confirmRead;

        // Button pressed (active LOW)
        if (confirmRead == INPUT_ACTIVE_STATE) {
            _inputHeld = true;
            _pressStartTime = millis();
            return InputEvent::NONE;
        }

        // Button released
        if (confirmRead == INPUT_INACTIVE_STATE && _inputHeld) {
            _inputHeld = false;
            unsigned long pressDuration = millis() - _pressStartTime;

            if (pressDuration < START_MAX_MS) {
                LOG_INFO("Physical button: START (held %lums)", pressDuration);
                return InputEvent::START;
            } else if (pressDuration >= STOP_MIN_MS) {
                LOG_INFO("Physical button: STOP (held %lums)", pressDuration);
                return InputEvent::STOP;
            }
            // Dead zone (START_MAX_MS..STOP_MIN_MS) - ignored
            LOG_INFO("Physical button: dead zone (held %lums) - ignored", pressDuration);
        }
    }

    return InputEvent::NONE;
}

// ============================================
// STATE TRANSITIONS
// ============================================

void SwitchTimer::handleEvent(InputEvent event) {
    switch (_outputState) {
        case OutputState::IDLE:
            if (event == InputEvent::START) {
                activateOutput();
                _cycleEndTime = millis() + CYCLE_TIME_MS;
                _outputState = OutputState::ACTIVE;
                LOG_INFO("IDLE -> ACTIVE (timer: %ds)", CYCLE_TIME_S);
            }
            break;

        case OutputState::ACTIVE:
            if (event == InputEvent::START) {
                _cycleEndTime += CYCLE_TIME_MS;
                unsigned long remaining = (_cycleEndTime - millis()) / 1000;
                LOG_INFO("ACTIVE: timer extended (remaining: %lus)", remaining);
            } else if (event == InputEvent::STOP) {
                deactivateOutput();
                _outputState = OutputState::IDLE;
                LOG_INFO("ACTIVE -> IDLE (manual stop)");
            }
            break;

        case OutputState::PERMANENT:
            if (event == InputEvent::STOP) {
                deactivateOutput();
                _outputState = OutputState::IDLE;
                LOG_INFO("PERMANENT -> IDLE (manual stop)");
            }
            // START ignored in PERMANENT mode
            break;
    }
}

// ============================================
// WEB CONTROL METHODS
// ============================================

void SwitchTimer::webAddTime() {
    switch (_outputState) {
        case OutputState::IDLE:
            activateOutput();
            _cycleEndTime = millis() + CYCLE_TIME_MS;
            _outputState = OutputState::ACTIVE;
            LOG_INFO("Web: IDLE -> ACTIVE (timer: %ds)", CYCLE_TIME_S);
            break;

        case OutputState::ACTIVE: {
            _cycleEndTime += CYCLE_TIME_MS;
            unsigned long remaining = (_cycleEndTime - millis()) / 1000;
            LOG_INFO("Web: ACTIVE timer extended (remaining: %lus)", remaining);
            break;
        }

        case OutputState::PERMANENT:
            LOG_INFO("Web: Add Time ignored (PERMANENT mode)");
            break;
    }
}

void SwitchTimer::webForceOn() {
    activateOutput();
    _outputState = OutputState::PERMANENT;
    _cycleEndTime = 0;
    LOG_INFO("Web: -> PERMANENT (force ON)");
}

void SwitchTimer::webForceOff() {
    deactivateOutput();
    _outputState = OutputState::IDLE;
    _cycleEndTime = 0;
    LOG_INFO("Web: -> IDLE (force OFF)");
}

// ============================================
// OUTPUT CONTROL
// ============================================

void SwitchTimer::activateOutput() {
    digitalWrite(OUTPUT_PIN, OUTPUT_ACTIVE_STATE);
}

void SwitchTimer::deactivateOutput() {
    digitalWrite(OUTPUT_PIN, OUTPUT_IDLE_STATE);
}

// ============================================
// STATE QUERIES
// ============================================

OutputState SwitchTimer::getOutputState() const {
    return _outputState;
}

unsigned long SwitchTimer::getRemainingMs() const {
    if (_outputState != OutputState::ACTIVE) {
        return 0;
    }
    unsigned long now = millis();
    if (now >= _cycleEndTime) {
        return 0;
    }
    return _cycleEndTime - now;
}

const char* SwitchTimer::getOutputStateString() const {
    switch (_outputState) {
        case OutputState::IDLE:      return "idle";
        case OutputState::ACTIVE:    return "active";
        case OutputState::PERMANENT: return "permanent";
        default:                     return "unknown";
    }
}

bool SwitchTimer::isRelayOn() const {
    return _outputState != OutputState::IDLE;
}
