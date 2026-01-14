/**
 * User Module - Account Implementation
 * Purpose: UserAccount structure functions implementation
 */

#include "user/user_account.h"
#include <string.h>
#include <time.h>
#include <stdio.h>

/* ==================== Helper Functions ==================== */

/**
 * Simple hash function (TEMPORARY IMPLEMENTATION)
 *
 * NOTE: This is a SIMPLE hashing implementation for compilation testing only.
 * In production, this MUST be replaced with proper SHA-256 using OpenSSL:
 *   - Uncomment: #include <openssl/sha.h>
 *   - Add to Dockerfile: openssl-devel.i686
 *   - Replace hash_password() with SHA256-based implementation
 *
 * Current implementation: Simple DJB2 hash (NOT cryptographically secure!)
 */
static void hash_password(const char* password_plain, char* output_hash)
{
    unsigned long hash = 5381;
    int c;
    const char* str = password_plain;

    // DJB2 hash algorithm (simple but NOT secure)
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    // Convert to hex string (64 characters to match SHA256 length)
    sprintf(output_hash, "%016lx%016lx%016lx%016lx", hash, hash ^ 0xDEADBEEF, hash ^ 0xCAFEBABE, hash ^ 0xFEEDFACE);
    output_hash[64] = '\0';
}

/* ==================== Account Management Functions ==================== */

/**
 * Initialize UserAccount structure with default values
 */
void UserAccount_Initialize(UserAccount* account)
{
    if (!account) return;

    memset(account, 0, sizeof(UserAccount));

    // Set defaults
    account->status = ACCOUNT_STATUS_ACTIVE;
    account->user_type = USER_TYPE_PLAYER;
    account->register_date = (uint32_t)time(NULL);
    account->created_at = (uint32_t)time(NULL);
    account->updated_at = (uint32_t)time(NULL);
    account->is_first_login = 1;
    account->vip_info.vip_level = VIP_LEVEL_NONE;
    account->warehouse_slots_unlocked = 0;
}

/**
 * Validate account credentials (password check)
 */
int UserAccount_ValidatePassword(const UserAccount* account, const char* password_plain)
{
    if (!account || !password_plain) {
        return 0;
    }

    char password_hash[MAX_PASSWORD_HASH_LENGTH];
    hash_password(password_plain, password_hash);

    return (strcmp(account->password_hash, password_hash) == 0) ? 1 : 0;
}

/**
 * Check if account is currently blocked
 */
int UserAccount_IsBlocked(const UserAccount* account)
{
    if (!account) {
        return 0;
    }

    if (!account->is_blocked) {
        return 0;
    }

    // Check if block has expired
    uint32_t current_time = (uint32_t)time(NULL);

    // block_expire_time == 0 means permanent ban
    if (account->block_expire_time == 0) {
        return 1;
    }

    // If current time is past expiration, not blocked anymore
    if (current_time > account->block_expire_time) {
        return 0;
    }

    return 1;
}

/**
 * Check if account VIP is valid (not expired)
 */
int UserAccount_IsVIPValid(const UserAccount* account)
{
    if (!account) {
        return 0;
    }

    if (account->vip_info.vip_level == VIP_LEVEL_NONE) {
        return 0;
    }

    uint32_t current_time = (uint32_t)time(NULL);

    // vip_expire_time == 0 means permanent VIP
    if (account->vip_info.vip_expire_time == 0) {
        return 1;
    }

    // Check if VIP has expired
    if (current_time > account->vip_info.vip_expire_time) {
        return 0;
    }

    return 1;
}

/**
 * Check if account has sufficient permissions
 */
int UserAccount_HasPermission(const UserAccount* account, UserType required_type)
{
    if (!account) {
        return 0;
    }

    // Higher user type value = more permissions
    return (account->user_type >= required_type) ? 1 : 0;
}

/**
 * Update last login information
 */
void UserAccount_UpdateLoginInfo(UserAccount* account, const char* ip_address)
{
    if (!account) return;

    uint32_t current_time = (uint32_t)time(NULL);

    account->last_login_date = current_time;
    account->current_login_time = current_time;
    account->is_currently_online = 1;
    account->statistics.total_login_count++;

    if (ip_address) {
        strncpy(account->last_login_ip, ip_address, MAX_IP_ADDRESS_LENGTH - 1);
        account->last_login_ip[MAX_IP_ADDRESS_LENGTH - 1] = '\0';
    }

    // Reset first login flag
    if (account->is_first_login) {
        account->is_first_login = 0;
    }

    // Reset failed login attempts on successful login
    account->security_info.login_attempt_count = 0;

    account->updated_at = current_time;
}

