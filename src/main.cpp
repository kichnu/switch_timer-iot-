
#include <Arduino.h>
#include "core/logging.h"
#include "config/config.h"
#include "config/credentials_manager.h"
#include "network/wifi_manager.h"
#include "security/auth_manager.h"
#include "security/session_manager.h"
#include "security/rate_limiter.h"
#include "web/web_server.h"
#include "algorithm/switch_timer.h"
#include "provisioning/prov_detector.h"
#include "provisioning/ap_core.h"
#include "provisioning/ap_server.h"
#include "provisioning/prov_config.h"

extern SwitchTimer switchTimer;

void setup() {
    initLogging();
    delay(5000);

    LOG_INFO("");
    LOG_INFO("====================================");
    LOG_INFO("=== Switch Timer Starting ===");
    LOG_INFO("ESP32 Flash size: %d", ESP.getFlashChipSize());
    LOG_INFO("Free heap: %d", ESP.getFreeHeap());
    LOG_INFO("CHECKING PROVISIONING BUTTON");

    // Check if provisioning button is held
    if (checkProvisioningButton()) {
        LOG_INFO("");
        LOG_INFO("====================================");
        LOG_INFO("    ENTERING PROVISIONING MODE");
        LOG_INFO("====================================");

        // Initialize NVS for credential storage
        initCredentialsManager();

        if (!startAccessPoint()) {
            LOG_ERROR("FATAL: Failed to start AP - halting");
            while(1) delay(1000);
        }

        if (!startDNSServer()) {
            LOG_ERROR("FATAL: Failed to start DNS - halting");
            while(1) delay(1000);
        }

        LOG_INFO("");
        LOG_INFO("====================================");
        LOG_INFO("=== PROVISIONING MODE ACTIVE ===");
        LOG_INFO("SSID: %s", PROV_AP_SSID);
        LOG_INFO("Password: %s", PROV_AP_PASSWORD);
        LOG_INFO("URL: http://%s", getAPIPAddress().toString().c_str());

        if (!startWebServer()) {
            LOG_ERROR("FATAL: Failed to start web server - halting");
            while(1) delay(1000);
        }

        LOG_INFO("=== CAPTIVE PORTAL READY ===");

        // Enter blocking loop - never returns
        runProvisioningLoop();
    }

    // === PRODUCTION MODE ===
    LOG_INFO("");
    LOG_INFO("=== Production Mode - Switch Timer ===");

    bool credentials_loaded = initCredentialsManager();
    if (credentials_loaded) {
        LOG_INFO("Device ID: %s", getDeviceID());
    } else {
        LOG_WARNING("No credentials configured. Use provisioning mode.");
    }

    initWiFi();

    // Initialize security
    initAuthManager();
    initSessionManager();
    initRateLimiter();

    // Initialize web server
    initWebServer();

    // Initialize switch timer (GPIO setup + IDLE state)
    switchTimer.init();

    LOG_INFO("");
    LOG_INFO("====================================");
    LOG_INFO("SYSTEM READY");
    if (isWiFiConnected()) {
        LOG_INFO("Dashboard: http://%s", getLocalIP().toString().c_str());
    }
    LOG_INFO("Free heap: %d", ESP.getFreeHeap());
    LOG_INFO("====================================");
}

void loop() {
    static unsigned long lastUpdate = 0;
    unsigned long now = millis();

    // 24h auto-restart
    if (now > 86400000UL) {
        LOG_INFO("=== DAILY RESTART: 24h uptime reached ===");
        switchTimer.webForceOff();
        delay(1000);
        ESP.restart();
    }

    // Process physical button input + timer check every iteration
    switchTimer.update();

    // Update subsystems every 100ms
    if (now - lastUpdate >= 100) {
        updateSessionManager();
        updateRateLimiter();
        updateWiFi();
        lastUpdate = now;
    }

    delay(10);
}
