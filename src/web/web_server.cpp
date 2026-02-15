#include "web_server.h"
#include "web_handlers.h"
#include "../security/session_manager.h"
#include "../security/rate_limiter.h"
#include "../security/auth_manager.h"
#include "../core/logging.h"

AsyncWebServer server(80);

void initWebServer() {
    // Static pages
    server.on("/", HTTP_GET, handleDashboard);
    server.on("/login", HTTP_GET, handleLoginPage);

    // Authentication
    server.on("/api/login", HTTP_POST, handleLogin);
    server.on("/api/logout", HTTP_POST, handleLogout);

    // API endpoints
    server.on("/api/status", HTTP_GET, handleStatus);
    server.on("/api/health", HTTP_GET, handleHealth);
    server.on("/api/relay/add-time", HTTP_POST, handleRelayAddTime);
    server.on("/api/relay/force-on", HTTP_POST, handleRelayForceOn);
    server.on("/api/relay/off", HTTP_POST, handleRelayOff);

    // 404 handler with whitelist/proxy enforcement
    server.onNotFound([](AsyncWebServerRequest* request) {
        IPAddress sourceIP = request->client()->remoteIP();
        if (!isIPAllowed(sourceIP) && !isTrustedProxy(sourceIP)) {
            request->send(403, "text/plain", "Forbidden");
            return;
        }
        request->send(404, "text/plain", "Not Found");
    });

    server.begin();
    LOG_INFO("Web server started on port 80");
}

bool checkAuthentication(AsyncWebServerRequest* request) {
    IPAddress sourceIP = request->client()->remoteIP();

    // VPS proxy auto-auth — VPS already authenticated user via auth_request + session
    if (isTrustedProxy(sourceIP)) {
        return true;
    }

    // Direct LAN access — full auth chain
    if (!isIPAllowed(sourceIP)) {
        LOG_WARNING("IP %s not on whitelist - access denied", sourceIP.toString().c_str());
        return false;
    }

    if (isIPBlocked(sourceIP)) {
        return false;
    }

    if (isRateLimited(sourceIP)) {
        recordFailedAttempt(sourceIP);
        return false;
    }

    recordRequest(sourceIP);

    // Check session cookie
    if (request->hasHeader("Cookie")) {
        String cookie = request->getHeader("Cookie")->value();
        int tokenStart = cookie.indexOf("session_token=");
        if (tokenStart != -1) {
            tokenStart += 14;
            int tokenEnd = cookie.indexOf(";", tokenStart);
            if (tokenEnd == -1) tokenEnd = cookie.length();

            String token = cookie.substring(tokenStart, tokenEnd);
            if (validateSession(token, sourceIP)) {
                return true;
            }
        }
    }

    recordFailedAttempt(sourceIP);
    return false;
}