/**
 * Block account with reason
 */
void UserAccount_Block(UserAccount* account, uint32_t duration_seconds,
                       const char* reason, uint32_t gm_id)
{
    if (!account) return;

    uint32_t current_time = (uint32_t)time(NULL);

    account->is_blocked = 1;
    account->block_start_time = current_time;

    if (duration_seconds == 0) {
        // Permanent ban
        account->block_expire_time = 0;
        account->status = ACCOUNT_STATUS_BANNED;
    } else {
        // Temporary block
        account->block_expire_time = current_time + duration_seconds;
        account->status = ACCOUNT_STATUS_BLOCKED;
    }

    if (reason) {
        strncpy(account->block_reason, reason, MAX_BLOCK_REASON_LENGTH - 1);
        account->block_reason[MAX_BLOCK_REASON_LENGTH - 1] = '\0';
    }

    account->block_by_gm_id = gm_id;
    account->updated_at = current_time;
}

/**
 * Unblock account
 */
void UserAccount_Unblock(UserAccount* account)
{
    if (!account) return;

    account->is_blocked = 0;
    account->block_start_time = 0;
    account->block_expire_time = 0;
    account->status = ACCOUNT_STATUS_ACTIVE;
    memset(account->block_reason, 0, sizeof(account->block_reason));
    account->block_by_gm_id = 0;

    account->updated_at = (uint32_t)time(NULL);
}

/**
 * Set VIP status
 */
void UserAccount_SetVIP(UserAccount* account, VIPLevel vip_level, uint32_t duration_seconds)
{
    if (!account) return;

    uint32_t current_time = (uint32_t)time(NULL);

    account->vip_info.vip_level = vip_level;

    if (duration_seconds == 0) {
        // Permanent VIP
        account->vip_info.vip_expire_time = 0;
    } else {
        // Extend VIP time (if already VIP, add to existing time)
        if (account->vip_info.vip_expire_time > current_time) {
            account->vip_info.vip_expire_time += duration_seconds;
        } else {
            account->vip_info.vip_expire_time = current_time + duration_seconds;
        }
    }

    // Update user type to premium if VIP
    if (vip_level > VIP_LEVEL_NONE && account->user_type == USER_TYPE_PLAYER) {
        account->user_type = USER_TYPE_PREMIUM;
    }

    account->updated_at = current_time;
}

/**
 * Add account statistics
 */
void UserAccount_AddStatistic(UserAccount* account, int field, uint32_t value)
{
    if (!account) return;

    switch (field) {
        case 0: // Total play time
            account->statistics.total_play_time += value;
            break;
        case 1: // Dungeon cleared
            account->statistics.dungeon_cleared += value;
            break;
        case 2: // PVP matches
            account->statistics.pvp_matches += value;
            break;
        case 3: // PVP wins
            account->statistics.pvp_wins += value;
            break;
        case 4: // PVP losses
            account->statistics.pvp_losses += value;
            break;
        case 5: // Quests completed
            account->statistics.quests_completed += value;
            break;
        case 6: // Achievements unlocked
            account->statistics.achievements_unlocked += value;
            break;
        case 7: // Gold earned
            account->statistics.total_gold_earned += value;
            break;
        case 8: // Gold spent
            account->statistics.total_gold_spent += value;
            break;
        default:
            break;
    }

    account->updated_at = (uint32_t)time(NULL);
}

/**
 * Record failed login attempt
 */
void UserAccount_RecordFailedLogin(UserAccount* account, const char* ip_address)
{
    if (!account) return;

    uint32_t current_time = (uint32_t)time(NULL);

    account->security_info.login_attempt_count++;
    account->security_info.last_failed_login = current_time;

    if (ip_address) {
        strncpy(account->security_info.last_failed_ip, ip_address,
                MAX_IP_ADDRESS_LENGTH - 1);
        account->security_info.last_failed_ip[MAX_IP_ADDRESS_LENGTH - 1] = '\0';
    }

    // Auto-block after 5 failed attempts (15 minutes)
    if (account->security_info.login_attempt_count >= 5) {
        UserAccount_Block(account, 900, "Too many failed login attempts", 0);
    }

    account->updated_at = current_time;
}

/**
 * Change password
 */
