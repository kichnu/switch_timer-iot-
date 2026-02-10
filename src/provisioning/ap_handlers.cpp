#include "ap_handlers.h"
#include "credentials_validator.h"
#include "../core/logging.h"
#include "../config/credentials_manager.h"
#include "../security/auth_manager.h"

void handleConfigureSubmit(AsyncWebServerRequest *request, JsonVariant &json) {
    LOG_INFO("Configuration submission from: %s", request->client()->remoteIP().toString().c_str());

    JsonObject jsonObj = json.as<JsonObject>();

    String deviceName = jsonObj["device_name"] | "";
    String wifiSSID = jsonObj["wifi_ssid"] | "";
    String wifiPassword = jsonObj["wifi_password"] | "";
    String adminPassword = jsonObj["admin_password"] | "";

    LOG_INFO("Received credentials:");
    LOG_INFO("  Device Name: %s", deviceName.c_str());
    LOG_INFO("  WiFi SSID: %s", wifiSSID.c_str());

    // Validate all credentials
    ValidationResult validation = prov_validateAllCredentials(
        deviceName, wifiSSID, wifiPassword, adminPassword
    );

    if (!validation.valid) {
        LOG_ERROR("Validation failed: %s - %s",
                  validation.errorField.c_str(),
                  validation.errorMessage.c_str());

        DynamicJsonDocument responseDoc(512);
        responseDoc["success"] = false;
        responseDoc["error"] = validation.errorMessage;
        responseDoc["field"] = validation.errorField;

        String response;
        serializeJson(responseDoc, response);
        request->send(400, "application/json", response);
        return;
    }

    LOG_INFO("Validation passed");

    // Hash admin password before storage
    String adminHash = hashPassword(adminPassword);

    // Save to NVS
    if (!saveCredentialsToNVS(deviceName, wifiSSID, wifiPassword, adminHash)) {
        LOG_ERROR("Failed to save credentials to NVS");

        DynamicJsonDocument responseDoc(512);
        responseDoc["success"] = false;
        responseDoc["error"] = "Failed to save credentials";

        String response;
        serializeJson(responseDoc, response);
        request->send(500, "application/json", response);
        return;
    }

    LOG_INFO("=== CONFIGURATION SAVED SUCCESSFULLY ===");
    LOG_INFO("Device configured as: %s", deviceName.c_str());
    LOG_INFO("Manual restart required to boot in production mode");

    DynamicJsonDocument responseDoc(512);
    responseDoc["success"] = true;
    responseDoc["message"] = "Configuration saved successfully! Please restart the device manually.";
    responseDoc["device_name"] = deviceName;

    String response;
    serializeJson(responseDoc, response);
    request->send(200, "application/json", response);
}
