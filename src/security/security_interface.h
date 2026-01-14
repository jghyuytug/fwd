/**
 * Security Module - Public Interface
 * Purpose: Anti-cheat and hack detection system
 *
 * Components:
 * - CHackAnalyzer: Detects various types of hacks (speed, difficulty, equipment upgrade, etc.)
 * - CAutoPunishRuleBlackIPMgr: Manages IP-based bans and punishments
 * - CGMAccounts: GM account verification and management
 */

#ifndef SRC_SECURITY_SECURITY_INTERFACE_H_
#define SRC_SECURITY_SECURITY_INTERFACE_H_

#include <common/defs.h>
#include <security/headers/CHackAnalyzer.h>
#include <security/headers/CAutoPunishRuleBlackIPMgr.h>
#include <security/headers/CGMAccounts.h>

/**
 * Security Module Initialization
 * Call once at server startup
 *
 * Returns: 0 on success, -1 on failure
 */
int Security_Initialize();

/**
 * Security Module Cleanup
 * Call once at server shutdown
 */
void Security_Cleanup();

/**
 * Get Hack Analyzer instance for a user
 * Returns: Pointer to CHackAnalyzer for the specified user
 */
void* Security_GetHackAnalyzer(CUser *user);

/**
 * Add IP address to blacklist
 *
 * Parameters:
 *   ip_address: IP to blacklist
 *   reason: Reason for blacklist (enumeration)
 *   duration: Ban duration in seconds (0 = permanent)
 *
 * Returns: 0 on success, -1 on failure
 */
int Security_AddBlackIP(const char *ip_address, int reason, unsigned int duration);

/**
 * Check if IP is blacklisted
 *
 * Returns: 1 if blacklisted, 0 if not
 */
int Security_IsBlacklistedIP(const char *ip_address);

/**
 * Verify GM account credentials
 *
 * Returns: 1 if valid GM, 0 if not
 */
int Security_VerifyGMAccount(const char *account_id, const char *password);

#endif // SRC_SECURITY_SECURITY_INTERFACE_H_
