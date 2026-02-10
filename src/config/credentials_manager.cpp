#include "credentials_manager.h"
#include "../core/logging.h"
#include <Preferences.h>

static const char* NVS_NAMESPACE = "creds";

DynamicCredentials dynamicCredentials;

bool initCredentialsManager() {
    LOG_INFO("Initializing credentials manager (NVS)...");

    dynamicCredentials.wifi_ssid = "";
    dynamicCredentials.wifi_password = "";
    dynamicCredentials.admin_password_hash = "";
    dynamicCredentials.device_id = "";
    dynamicCredentials.loaded = false;

    if (loadCredentialsFromNVS()) {
        LOG_INFO("Credentials loaded from NVS successfully");
        return true;
    }

    LOG_WARNING("====================================");
    LOG_WARNING("No credentials in NVS - authentication DISABLED");
    LOG_WARNING("Use provisioning mode to configure credentials");
    LOG_WARNING("  1. Hold button (GPIO8) for 5s during boot");
    LOG_WARNING("  2. Connect to SWITCH-TIMER-SETUP WiFi");
    LOG_WARNING("  3. Configure credentials in browser");
    LOG_WARNING("====================================");
    return false;
}

bool loadCredentialsFromNVS() {
    Preferences prefs;
    if (!prefs.begin(NVS_NAMESPACE, true)) {
        LOG_ERROR("Failed to open NVS namespace");
        return false;
    }

    bool configured = prefs.getBool("configured", false);
    if (!configured) {
        prefs.end();
        LOG_WARNING("NVS: no credentials configured");
        return false;
    }

    dynamicCredentials.device_id = prefs.getString("device_name", "");
    dynamicCredentials.wifi_ssid = prefs.getString("wifi_ssid", "");
    dynamicCredentials.wifi_password = prefs.getString("wifi_pass", "");
    dynamicCredentials.admin_password_hash = prefs.getString("admin_hash", "");
    prefs.end();

    // Validate loaded credentials
    if (dynamicCredentials.wifi_ssid.length() == 0 ||
        dynamicCredentials.wifi_password.length() == 0 ||
        dynamicCredentials.admin_password_hash.length() == 0) {
        LOG_ERROR("NVS credentials are incomplete");
        return false;
    }

    dynamicCredentials.loaded = true;

    LOG_INFO("====================================");
    LOG_INFO("Credentials loaded from NVS:");
    LOG_INFO("  Device ID: %s", dynamicCredentials.device_id.c_str());
    LOG_INFO("  WiFi SSID: %s", dynamicCredentials.wifi_ssid.c_str());
    LOG_INFO("  WiFi Pass: ******* (%d chars)", dynamicCredentials.wifi_password.length());
    LOG_INFO("  Admin Hash: %.16s...", dynamicCredentials.admin_password_hash.c_str());
    LOG_INFO("====================================");
    return true;
}

bool saveCredentialsToNVS(const String& deviceName, const String& wifiSSID,
                          const String& wifiPassword, const String& adminPasswordHash) {
    Preferences prefs;
    if (!prefs.begin(NVS_NAMESPACE, false)) {
        LOG_ERROR("Failed to open NVS namespace for writing");
        return false;
    }

    prefs.putString("device_name", deviceName);
    prefs.putString("wifi_ssid", wifiSSID);
    prefs.putString("wifi_pass", wifiPassword);
    prefs.putString("admin_hash", adminPasswordHash);
    prefs.putBool("configured", true);
    prefs.end();

    LOG_INFO("Credentials saved to NVS successfully");
    return true;
}

bool areCredentialsLoaded() {
    return dynamicCredentials.loaded;
}

const char* getWiFiSSID() {
    return dynamicCredentials.wifi_ssid.c_str();
}

const char* getWiFiPassword() {
    return dynamicCredentials.wifi_password.c_str();
}

const char* getAdminPasswordHash() {
    return dynamicCredentials.admin_password_hash.c_str();
}

const char* getDeviceID() {
    return dynamicCredentials.device_id.c_str();
}
