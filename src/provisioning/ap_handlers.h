#ifndef AP_HANDLERS_H
#define AP_HANDLERS_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Handle configuration submission - POST /api/configure
// Request JSON: { device_name, wifi_ssid, wifi_password, admin_password }
// Response JSON: { success, message, device_name }
void handleConfigureSubmit(AsyncWebServerRequest *request, JsonVariant &json);

#endif
