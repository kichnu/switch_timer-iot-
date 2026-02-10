#ifndef CREDENTIALS_MANAGER_H
#define CREDENTIALS_MANAGER_H

#include <Arduino.h>

struct DynamicCredentials {
    String wifi_ssid;
    String wifi_password;
    String admin_password_hash;  // SHA-256 hex string (64 chars)
    String device_id;
    bool loaded;
};

extern DynamicCredentials dynamicCredentials;

// Lifecycle
bool initCredentialsManager();
bool areCredentialsLoaded();

// NVS operations
bool loadCredentialsFromNVS();
bool saveCredentialsToNVS(const String& deviceName, const String& wifiSSID,
                          const String& wifiPassword, const String& adminPasswordHash);

// Accessor functions
const char* getWiFiSSID();
const char* getWiFiPassword();
const char* getAdminPasswordHash();
const char* getDeviceID();

#endif
