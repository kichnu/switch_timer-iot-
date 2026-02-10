
#include "html_pages.h"

const char* LOGIN_HTML = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Switch Timer - Login</title>
    <style>
        :root {
            --bg-primary: #0a0f1a;
            --bg-card: #111827;
            --bg-input: #1e293b;
            --border: #2d3a4f;
            --text-primary: #f1f5f9;
            --text-secondary: #94a3b8;
            --text-muted: #64748b;
            --accent-blue: #38bdf8;
            --accent-cyan: #22d3d5;
            --accent-green: #22c55e;
            --accent-red: #ef4444;
            --radius: 12px;
            --radius-sm: 8px;
        }
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: var(--bg-primary);
            color: var(--text-primary);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
        }
        body::before {
            content: '';
            position: fixed;
            top: 0; left: 0; right: 0; bottom: 0;
            background:
                radial-gradient(ellipse at 20% 0%, rgba(56,189,248,0.08) 0%, transparent 50%),
                radial-gradient(ellipse at 80% 100%, rgba(34,211,213,0.06) 0%, transparent 50%);
            pointer-events: none;
        }
        .login-box {
            background: var(--bg-card);
            border: 1px solid var(--border);
            padding: 40px;
            border-radius: var(--radius);
            box-shadow: 0 4px 24px rgba(0,0,0,0.4);
            width: 100%; max-width: 400px; margin: 20px;
            position: relative; z-index: 1;
        }
        .logo {
            display: flex; align-items: center; justify-content: center;
            gap: 12px; margin-bottom: 30px;
        }
        .logo-icon {
            width: 40px; height: 40px;
            background: linear-gradient(135deg, var(--accent-cyan), var(--accent-blue));
            border-radius: var(--radius-sm);
            display: flex; align-items: center; justify-content: center;
        }
        .logo-icon svg { width: 24px; height: 24px; fill: var(--bg-primary); }
        h1 { font-size: 1.25rem; font-weight: 700; }
        h1 span { color: var(--text-muted); font-weight: 500; }
        .form-group { margin-bottom: 20px; }
        label {
            display: block; margin-bottom: 8px; font-weight: 500;
            color: var(--text-secondary); font-size: 0.875rem;
        }
        input[type="password"] {
            width: 100%; padding: 12px 16px;
            background: var(--bg-input); border: 1px solid var(--border);
            border-radius: var(--radius-sm); font-size: 1rem; color: var(--text-primary);
        }
        input[type="password"]:focus {
            outline: none; border-color: var(--accent-blue);
            box-shadow: 0 0 0 3px rgba(56,189,248,0.2);
        }
        .login-btn {
            width: 100%; padding: 14px;
            background: linear-gradient(135deg, var(--accent-cyan), var(--accent-blue));
            color: var(--bg-primary); border: none; border-radius: var(--radius-sm);
            font-size: 1rem; font-weight: 600; cursor: pointer; transition: all 0.2s;
        }
        .login-btn:hover { transform: translateY(-2px); box-shadow: 0 4px 12px rgba(56,189,248,0.3); }
        .alert {
            padding: 12px 16px; margin: 15px 0; border-radius: var(--radius-sm);
            display: none; font-size: 0.875rem;
        }
        .alert.error {
            background: rgba(239,68,68,0.15); border: 1px solid rgba(239,68,68,0.3);
            color: var(--accent-red);
        }
        .info {
            margin-top: 24px; padding: 16px; background: var(--bg-input);
            border-radius: var(--radius-sm); font-size: 0.75rem; color: var(--text-muted);
        }
        .info strong { color: var(--text-secondary); }
    </style>
