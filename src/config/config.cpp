#include "config.h"
#include "../algorithm/switch_timer.h"

// ================= IP WHITELIST =================
// Add your local network IPs here. Only these can access the web interface.
const IPAddress ALLOWED_IPS[] = {
    IPAddress(192, 168, 2, 10),    // Example - replace with your IPs
    IPAddress(192, 168, 2, 20),
    IPAddress(10, 99, 0, 1)
};
const int ALLOWED_IPS_COUNT = sizeof(ALLOWED_IPS) / sizeof(ALLOWED_IPS[0]);

// ================= GLOBAL SWITCH TIMER INSTANCE =================
SwitchTimer switchTimer;
