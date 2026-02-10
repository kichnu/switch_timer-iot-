#ifndef CREDENTIALS_VALIDATOR_H
#define CREDENTIALS_VALIDATOR_H

#include <Arduino.h>

struct ValidationResult {
    bool valid;
    String errorField;
    String errorMessage;
};

ValidationResult prov_validateDeviceName(const String& name);
ValidationResult prov_validateWiFiSSID(const String& ssid);
ValidationResult prov_validateWiFiPassword(const String& password);
ValidationResult prov_validateAdminPassword(const String& password);

ValidationResult prov_validateAllCredentials(
    const String& deviceName,
    const String& wifiSSID,
    const String& wifiPassword,
    const String& adminPassword
);

#endif
