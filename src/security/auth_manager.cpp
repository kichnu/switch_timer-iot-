#include "auth_manager.h"
#include "../config/config.h"
#include "../core/logging.h"
#include <mbedtls/md.h>
#include "../config/credentials_manager.h"

void initAuthManager() {
    LOG_INFO("Authentication manager initialized");

    if (areCredentialsLoaded()) {
        LOG_INFO("Using NVS admin credentials");
    } else {
        LOG_WARNING("====================================");
        LOG_WARNING("No NVS credentials - web authentication DISABLED");
        LOG_WARNING("Use Captive Portal to configure credentials");
        LOG_WARNING("====================================");
    }
}

bool isIPAllowed(IPAddress ip) {
    for (int i = 0; i < ALLOWED_IPS_COUNT; i++) {
        if (ALLOWED_IPS[i] == ip) {
            return true;
        }
    }
    return false;
}

String hashPassword(const String& password) {
    byte hash[32];

    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char*)password.c_str(), password.length());
    mbedtls_md_finish(&ctx, hash);
    mbedtls_md_free(&ctx);

    String hashString = "";
    for (int i = 0; i < 32; i++) {
        char str[3];
        sprintf(str, "%02x", (int)hash[i]);
        hashString += str;
    }

    return hashString;
}

bool verifyPassword(const String& password) {
    if (!areCredentialsLoaded()) {
        LOG_ERROR("Authentication BLOCKED - no NVS credentials loaded");
        return false;
    }

    String inputHash = hashPassword(password);
    String expectedHash = String(getAdminPasswordHash());

    if (expectedHash.length() == 0) {
        LOG_ERROR("Invalid admin hash from NVS");
        return false;
    }

    bool valid = (inputHash == expectedHash);

    if (valid) {
        LOG_INFO("Password verification successful");
    } else {
        LOG_WARNING("Password verification failed");
    }
    return valid;
}