</head>
<body>
    <div class="login-box">
        <div class="logo">
            <div class="logo-icon">
                <svg viewBox="0 0 24 24"><path d="M13 3h-2v10h2V3zm4.83 2.17l-1.42 1.42C17.99 7.86 19 9.81 19 12c0 3.87-3.13 7-7 7s-7-3.13-7-7c0-2.19 1.01-4.14 2.58-5.42L6.17 5.17C4.23 6.82 3 9.26 3 12c0 4.97 4.03 9 9 9s9-4.03 9-9c0-2.74-1.23-5.18-3.17-6.83z"/></svg>
            </div>
            <h1>Switch Timer <span>- Login</span></h1>
        </div>
        <form id="loginForm">
            <div class="form-group">
                <label for="password">Administrator Password</label>
                <input type="password" id="password" name="password" required />
            </div>
            <button type="submit" class="login-btn">Login</button>
        </form>
        <div id="error" class="alert error"></div>
        <div class="info">
            <strong>Security Features:</strong><br />
            Session-based authentication | Rate limiting | IP filtering
        </div>
    </div>
    <script>
        document.getElementById("loginForm").addEventListener("submit", function(e) {
            e.preventDefault();
            var pw = document.getElementById("password").value;
            var err = document.getElementById("error");
            fetch("api/login", {
                method: "POST",
                headers: {"Content-Type": "application/x-www-form-urlencoded"},
                body: "password=" + encodeURIComponent(pw)
            })
            .then(function(r) { return r.json(); })
            .then(function(d) {
                if (d.success) { window.location.href = "/"; }
                else { err.textContent = d.error || "Login failed"; err.style.display = "block"; }
            })
            .catch(function() {
                err.textContent = "Connection error"; err.style.display = "block";
            });
        });
    </script>
</body>
</html>
)rawliteral";

