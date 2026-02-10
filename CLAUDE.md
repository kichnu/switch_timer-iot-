# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP32-C3 (Seeed Xiao) based IoT switch timer. Written in C/C++ using the Arduino framework. The system provides timed relay control via physical button and web dashboard, with credential provisioning via captive portal.

## Build & Upload

- **Framework:** Arduino (ESP32)
- **Target board:** Seeed Xiao ESP32-C3
- **Required libraries:** ESPAsyncWebServer, ArduinoJson, MbedTLS (bundled with ESP32)
- **Build:** `pio run`
- **Upload:** `pio run -t upload`
- **Monitor:** `pio device monitor`

## Architecture

### Two-Mode Boot System

The device operates in one of two mutually exclusive modes determined at boot:

1. **Provisioning Mode** — Activated by holding GPIO8 for 5 seconds at boot. Starts a WiFi Access Point (`SWITCH-TIMER-SETUP` / `setup12345`) with a captive portal at 192.168.4.1. The portal collects device name, WiFi SSID/password, and admin password. Credentials stored in NVS (admin password as SHA-256 hash). After saving, the device must be manually restarted.

2. **Production Mode** — Normal operation. Loads credentials from NVS, connects to WiFi, starts the async web server on port 80, and runs the switch timer algorithm. Forces a restart every 24 hours.

### State Machine

```
OutputState: IDLE | ACTIVE | PERMANENT

Physical button:
  IDLE + START     → ACTIVE (timer = 180s)
  ACTIVE + START   → ACTIVE (timer += 180s)
  PERMANENT + START → ignored
  Any + STOP       → IDLE

Web "Add Time":  same as physical START
Web "Force ON":  Any → PERMANENT
Web "Force OFF": Any → IDLE
```

Input classification: press < 1500ms = START, press > 3000ms = STOP, 1500-3000ms = ignored (dead zone).

### Source Layout

- `src/main.cpp` — Entry point, boot mode selection, production loop
- `src/conf.h` — Pin configuration, timing constants, logic levels
- `src/algorithm/switch_timer.h/.cpp` — State machine (IDLE/ACTIVE/PERMANENT), physical input + web control
- `src/config/config.h/.cpp` — Security constants, IP whitelist, global SwitchTimer instance
- `src/config/credentials_manager.h/.cpp` — NVS credential storage (Preferences library)
- `src/core/logging.h/.cpp` — Logging macros with conditional compilation via `ENABLE_FULL_LOGGING`
- `src/network/wifi_manager.h/.cpp` — WiFi connection/reconnection
- `src/provisioning/` — Captive portal: AP setup, DNS server, web server, HTML/JS UI, input validation, WiFi scanning
- `src/security/auth_manager.h/.cpp` — IP whitelist, SHA-256 password verification
- `src/security/session_manager.h/.cpp` — Cookie-based sessions (30min timeout, 10 session limit)
- `src/security/rate_limiter.h/.cpp` — Rate limiting (5 req/s, 60s block after 10 failures)
- `src/web/web_server.h/.cpp` — Route setup, authentication middleware
- `src/web/web_handlers.h/.cpp` — API handler implementations
- `src/web/html_pages.h/.cpp` — Login page and dashboard HTML/CSS/JS

### Hardware Connections

| Pin | Function |
|-----|----------|
| GPIO 3 | Physical switch button (INPUT, active LOW, pull-up) |
| GPIO 4 | Relay output (OUTPUT, active HIGH) |
| GPIO 8 | Provisioning button (5s hold at boot) |
| GPIO 2 | Error/feedback signal |

### Security Model

Layered defense: IP whitelist (403) → rate limiting (429) → session cookie (401) → handler. Sessions are cookie-based (HttpOnly, SameSite=Strict). Passwords are SHA-256 hashed via mbedtls.

### Web API Endpoints

| Method | Path | Auth | Description |
|--------|------|------|-------------|
| GET | `/` | Session | Dashboard |
| GET | `/login` | Whitelist | Login page |
| POST | `/api/login` | Whitelist | Create session |
| POST | `/api/logout` | Session | Destroy session |
| GET | `/api/status` | Session | Relay state + system info |
| POST | `/api/relay/add-time` | Session | Add cycle time |
| POST | `/api/relay/force-on` | Session | Permanent ON |
| POST | `/api/relay/off` | Session | Immediate OFF |

## Code Conventions

- Logging uses macros: `LOG_INFO(fmt, ...)`, `LOG_WARNING(fmt, ...)`, `LOG_ERROR(fmt, ...)` — controlled by `ENABLE_FULL_LOGGING` in `config.h`
- Constants defined in header files with `#define`; shared state uses `extern` declarations in headers with definitions in `.cpp` files
- Comments and some log messages mix Polish and English
