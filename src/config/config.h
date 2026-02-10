#ifndef CONFIG_H
#define CONFIG_H

#include <IPAddress.h>

// ================= DEBUG & LOGGING CONFIG =================
#define ENABLE_FULL_LOGGING true
#define ENABLE_SERIAL_DEBUG true

// ================= IP WHITELIST =================
extern const IPAddress ALLOWED_IPS[];
extern const int ALLOWED_IPS_COUNT;

// ================= SECURITY CONSTANTS =================
const unsigned long SESSION_TIMEOUT_MS = 1800000;   // 30 minutes
const unsigned long RATE_LIMIT_WINDOW_MS = 1000;    // 1 second window
const int MAX_REQUESTS_PER_SECOND = 5;
const int MAX_FAILED_ATTEMPTS = 10;
const unsigned long BLOCK_DURATION_MS = 60000;      // 1 minute block

// Include credentials manager
#include "credentials_manager.h"

#endif
