#include "config.h"
#include "../algorithm/switch_timer.h"

// ================= IP WHITELIST (LAN) =================
// Add your local network IPs here. Only these can access the web interface directly.
const IPAddress ALLOWED_IPS[] = {
    IPAddress(192, 168, 2, 10),
    IPAddress(192, 168, 2, 20)
};
const int ALLOWED_IPS_COUNT = sizeof(ALLOWED_IPS) / sizeof(ALLOWED_IPS[0]);

// ================= TRUSTED PROXY (WireGuard VPS) =================
// Requests from this IP use X-Forwarded-For for real client IP.
// Bypass whitelist (WireGuard is the trust boundary), enforce rate limit + session.
const IPAddress TRUSTED_PROXY_IP(10, 99, 0, 1);

// ================= GLOBAL SWITCH TIMER INSTANCE =================
SwitchTimer switchTimer;
