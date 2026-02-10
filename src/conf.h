#ifndef CONF_H
#define CONF_H

// ============== PIN CONFIGURATION ==============
#define INPUT_PIN               3       // GPIO3 - wejście sterujące (active LOW, zwarcie do masy)
#define OUTPUT_PIN              4       // GPIO4 - wyjście sterujące (active HIGH)

// ============== INPUT DEBOUNCE ==============
#define DEBOUNCE_INTERVAL_MS    50      // Przerwa między dwoma pomiarami [ms]

// ============== INPUT CLASSIFICATION ==============
#define START_MAX_MS            1500    // Aktywny INPUT < 1500 ms = START
#define STOP_MIN_MS             3000    // Aktywny INPUT > 3000 ms = STOP
                                        // Zakres 1500–3000 ms = ignorowany (martwa strefa)

// ============== OUTPUT TIMING ==============
#define CYCLE_TIME_S            180     // Czas ACTIVE w sekundach
#define CYCLE_TIME_MS           (CYCLE_TIME_S * 1000UL)

// ============== INPUT/OUTPUT LOGIC LEVELS ==============
#define INPUT_ACTIVE_STATE      LOW     // Aktywny = zwarcie do masy
#define INPUT_INACTIVE_STATE    HIGH
#define OUTPUT_ACTIVE_STATE     HIGH    // ACTIVE
#define OUTPUT_IDLE_STATE       LOW     // IDLE

#endif
