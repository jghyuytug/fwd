/**
 * Security Module - Full Implementation
 *
 * Provides:
 * - Hack detection and analysis (CHackAnalyzer compatible)
 * - IP blacklist management (CAutoPunishRuleBlackIPMgr compatible)
 * - GM account verification (CGMAccounts compatible)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../../include/common/error_codes.h"

/* Error codes for security module */
#define ERR_SECURITY_BASE           4000
#define ERR_SECURITY_NOT_INIT       (ERR_SECURITY_BASE + 1)
#define ERR_SECURITY_BLACKLISTED    (ERR_SECURITY_BASE + 2)
#define ERR_SECURITY_HACK_DETECTED  (ERR_SECURITY_BASE + 3)
#define ERR_SECURITY_GM_INVALID     (ERR_SECURITY_BASE + 4)
#define ERR_SECURITY_MAX_BLACKLIST  (ERR_SECURITY_BASE + 5)
#define ERR_SECURITY_MAX_GM         (ERR_SECURITY_BASE + 6)

/* Configuration constants */
#define MAX_BLACKLIST_ENTRIES   10000
#define MAX_GM_ACCOUNTS         1000
#define MAX_HACK_ANALYZERS      50000
#define IP_ADDRESS_LENGTH       64
#define ACCOUNT_ID_LENGTH       64
#define PASSWORD_LENGTH         128

/* Blacklist reason codes */
typedef enum {
    BLACKLIST_REASON_HACK_DETECT = 1,
    BLACKLIST_REASON_SPEED_HACK = 2,
    BLACKLIST_REASON_MEMORY_HACK = 3,
    BLACKLIST_REASON_EXPLOIT = 4,
    BLACKLIST_REASON_BOT = 5,
    BLACKLIST_REASON_MANUAL_BAN = 6,
    BLACKLIST_REASON_SUSPICIOUS = 7
} BlacklistReason;

/* GM permission levels */
typedef enum {
    GM_LEVEL_NONE = 0,
    GM_LEVEL_HELPER = 1,
    GM_LEVEL_MODERATOR = 2,
    GM_LEVEL_ADMIN = 3,
    GM_LEVEL_SUPER = 4
} GMPermissionLevel;

/* Hack type flags */
typedef enum {
    HACK_TYPE_NONE = 0,
    HACK_TYPE_SPEED = (1 << 0),
    HACK_TYPE_MEMORY = (1 << 1),
    HACK_TYPE_MONSTER_DIE = (1 << 2),
    HACK_TYPE_DIFFICULTY = (1 << 3),
    HACK_TYPE_EQUIPMENT = (1 << 4),
    HACK_TYPE_PACKET = (1 << 5),
    HACK_TYPE_POSITION = (1 << 6)
} HackTypeFlags;

/* Black IP entry */
typedef struct {
    char ip_address[IP_ADDRESS_LENGTH];
    BlacklistReason reason;
    unsigned int ban_time;
    unsigned int expire_time;  /* 0 = permanent */
    int is_active;
} BlackIPEntry;

/* GM Account entry */
typedef struct {
    char account_id[ACCOUNT_ID_LENGTH];
    char password_hash[PASSWORD_LENGTH];
    GMPermissionLevel level;
    unsigned int created_time;
    unsigned int last_login;
    int is_active;
} GMAccountEntry;

/* Hack analyzer entry per user */
typedef struct {
    unsigned int user_id;
    unsigned int hack_flags;
    unsigned int hack_count[8];  /* Count per hack type */
    unsigned int last_check_time;
    unsigned int warning_count;
    int is_active;
} HackAnalyzerEntry;

/* Global security state */
static struct {
    int initialized;

    /* Black IP management */
    BlackIPEntry* blacklist;
    int blacklist_count;
    int blacklist_capacity;

    /* GM Account management */
    GMAccountEntry* gm_accounts;
    int gm_count;
    int gm_capacity;

    /* Hack analyzer pool */
    HackAnalyzerEntry* hack_analyzers;
    int analyzer_count;
    int analyzer_capacity;

