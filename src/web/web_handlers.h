#ifndef WEB_HANDLERS_H
#define WEB_HANDLERS_H

#include <ESPAsyncWebServer.h>

// Page handlers
void handleDashboard(AsyncWebServerRequest *request);
void handleLoginPage(AsyncWebServerRequest *request);

// Authentication handlers
void handleLogin(AsyncWebServerRequest *request);
void handleLogout(AsyncWebServerRequest *request);

// API handlers
void handleStatus(AsyncWebServerRequest *request);
void handleRelayAddTime(AsyncWebServerRequest *request);
void handleRelayForceOn(AsyncWebServerRequest *request);
void handleRelayOff(AsyncWebServerRequest *request);

#endif
