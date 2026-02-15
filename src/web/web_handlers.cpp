#include "web_handlers.h"
#include "web_server.h"
#include "html_pages.h"
#include "../config/config.h"
#include "../core/logging.h"
#include "../security/auth_manager.h"
#include "../security/session_manager.h"
#include "../security/rate_limiter.h"
#include "../algorithm/switch_timer.h"
#include "../network/wifi_manager.h"

extern SwitchTimer switchTimer;

// ============================================
// PAGE HANDLERS
// ============================================

void handleDashboard(AsyncWebServerRequest *request) {
    if (!checkAuthentication(request)) {
        request->redirect("login");
        return;
    }
    request->send(200, "text/html", DASHBOARD_HTML);
}

void handleLoginPage(AsyncWebServerRequest *request) {
    IPAddress sourceIP = request->client()->remoteIP();
    if (!isIPAllowed(sourceIP) && !isTrustedProxy(sourceIP)) {
        request->send(403, "text/plain", "Forbidden");
        return;
    }
    request->send(200, "text/html", LOGIN_HTML);
}

// ============================================
// AUTHENTICATION HANDLERS
// ============================================

void handleLogin(AsyncWebServerRequest *request) {
    IPAddress sourceIP = request->client()->remoteIP();
    bool viaProxy = isTrustedProxy(sourceIP);
    IPAddress clientIP = viaProxy ? resolveClientIP(request) : sourceIP;

    if (!viaProxy && !isIPAllowed(clientIP)) {
        request->send(403, "application/json", "{\"success\":false,\"error\":\"Forbidden\"}");
        return;
    }

    if (isIPBlocked(clientIP)) {
        request->send(429, "application/json", "{\"success\":false,\"error\":\"Too many attempts. Try again later.\"}");
        return;
    }

    if (!request->hasParam("password", true)) {
        request->send(400, "application/json", "{\"success\":false,\"error\":\"Password required\"}");
        return;
    }

    String password = request->getParam("password", true)->value();

    if (!areCredentialsLoaded()) {
        request->send(503, "application/json",
            "{\"success\":false,\"error\":\"No credentials configured. Use provisioning mode.\"}");
        return;
    }

    if (verifyPassword(password)) {
        String token = createSession(clientIP);
        if (token.length() == 0) {
            request->send(500, "application/json", "{\"success\":false,\"error\":\"Session creation failed\"}");
            return;
        }

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json",
            "{\"success\":true}");
        response->addHeader("Set-Cookie",
            "session_token=" + token + "; Path=/; HttpOnly; SameSite=Strict; Max-Age=1800");
        request->send(response);

        LOG_INFO("Login successful from %s", clientIP.toString().c_str());
    } else {
        recordFailedAttempt(clientIP);
        request->send(401, "application/json", "{\"success\":false,\"error\":\"Invalid password\"}");
        LOG_WARNING("Login failed from %s", clientIP.toString().c_str());
    }
}

void handleLogout(AsyncWebServerRequest *request) {
    if (request->hasHeader("Cookie")) {
        String cookie = request->getHeader("Cookie")->value();
        int tokenStart = cookie.indexOf("session_token=");
        if (tokenStart != -1) {
            tokenStart += 14;
            int tokenEnd = cookie.indexOf(";", tokenStart);
            if (tokenEnd == -1) tokenEnd = cookie.length();
            String token = cookie.substring(tokenStart, tokenEnd);
            destroySession(token);
        }
    }

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json",
        "{\"success\":true}");
    response->addHeader("Set-Cookie",
        "session_token=; Path=/; HttpOnly; Max-Age=0");
    request->send(response);
}

// ============================================
// API HANDLERS
// ============================================

void handleStatus(AsyncWebServerRequest *request) {
    if (!checkAuthentication(request)) {
        request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
        return;
    }

    String json = "{";
    json += "\"relay_state\":\"" + String(switchTimer.getOutputStateString()) + "\",";
    json += "\"relay_on\":" + String(switchTimer.isRelayOn() ? "true" : "false") + ",";
    json += "\"remaining_seconds\":" + String(switchTimer.getRemainingMs() / 1000) + ",";
    json += "\"cycle_time_seconds\":" + String(CYCLE_TIME_S) + ",";
    json += "\"uptime\":" + String(millis()) + ",";
    json += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"wifi_connected\":" + String(isWiFiConnected() ? "true" : "false") + ",";
    json += "\"wifi_status\":\"" + getWiFiStatus() + "\"";
    json += "}";

    request->send(200, "application/json", json);
}

void handleHealth(AsyncWebServerRequest *request) {
    IPAddress sourceIP = request->client()->remoteIP();
    if (!isIPAllowed(sourceIP) && !isTrustedProxy(sourceIP)) {
        request->send(403, "application/json", "{\"error\":\"Forbidden\"}");
        return;
    }

    String json = "{";
    json += "\"status\":\"ok\",";
    json += "\"device_name\":\"" + String(getDeviceID()) + "\",";
    json += "\"uptime\":" + String(millis());
    json += "}";

    request->send(200, "application/json", json);
}

void handleRelayAddTime(AsyncWebServerRequest *request) {
    if (!checkAuthentication(request)) {
        request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
        return;
    }

    switchTimer.webAddTime();

    String json = "{";
    json += "\"success\":true,";
    json += "\"relay_state\":\"" + String(switchTimer.getOutputStateString()) + "\",";
    json += "\"remaining_seconds\":" + String(switchTimer.getRemainingMs() / 1000);
    json += "}";

    request->send(200, "application/json", json);
    LOG_INFO("Web: relay add-time from %s", resolveClientIP(request).toString().c_str());
}

void handleRelayForceOn(AsyncWebServerRequest *request) {
    if (!checkAuthentication(request)) {
        request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
        return;
    }

    switchTimer.webForceOn();

    String json = "{";
    json += "\"success\":true,";
    json += "\"relay_state\":\"" + String(switchTimer.getOutputStateString()) + "\",";
    json += "\"remaining_seconds\":0";
    json += "}";

    request->send(200, "application/json", json);
    LOG_INFO("Web: relay force-on from %s", resolveClientIP(request).toString().c_str());
}

void handleRelayOff(AsyncWebServerRequest *request) {
    if (!checkAuthentication(request)) {
        request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
        return;
    }

    switchTimer.webForceOff();

    String json = "{";
    json += "\"success\":true,";
    json += "\"relay_state\":\"" + String(switchTimer.getOutputStateString()) + "\",";
    json += "\"remaining_seconds\":0";
    json += "}";

    request->send(200, "application/json", json);
    LOG_INFO("Web: relay off from %s", resolveClientIP(request).toString().c_str());
}