    /* Statistics */
    struct {
        unsigned int total_blacklist_checks;
        unsigned int total_blacklist_hits;
        unsigned int total_hack_detections;
        unsigned int total_gm_verifications;
        unsigned int total_gm_failures;
    } stats;
} g_security = {0};

/* Simple hash function for password */
static unsigned int hash_password(const char* password)
{
    unsigned int hash = 5381;
    int c;
    while ((c = *password++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/**
 * Initialize Security Module
 */
int Security_Initialize(void)
{
    if (g_security.initialized) {
        fprintf(stderr, "[Security] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    printf("[Security] Initializing security module...\n");

    /* Allocate blacklist */
    g_security.blacklist_capacity = MAX_BLACKLIST_ENTRIES;
    g_security.blacklist = calloc(g_security.blacklist_capacity, sizeof(BlackIPEntry));
    if (!g_security.blacklist) {
        fprintf(stderr, "[Security] Failed to allocate blacklist\n");
        return ERR_OUT_OF_MEMORY;
    }

    /* Allocate GM accounts */
    g_security.gm_capacity = MAX_GM_ACCOUNTS;
    g_security.gm_accounts = calloc(g_security.gm_capacity, sizeof(GMAccountEntry));
    if (!g_security.gm_accounts) {
        fprintf(stderr, "[Security] Failed to allocate GM accounts\n");
        free(g_security.blacklist);
        return ERR_OUT_OF_MEMORY;
    }

    /* Allocate hack analyzers */
    g_security.analyzer_capacity = MAX_HACK_ANALYZERS;
    g_security.hack_analyzers = calloc(g_security.analyzer_capacity, sizeof(HackAnalyzerEntry));
    if (!g_security.hack_analyzers) {
        fprintf(stderr, "[Security] Failed to allocate hack analyzers\n");
        free(g_security.blacklist);
        free(g_security.gm_accounts);
        return ERR_OUT_OF_MEMORY;
    }

    /* Reset statistics */
    memset(&g_security.stats, 0, sizeof(g_security.stats));

    g_security.initialized = 1;
    printf("[Security] Module initialized. Blacklist capacity: %d, GM capacity: %d\n",
           g_security.blacklist_capacity, g_security.gm_capacity);

    return ERR_SUCCESS;
}

/**
 * Cleanup Security Module
 */
void Security_Cleanup(void)
{
    if (!g_security.initialized) {
        return;
    }

    printf("[Security] Cleaning up security module...\n");
    printf("[Security] Statistics:\n");
    printf("  - Blacklist Checks: %u\n", g_security.stats.total_blacklist_checks);
    printf("  - Blacklist Hits: %u\n", g_security.stats.total_blacklist_hits);
    printf("  - Hack Detections: %u\n", g_security.stats.total_hack_detections);
    printf("  - GM Verifications: %u\n", g_security.stats.total_gm_verifications);
    printf("  - GM Failures: %u\n", g_security.stats.total_gm_failures);

    /* Free resources */
    free(g_security.blacklist);
    free(g_security.gm_accounts);
    free(g_security.hack_analyzers);

    g_security.blacklist = NULL;
    g_security.gm_accounts = NULL;
    g_security.hack_analyzers = NULL;
    g_security.initialized = 0;

    printf("[Security] Module cleanup complete\n");
}

/*============================================================================
 * Black IP Management
 *============================================================================*/

/**
 * Add IP to blacklist
 */
int Security_AddBlackIP(const char* ip_address, int reason, unsigned int duration)
{
    if (!g_security.initialized) {
        return ERR_SECURITY_NOT_INIT;
    }

    if (!ip_address || strlen(ip_address) == 0) {
        return ERR_INVALID_PARAMETER;
    }

    /* Check if already blacklisted */
    for (int i = 0; i < g_security.blacklist_count; i++) {
        if (g_security.blacklist[i].is_active &&
            strcmp(g_security.blacklist[i].ip_address, ip_address) == 0) {
            /* Update existing entry */
            g_security.blacklist[i].reason = (BlacklistReason)reason;
            g_security.blacklist[i].ban_time = (unsigned int)time(NULL);
            g_security.blacklist[i].expire_time = (duration > 0) ?
                g_security.blacklist[i].ban_time + duration : 0;
            printf("[Security] Updated blacklist entry for %s\n", ip_address);
            return ERR_SUCCESS;
        }
    }

    /* Find free slot or expand */
    if (g_security.blacklist_count >= g_security.blacklist_capacity) {
        fprintf(stderr, "[Security] Blacklist full\n");
        return ERR_SECURITY_MAX_BLACKLIST;
    }

    /* Add new entry */
    BlackIPEntry* entry = &g_security.blacklist[g_security.blacklist_count];
    strncpy(entry->ip_address, ip_address, IP_ADDRESS_LENGTH - 1);
    entry->ip_address[IP_ADDRESS_LENGTH - 1] = '\0';
    entry->reason = (BlacklistReason)reason;
    entry->ban_time = (unsigned int)time(NULL);
    entry->expire_time = (duration > 0) ? entry->ban_time + duration : 0;
    entry->is_active = 1;

    g_security.blacklist_count++;

    printf("[Security] Added %s to blacklist (reason=%d, duration=%u)\n",
           ip_address, reason, duration);

    return ERR_SUCCESS;
}

/**
 * Remove IP from blacklist
 */
int Security_RemoveBlackIP(const char* ip_address)
{
    if (!g_security.initialized) {
        return ERR_SECURITY_NOT_INIT;
    }

    if (!ip_address) {
        return ERR_INVALID_PARAMETER;
    }

    for (int i = 0; i < g_security.blacklist_count; i++) {
        if (g_security.blacklist[i].is_active &&
            strcmp(g_security.blacklist[i].ip_address, ip_address) == 0) {
            g_security.blacklist[i].is_active = 0;
            printf("[Security] Removed %s from blacklist\n", ip_address);
            return ERR_SUCCESS;
        }
    }

    return ERR_NOT_FOUND;
}

/**
 * Check if IP is blacklisted
 */
int Security_IsBlacklistedIP(const char* ip_address)
{
    if (!g_security.initialized || !ip_address) {
        return 0;
    }

    g_security.stats.total_blacklist_checks++;
    unsigned int current_time = (unsigned int)time(NULL);

    for (int i = 0; i < g_security.blacklist_count; i++) {
        BlackIPEntry* entry = &g_security.blacklist[i];
        if (entry->is_active && strcmp(entry->ip_address, ip_address) == 0) {
            /* Check if expired */
            if (entry->expire_time > 0 && current_time > entry->expire_time) {
                entry->is_active = 0;  /* Auto-expire */
                return 0;
            }
            g_security.stats.total_blacklist_hits++;
            return 1;
        }
    }

    return 0;
}

/**
 * Get blacklist count
 */
int Security_GetBlacklistCount(void)
{
    if (!g_security.initialized) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < g_security.blacklist_count; i++) {
        if (g_security.blacklist[i].is_active) {
            count++;
        }
    }
    return count;
}

/*============================================================================
 * GM Account Management
 *============================================================================*/

/**
 * Add GM account
 */
int Security_AddGMAccount(const char* account_id, const char* password, int level)
{
    if (!g_security.initialized) {
        return ERR_SECURITY_NOT_INIT;
    }

    if (!account_id || !password || level < GM_LEVEL_HELPER || level > GM_LEVEL_SUPER) {
        return ERR_INVALID_PARAMETER;
    }

    /* Check if already exists */
    for (int i = 0; i < g_security.gm_count; i++) {
        if (g_security.gm_accounts[i].is_active &&
            strcmp(g_security.gm_accounts[i].account_id, account_id) == 0) {
            return ERR_ALREADY_EXISTS;
        }
    }

    if (g_security.gm_count >= g_security.gm_capacity) {
        return ERR_SECURITY_MAX_GM;
    }

    /* Add new GM account */
    GMAccountEntry* entry = &g_security.gm_accounts[g_security.gm_count];
    strncpy(entry->account_id, account_id, ACCOUNT_ID_LENGTH - 1);
    entry->account_id[ACCOUNT_ID_LENGTH - 1] = '\0';

    /* Hash password */
    snprintf(entry->password_hash, PASSWORD_LENGTH, "%u", hash_password(password));

    entry->level = (GMPermissionLevel)level;
    entry->created_time = (unsigned int)time(NULL);
    entry->last_login = 0;
    entry->is_active = 1;

    g_security.gm_count++;

    printf("[Security] Added GM account: %s (level=%d)\n", account_id, level);

    return ERR_SUCCESS;
}

/**
 * Verify GM account
 */
int Security_VerifyGMAccount(const char* account_id, const char* password)
{
    if (!g_security.initialized) {
        return 0;
    }

    if (!account_id || !password) {
        return 0;
    }

    g_security.stats.total_gm_verifications++;

    char password_hash[PASSWORD_LENGTH];
    snprintf(password_hash, PASSWORD_LENGTH, "%u", hash_password(password));

    for (int i = 0; i < g_security.gm_count; i++) {
        GMAccountEntry* entry = &g_security.gm_accounts[i];
        if (entry->is_active &&
            strcmp(entry->account_id, account_id) == 0 &&
            strcmp(entry->password_hash, password_hash) == 0) {
            entry->last_login = (unsigned int)time(NULL);
            return entry->level;
        }
    }

    g_security.stats.total_gm_failures++;
    return 0;
}

/**
 * Check if account is GM
 */
int Security_IsGM(unsigned int user_id)
{
    if (!g_security.initialized) {
        return 0;
    }

    /* In a real implementation, this would check user_id against GM list */
    /* For now, just return 0 (not GM) */
    (void)user_id;
    return 0;
}

/**
 * Remove GM account
 */
int Security_RemoveGMAccount(const char* account_id)
{
    if (!g_security.initialized) {
        return ERR_SECURITY_NOT_INIT;
    }

    if (!account_id) {
        return ERR_INVALID_PARAMETER;
    }

    for (int i = 0; i < g_security.gm_count; i++) {
        if (g_security.gm_accounts[i].is_active &&
            strcmp(g_security.gm_accounts[i].account_id, account_id) == 0) {
            g_security.gm_accounts[i].is_active = 0;
            printf("[Security] Removed GM account: %s\n", account_id);
            return ERR_SUCCESS;
        }
    }

    return ERR_NOT_FOUND;
}

/*============================================================================
 * Hack Detection / Analysis
 *============================================================================*/

/**
 * Get or create hack analyzer for user
 */
static HackAnalyzerEntry* get_or_create_analyzer(unsigned int user_id)
{
    /* Find existing */
    for (int i = 0; i < g_security.analyzer_count; i++) {
        if (g_security.hack_analyzers[i].is_active &&
            g_security.hack_analyzers[i].user_id == user_id) {
            return &g_security.hack_analyzers[i];
        }
    }

    /* Create new */
    if (g_security.analyzer_count >= g_security.analyzer_capacity) {
        return NULL;
    }

    HackAnalyzerEntry* entry = &g_security.hack_analyzers[g_security.analyzer_count];
    memset(entry, 0, sizeof(HackAnalyzerEntry));
    entry->user_id = user_id;
    entry->is_active = 1;
    entry->last_check_time = (unsigned int)time(NULL);

    g_security.analyzer_count++;

    return entry;
}

/**
 * Begin collecting hack info for user
 */
int Security_BeginHackCollection(unsigned int user_id)
{
    if (!g_security.initialized) {
        return ERR_SECURITY_NOT_INIT;
    }

    HackAnalyzerEntry* entry = get_or_create_analyzer(user_id);
    if (!entry) {
        return ERR_OUT_OF_MEMORY;
    }

    /* Reset counters */
    memset(entry->hack_count, 0, sizeof(entry->hack_count));
    entry->hack_flags = HACK_TYPE_NONE;
    entry->warning_count = 0;
    entry->last_check_time = (unsigned int)time(NULL);

    return ERR_SUCCESS;
}

/**
 * Report hack detection
 */
int Security_ReportHack(unsigned int user_id, int hack_type)
{
    if (!g_security.initialized) {
        return ERR_SECURITY_NOT_INIT;
    }

    HackAnalyzerEntry* entry = get_or_create_analyzer(user_id);
    if (!entry) {
        return ERR_OUT_OF_MEMORY;
    }

    /* Record hack type */
    entry->hack_flags |= (unsigned int)hack_type;

    /* Increment counter for this hack type */
    int type_index = 0;
    int temp_type = hack_type;
    while (temp_type > 1 && type_index < 7) {
        temp_type >>= 1;
        type_index++;
    }
    entry->hack_count[type_index]++;
    entry->warning_count++;

    g_security.stats.total_hack_detections++;

    printf("[Security] Hack detected for user %u: type=%d (warnings=%u)\n",
           user_id, hack_type, entry->warning_count);

    return ERR_SUCCESS;
}

/**
 * Check if user has hack flags
 */
int Security_GetHackFlags(unsigned int user_id)
{
    if (!g_security.initialized) {
        return 0;
    }

    for (int i = 0; i < g_security.analyzer_count; i++) {
        if (g_security.hack_analyzers[i].is_active &&
            g_security.hack_analyzers[i].user_id == user_id) {
            return (int)g_security.hack_analyzers[i].hack_flags;
        }
    }

    return 0;
}

/**
 * Get warning count for user
 */
int Security_GetWarningCount(unsigned int user_id)
{
    if (!g_security.initialized) {
        return 0;
    }

    for (int i = 0; i < g_security.analyzer_count; i++) {
        if (g_security.hack_analyzers[i].is_active &&
            g_security.hack_analyzers[i].user_id == user_id) {
            return (int)g_security.hack_analyzers[i].warning_count;
        }
    }

    return 0;
}

/**
 * Reset hack info for user
 */
int Security_ResetHackInfo(unsigned int user_id)
{
    if (!g_security.initialized) {
        return ERR_SECURITY_NOT_INIT;
    }

    for (int i = 0; i < g_security.analyzer_count; i++) {
        if (g_security.hack_analyzers[i].is_active &&
            g_security.hack_analyzers[i].user_id == user_id) {
            g_security.hack_analyzers[i].hack_flags = HACK_TYPE_NONE;
            memset(g_security.hack_analyzers[i].hack_count, 0,
                   sizeof(g_security.hack_analyzers[i].hack_count));
            g_security.hack_analyzers[i].warning_count = 0;
            return ERR_SUCCESS;
        }
    }

    return ERR_NOT_FOUND;
}

/**
 * Remove user from hack analyzer pool
 */
int Security_RemoveUserAnalyzer(unsigned int user_id)
{
    if (!g_security.initialized) {
        return ERR_SECURITY_NOT_INIT;
    }

    for (int i = 0; i < g_security.analyzer_count; i++) {
        if (g_security.hack_analyzers[i].is_active &&
            g_security.hack_analyzers[i].user_id == user_id) {
            g_security.hack_analyzers[i].is_active = 0;
            return ERR_SUCCESS;
        }
    }

    return ERR_NOT_FOUND;
}

/*============================================================================
 * Statistics
 *============================================================================*/

/**
 * Get security statistics
 */
void* Security_GetStatistics(void)
{
    if (!g_security.initialized) {
        return NULL;
    }
    return &g_security.stats;
}

/**
 * Reset security statistics
 */
void Security_ResetStatistics(void)
{
    if (!g_security.initialized) {
        return;
    }
    memset(&g_security.stats, 0, sizeof(g_security.stats));
    printf("[Security] Statistics reset\n");
}