int UserAccount_ChangePassword(UserAccount* account, const char* old_password,
                               const char* new_password)
{
    if (!account || !old_password || !new_password) {
        return 0;
    }

    // Verify old password
    if (!UserAccount_ValidatePassword(account, old_password)) {
        return 0;
    }

    // Validate new password length
    size_t new_pass_len = strlen(new_password);
    if (new_pass_len < 6 || new_pass_len > 32) {
        return 0;
    }

    // Hash and set new password
    hash_password(new_password, account->password_hash);

    account->security_info.last_password_change = (uint32_t)time(NULL);
    account->updated_at = (uint32_t)time(NULL);

    return 1;
}

/**
 * Check if account can create more characters
 */
int UserAccount_CanCreateCharacter(const UserAccount* account)
{
    if (!account) {
        return 0;
    }

    return (account->statistics.character_count < MAX_CHARACTERS_PER_ACCOUNT) ? 1 : 0;
}

/**
 * Increment character count
 */
void UserAccount_IncrementCharacterCount(UserAccount* account)
{
    if (!account) return;

    if (account->statistics.character_count < MAX_CHARACTERS_PER_ACCOUNT) {
        account->statistics.character_count++;
        account->updated_at = (uint32_t)time(NULL);
    }
}

/**
 * Decrement character count (on deletion)
 */
void UserAccount_DecrementCharacterCount(UserAccount* account)
{
    if (!account) return;

    if (account->statistics.character_count > 0) {
        account->statistics.character_count--;
        account->statistics.deleted_char_count++;
        account->updated_at = (uint32_t)time(NULL);
    }
}

/**
 * Set account as GM
 */
void UserAccount_SetGM(UserAccount* account, uint8_t gm_level)
{
    if (!account) return;

    account->is_gm = 1;
    account->gm_level = gm_level;
    account->user_type = USER_TYPE_GM;

    account->updated_at = (uint32_t)time(NULL);
}

/**
 * Remove GM status
 */
void UserAccount_RemoveGM(UserAccount* account)
{
    if (!account) return;

    account->is_gm = 0;
    account->gm_level = 0;

    // Revert to appropriate user type
    if (UserAccount_IsVIPValid(account)) {
        account->user_type = USER_TYPE_PREMIUM;
    } else {
        account->user_type = USER_TYPE_PLAYER;
    }

    account->updated_at = (uint32_t)time(NULL);
}

/**
 * Get account status string
 */
const char* UserAccount_GetStatusString(const UserAccount* account)
{
    if (!account) {
        return "Unknown";
    }

    switch (account->status) {
        case ACCOUNT_STATUS_ACTIVE:
            return "Active";
        case ACCOUNT_STATUS_BLOCKED:
            return "Blocked";
        case ACCOUNT_STATUS_BANNED:
            return "Banned";
        case ACCOUNT_STATUS_DELETED:
            return "Deleted";
        case ACCOUNT_STATUS_PENDING:
            return "Pending";
        default:
            return "Unknown";
    }
}

/**
 * Get user type string
 */
const char* UserAccount_GetUserTypeString(const UserAccount* account)
{
    if (!account) {
        return "Unknown";
    }

    switch (account->user_type) {
        case USER_TYPE_GUEST:
            return "Guest";
        case USER_TYPE_PLAYER:
            return "Player";
        case USER_TYPE_PREMIUM:
            return "Premium";
        case USER_TYPE_GM:
            return "GM";
        case USER_TYPE_ADMIN:
            return "Admin";
        default:
            return "Unknown";
    }
}

/**
 * Print account summary (for debugging)
 */
void UserAccount_PrintSummary(const UserAccount* account)
{
    if (!account) {
        printf("Account: NULL\n");
        return;
    }

    printf("========== Account Summary ==========\n");
    printf("Account ID: %u\n", account->account_id);
    printf("Username: %s\n", account->account_name);
    printf("Status: %s\n", UserAccount_GetStatusString(account));
    printf("User Type: %s\n", UserAccount_GetUserTypeString(account));
    printf("VIP Level: %d\n", account->vip_info.vip_level);
    printf("Is GM: %s (Level %d)\n", account->is_gm ? "Yes" : "No", account->gm_level);
    printf("Total Logins: %u\n", account->statistics.total_login_count);
    printf("Total Play Time: %u seconds (%u hours)\n",
           account->statistics.total_play_time,
           account->statistics.total_play_time / 3600);
    printf("Characters: %u\n", account->statistics.character_count);
    printf("Is Blocked: %s\n", UserAccount_IsBlocked(account) ? "Yes" : "No");

    if (account->is_blocked) {
        printf("Block Reason: %s\n", account->block_reason);
    }

    printf("=====================================\n");
}