const char* DASHBOARD_HTML = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Switch Timer</title>
    <style>
        :root {
            --bg-primary: #0a0f1a;
            --bg-card: #111827;
            --bg-card-hover: #1a2332;
            --bg-input: #1e293b;
            --border: #2d3a4f;
            --text-primary: #f1f5f9;
            --text-secondary: #94a3b8;
            --text-muted: #64748b;
            --accent-blue: #38bdf8;
            --accent-cyan: #22d3d5;
            --accent-green: #22c55e;
            --accent-red: #ef4444;
            --accent-orange: #f97316;
            --accent-yellow: #eab308;
            --shadow: 0 4px 24px rgba(0,0,0,0.4);
            --radius: 12px;
            --radius-sm: 8px;
        }
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: var(--bg-primary);
            color: var(--text-primary);
            min-height: 100vh;
            line-height: 1.5;
        }
        body::before {
            content: '';
            position: fixed;
            top: 0; left: 0; right: 0; bottom: 0;
            background:
                radial-gradient(ellipse at 20% 0%, rgba(56,189,248,0.08) 0%, transparent 50%),
                radial-gradient(ellipse at 80% 100%, rgba(34,211,213,0.06) 0%, transparent 50%);
            pointer-events: none; z-index: 0;
        }
        .container {
            max-width: 520px; margin: 0 auto; padding: 16px;
            position: relative; z-index: 1;
        }

        /* Header */
        header {
            display: flex; align-items: center; justify-content: space-between;
            padding: 12px 0 24px; border-bottom: 1px solid var(--border); margin-bottom: 24px;
        }
        .logo { display: flex; align-items: center; gap: 12px; }
        .logo-icon {
            width: 40px; height: 40px;
            background: linear-gradient(135deg, var(--accent-cyan), var(--accent-blue));
            border-radius: var(--radius-sm);
            display: flex; align-items: center; justify-content: center;
        }
        .logo-icon svg { width: 24px; height: 24px; fill: var(--bg-primary); }
        h1 { font-size: 1.25rem; font-weight: 700; }
        .btn-back {
            background: var(--bg-input); border: 1px solid var(--border);
            color: var(--text-secondary); padding: 8px 16px;
            border-radius: var(--radius-sm); font-size: 0.875rem;
            font-weight: 500; cursor: pointer; transition: all 0.2s;
        }
        .btn-back:hover { background: var(--bg-card-hover); color: var(--text-primary); }

        /* Cards */
        .card {
            background: var(--bg-card); border: 1px solid var(--border);
            border-radius: var(--radius); padding: 20px;
            margin-bottom: 16px; box-shadow: var(--shadow);
        }
        .card-header {
            display: flex; align-items: center; gap: 10px;
            margin-bottom: 16px; padding-bottom: 12px;
            border-bottom: 1px solid var(--border);
        }
        .card-header h2 {
            font-size: 0.9rem; font-weight: 600;
            text-transform: uppercase; letter-spacing: 0.05em;
            color: var(--text-secondary);
        }
        .card-header-icon {
            width: 28px; height: 28px; border-radius: 6px;
            display: flex; align-items: center; justify-content: center;
        }
        .card-header-icon svg { width: 16px; height: 16px; }

        /* Relay Status Display */
        .relay-display {
            text-align: center; padding: 24px 0 20px;
        }
        .relay-indicator {
            display: inline-flex; flex-direction: column;
            align-items: center; justify-content: center;
            width: 100%; height: 100px;
            border-radius: var(--radius-sm); border: 3px solid var(--border);
            background: var(--bg-input); transition: all 0.3s;
            margin-bottom: 16px;
        }
        .relay-indicator.on {
            border-color: rgba(34,197,94,0.6);
            background: rgba(34,197,94,0.1);
            box-shadow: 0 0 30px rgba(34,197,94,0.15);
        }
        .relay-indicator.on .relay-state { color: var(--accent-green); }
        .relay-indicator.permanent {
            border-color: rgba(249,115,22,0.6);
            background: rgba(249,115,22,0.1);
            box-shadow: 0 0 30px rgba(249,115,22,0.15);
        }
        .relay-indicator.permanent .relay-state { color: var(--accent-orange); }
        .relay-state {
            font-size: 1.5rem; font-weight: 700;
            color: var(--text-muted); margin-bottom: 4px;
        }
        .relay-time {
            font-family: 'Courier New', monospace;
            font-size: 1.8rem; font-weight: 700;
            color: var(--text-primary); line-height: 1;
        }
        .relay-sub {
            font-size: 0.75rem; color: var(--text-muted);
            margin-top: 4px; text-transform: uppercase;
            letter-spacing: 0.1em;
        }

        /* Buttons */
        .button-row {
            display: grid; grid-template-columns: repeat(3, 1fr);
            gap: 10px; margin-top: 8px;
        }
        .btn {
            display: flex; align-items: center; justify-content: center;
            height: 96px; gap: 6px; padding: 12px 16px;
            border-radius: var(--radius-sm); font-family: inherit;
            font-size: 0.85rem; font-weight: 600; border: none;
            cursor: pointer; transition: all 0.2s;
        }
        .btn:disabled { opacity: 0.5; cursor: not-allowed; transform: none !important; }
        .btn svg { width: 16px; height: 16px; }
        .btn-add {
            background: rgba(56,189,248,0.15); border: 1px solid rgba(56,189,248,0.3);
            color: var(--accent-blue);
        }
        .btn-add:hover:not(:disabled) {
            transform: translateY(-2px); box-shadow: 0 2px 8px rgba(56,189,248,0.3);
        }
        .btn-on {
            background: rgba(34,197,94,0.15); border: 1px solid rgba(34,197,94,0.3);
            color: var(--accent-green);
        }
        .btn-on:hover:not(:disabled) {
            transform: translateY(-2px); box-shadow: 0 2px 8px rgba(34,197,94,0.3);
        }
        .btn-off {
            background: rgba(239,68,68,0.15); border: 1px solid rgba(239,68,68,0.3);
            color: var(--accent-red);
        }
        .btn-off:hover:not(:disabled) {
            transform: translateY(-2px); box-shadow: 0 2px 8px rgba(239,68,68,0.3);
        }

        /* System Info */
        .info-grid {
            display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px;
        }
        @media (max-width: 480px) {
            .info-grid { grid-template-columns: 1fr; }
        }
        .info-item {
            display: flex; flex-direction: column; align-items: center;
            background: var(--bg-input); border-radius: var(--radius-sm); padding: 12px;
        }
        .info-item .label {
            font-size: 0.65rem; font-weight: 600; text-transform: uppercase;
            letter-spacing: 0.08em; color: var(--text-muted); margin-bottom: 4px;
        }
        .info-item .value {
            font-family: 'Courier New', monospace;
            font-size: 0.85rem; font-weight: 600; color: var(--text-primary);
        }
        .info-item.connected .value { color: var(--accent-green); }

        /* Footer */
        .footer-info {
            text-align: center; padding: 16px;
            color: var(--text-muted); font-size: 0.75rem;
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <div class="logo">
                <div class="logo-icon">
                    <svg viewBox="0 0 24 24"><path d="M13 3h-2v10h2V3zm4.83 2.17l-1.42 1.42C17.99 7.86 19 9.81 19 12c0 3.87-3.13 7-7 7s-7-3.13-7-7c0-2.19 1.01-4.14 2.58-5.42L6.17 5.17C4.23 6.82 3 9.26 3 12c0 4.97 4.03 9 9 9s9-4.03 9-9c0-2.74-1.23-5.18-3.17-6.83z"/></svg>
                </div>
                <h1>Switch Timer</h1>
            </div>
            <button class="btn-back" onclick="logout()">Logout</button>
        </header>

        <!-- RELAY STATUS CARD -->
        <div class="card">
            <div class="card-header">
                <div class="card-header-icon" style="background: rgba(34,197,94,0.15);">
                    <svg fill="currentColor" style="color: var(--accent-green);" viewBox="0 0 24 24"><path d="M13 3h-2v10h2V3zm4.83 2.17l-1.42 1.42C17.99 7.86 19 9.81 19 12c0 3.87-3.13 7-7 7s-7-3.13-7-7c0-2.19 1.01-4.14 2.58-5.42L6.17 5.17C4.23 6.82 3 9.26 3 12c0 4.97 4.03 9 9 9s9-4.03 9-9c0-2.74-1.23-5.18-3.17-6.83z"/></svg>
                </div>
                <h2>Relay Status</h2>
            </div>

            <div class="relay-display">
                <div class="relay-indicator" id="relayIndicator">
                    <div class="relay-state" id="relayState">OFF</div>
                    <div class="relay-time" id="relayTime"></div>
                    <div class="relay-sub" id="relaySub"></div>
                </div>
            </div>

            <div class="button-row">
                <button class="btn btn-add" id="btnAddTime" onclick="relayAction('add-time')">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="16"/><line x1="8" y1="12" x2="16" y2="12"/></svg>
                    Add Time
                </button>
                <button class="btn btn-on" id="btnForceOn" onclick="relayAction('force-on')">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M5 12h14"/><path d="M12 5l7 7-7 7"/></svg>
                    Force ON
                </button>
                <button class="btn btn-off" id="btnOff" onclick="relayAction('off')">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="6" y="6" width="12" height="12" rx="2"/></svg>
                    OFF
                </button>
            </div>
        </div>

        <!-- SYSTEM INFO CARD -->
        <div class="card">
            <div class="card-header">
                <div class="card-header-icon" style="background: rgba(56,189,248,0.15);">
                    <svg fill="currentColor" style="color: var(--accent-blue);" viewBox="0 0 24 24"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-2h2v2zm0-4h-2V7h2v6z"/></svg>
                </div>
                <h2>System Info</h2>
            </div>
            <div class="info-grid">
                <div class="info-item connected" id="wifiItem">
                    <span class="label">WiFi</span>
                    <span class="value" id="wifiStatus">...</span>
                </div>
                <div class="info-item">
                    <span class="label">Free Heap</span>
                    <span class="value" id="freeHeap">...</span>
                </div>
                <div class="info-item">
                    <span class="label">Uptime</span>
                    <span class="value" id="uptime">...</span>
                </div>
            </div>
        </div>

        <div class="footer-info">Switch Timer v1.0</div>
    </div>

    <script>
        // ============================================
        // STATE
        // ============================================
        var sessionExpired = false;
        var pollingIntervals = [];
        var countdownInterval = null;
        var lastKnownState = "idle";
        var lastKnownRemaining = 0;
        var lastStatusTime = 0;

        // ============================================
        // SESSION EXPIRY
        // ============================================
        function handleSessionExpired() {
            if (sessionExpired) return;
            sessionExpired = true;
            pollingIntervals.forEach(function(id) { clearInterval(id); });
            pollingIntervals = [];
            if (countdownInterval) { clearInterval(countdownInterval); countdownInterval = null; }

            var ov = document.createElement("div");
            ov.innerHTML = '<div style="position:fixed;top:0;left:0;right:0;bottom:0;background:rgba(0,0,0,0.85);z-index:9999;display:flex;justify-content:center;align-items:center;"><div style="background:var(--bg-card);border:1px solid var(--border);padding:40px;border-radius:12px;text-align:center;max-width:400px;margin:20px;"><h2 style="color:var(--accent-red);margin-bottom:15px;">Session Expired</h2><p style="color:var(--text-secondary);margin-bottom:25px;">Please log in again.</p><a href="/login" style="display:inline-block;padding:12px 30px;background:linear-gradient(135deg,var(--accent-cyan),var(--accent-blue));color:var(--bg-primary);text-decoration:none;border-radius:8px;font-weight:bold;">Login</a></div></div>';
            document.body.appendChild(ov);
        }

        function secureFetch(url, options) {
            if (sessionExpired) return Promise.resolve(null);
            return fetch(url, options || {}).then(function(r) {
                if (r.status === 401 || (r.redirected && r.url.indexOf("/login") !== -1)) {
                    handleSessionExpired();
                    return null;
                }
                return r;
            }).catch(function() { return null; });
        }

        // ============================================
        // RELAY DISPLAY UPDATE
        // ============================================
        function updateRelayDisplay(state, remainingSec) {
            var indicator = document.getElementById("relayIndicator");
            var stateEl = document.getElementById("relayState");
            var timeEl = document.getElementById("relayTime");
            var subEl = document.getElementById("relaySub");

            indicator.className = "relay-indicator";

            if (state === "idle") {
                stateEl.textContent = "OFF";
                timeEl.textContent = "";
                subEl.textContent = "";
            } else if (state === "active") {
                indicator.classList.add("on");
                stateEl.textContent = "ON";
                var m = Math.floor(remainingSec / 60);
                var s = remainingSec % 60;
                timeEl.textContent = m + ":" + (s < 10 ? "0" : "") + s;
                subEl.textContent = "remaining";
            } else if (state === "permanent") {
                indicator.classList.add("permanent");
                stateEl.textContent = "ON";
                timeEl.textContent = "";
                subEl.textContent = "permanent";
            }
        }

        // ============================================
        // CLIENT-SIDE COUNTDOWN
        // ============================================
        function startCountdown() {
            if (countdownInterval) clearInterval(countdownInterval);
            countdownInterval = setInterval(function() {
                if (lastKnownState === "active" && lastKnownRemaining > 0) {
                    lastKnownRemaining--;
                    updateRelayDisplay("active", lastKnownRemaining);
                    if (lastKnownRemaining <= 0) {
                        lastKnownState = "idle";
                        updateRelayDisplay("idle", 0);
                    }
                }
            }, 1000);
        }

        // ============================================
        // STATUS POLLING
        // ============================================
        function updateStatus() {
            secureFetch("api/status").then(function(r) {
                if (!r) return null;
                return r.json();
            }).then(function(d) {
                if (!d) return;

                lastKnownState = d.relay_state;
                lastKnownRemaining = d.remaining_seconds;
                lastStatusTime = Date.now();
                updateRelayDisplay(d.relay_state, d.remaining_seconds);

                // WiFi
                var wifiItem = document.getElementById("wifiItem");
                var wifiVal = document.getElementById("wifiStatus");
                wifiVal.textContent = d.wifi_status || "Unknown";
                wifiItem.className = d.wifi_connected ? "info-item connected" : "info-item";

                // Heap
                document.getElementById("freeHeap").textContent =
                    (d.free_heap / 1024).toFixed(1) + " KB";

                // Uptime
                var sec = Math.floor(d.uptime / 1000);
                var h = Math.floor(sec / 3600);
                var min = Math.floor((sec % 3600) / 60);
                document.getElementById("uptime").textContent = h + "h " + min + "m";
            });
        }

        // ============================================
        // RELAY ACTIONS
        // ============================================
        function relayAction(action) {
            var btnMap = {"add-time": "btnAddTime", "force-on": "btnForceOn", "off": "btnOff"};
            var btn = document.getElementById(btnMap[action]);
            btn.disabled = true;

            secureFetch("api/relay/" + action, {method: "POST"}).then(function(r) {
                if (!r) return null;
                return r.json();
            }).then(function(d) {
                if (!d) return;
                if (d.success) {
                    lastKnownState = d.relay_state;
                    lastKnownRemaining = d.remaining_seconds;
                    updateRelayDisplay(d.relay_state, d.remaining_seconds);
                }
            }).finally(function() {
                btn.disabled = false;
            });
        }

        // ============================================
        // LOGOUT
        // ============================================
        function logout() {
            fetch("api/logout", {method: "POST"}).then(function() {
                window.location.href = "/login";
            });
        }

        // ============================================
        // INIT
        // ============================================
        pollingIntervals.push(setInterval(updateStatus, 2000));
        updateStatus();
        startCountdown();
    </script>
</body>
</html>
)rawliteral";
