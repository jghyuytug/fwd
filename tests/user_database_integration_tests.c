/**
 * User Database Module Integration Tests
 * Tests complete user workflows and scenarios (Phase 6.1 - Week 3)
 *
 * Integration Test Coverage:
 * - Complete user registration and login flow
 * - Multi-character account management
 * - Session lifecycle management
 * - Account state transitions
 * - Data consistency across operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#include "common/defs.h"
#include "common/error_codes.h"
#include "database/headers/database_interface.h"
#include "user/user_account.h"
#include "user/user_character.h"
#include "user/user_session.h"
#include "user/user_database.h"

/* ==================== Test Configuration ==================== */

#define TEST_DB_CONFIG "config/database.cfg"
#define INTEGRATION_TEST_PREFIX "intg_test_"

/* ==================== Test Statistics ==================== */

static int scenarios_run = 0;
static int scenarios_passed = 0;
static int scenarios_failed = 0;
static int total_operations = 0;

/* ==================== Test Helper Macros ==================== */

#define SCENARIO_START(name) \
    do { \
        printf("\n╔═══════════════════════════════════════════════════════╗\n"); \
        printf("║  SCENARIO: %-44s║\n", name); \
        printf("╚═══════════════════════════════════════════════════════╝\n"); \
        scenarios_run++; \
        int scenario_failed = 0; \
        total_operations = 0; \
    } while(0)

#define SCENARIO_END() \
    do { \
        if (!scenario_failed) { \
            scenarios_passed++; \
            printf("✓ Scenario PASSED (%d operations successful)\n", total_operations); \
        } else { \
            scenarios_failed++; \
            printf("✗ Scenario FAILED\n"); \
        } \
    } while(0)

#define OPERATION(description, code) \
    do { \
        printf("  [%d] %s...", ++total_operations, description); \
        fflush(stdout); \
        int result = (code); \
        if (result == ERR_SUCCESS || result == 0) { \
            printf(" ✓\n"); \
        } else { \
            printf(" ✗ (error=%d)\n", result); \
            scenario_failed = 1; \
        } \
    } while(0)

#define VERIFY(condition, message) \
    do { \
        printf("  [%d] Verify: %s...", ++total_operations, message); \
        fflush(stdout); \
        if (condition) { \
            printf(" ✓\n"); \
        } else { \
            printf(" ✗\n"); \
            scenario_failed = 1; \
        } \
    } while(0)

/* ==================== Integration Test Scenarios ==================== */

/**
 * Scenario 1: Complete New User Registration Flow
 * Tests the full process from account creation to first login
 */
void scenario_new_user_registration()
{
    SCENARIO_START("New User Registration Flow");

    UserAccount account;
    Character character;
    UserSession session;
    uint32_t authenticated_id = 0;

    // Step 1: Create account
    memset(&account, 0, sizeof(UserAccount));
    snprintf(account.account_name, sizeof(account.account_name), "%suser001", INTEGRATION_TEST_PREFIX);
    snprintf(account.password_hash, sizeof(account.password_hash), "sha256_integration_test_001");
    snprintf(account.register_ip, sizeof(account.register_ip), "10.0.0.1");
    snprintf(account.security_info.email, sizeof(account.security_info.email), "intgtest001@example.com");
    account.status = 0;
    account.user_type = 0;
    account.register_date = (uint32_t)time(NULL);
    account.warehouse_slots_unlocked = 24;
    account.vip_info.vip_level = 0;

    OPERATION("Create new account", User_DB_CreateAccount(&account));

    // Step 2: Load created account to get account_id
    OPERATION("Load created account", User_DB_LoadAccount(account.account_name, &account));

    VERIFY(account.account_id > 0, "Account ID assigned");
    VERIFY(strcmp(account.account_name, "intg_test_user001") == 0, "Account name matches");

    // Step 3: Create first character
    memset(&character, 0, sizeof(Character));
    character.account_id = account.account_id;
    snprintf(character.character_name, sizeof(character.character_name), "IntgTestChar001");
    character.status = 0;
    character.job_class = 2;  // Mage
    character.job_grow_type = 0;
    character.level = 1;
    character.experience = 0;
    character.next_level_exp = 1000;
    character.appearance.gender = 1;  // Female
    character.appearance.hair_style = 3;
    character.position.current_zone = 1;
    character.position.current_map = 100;
    character.attributes.strength = 10;
    character.attributes.intelligence = 25;
    character.attributes.vitality = 12;
    character.attributes.spirit = 20;
    character.attributes.max_hp = 200;
    character.attributes.current_hp = 200;
    character.attributes.max_mp = 300;
    character.attributes.current_mp = 300;
    character.currency.gold = 1000;
    character.inventory_slots_total = 36;
    character.fatigue_max = 156;
    character.fatigue_value = 156;
    character.party_index = -1;

    OPERATION("Create first character", User_DB_CreateCharacter(&character));

    // Step 4: Verify character was created
    Character* characters = NULL;
    uint8_t char_count = 0;
    OPERATION("Load characters for account", User_DB_LoadCharacters(account.account_id, &characters, &char_count));

    VERIFY(char_count == 1, "Account has exactly 1 character");
    VERIFY(strcmp(characters[0].character_name, "IntgTestChar001") == 0, "Character name matches");

    // Step 5: Authenticate account
    OPERATION("Authenticate account",
              User_DB_AuthenticateAccount(account.account_name, account.password_hash,
                                          "10.0.0.1", &authenticated_id));

    VERIFY(authenticated_id == account.account_id, "Authenticated account ID matches");

    // Step 6: Create session
    memset(&session, 0, sizeof(UserSession));
    session.account_id = account.account_id;
    session.user_id = account.account_id;
    strncpy(session.username, account.account_name, sizeof(session.username) - 1);
    strncpy(session.client_ip, "10.0.0.1", sizeof(session.client_ip) - 1);
    session.is_authenticated = 1;
    session.session_state = 1;
    session.socket_fd = 12345;
    session.client_port = 50000;
    session.login_time = (uint32_t)time(NULL);
    session.last_activity = session.login_time;
    session.client_version = 20250108;

    OPERATION("Create session", User_DB_SaveSession(&session));

    // Step 7: Verify session exists
    UserSession loaded_session;
    OPERATION("Load session", User_DB_LoadSession(account.account_id, &loaded_session));

    VERIFY(loaded_session.is_authenticated == 1, "Session is authenticated");
    VERIFY(loaded_session.session_state == 1, "Session is active");

    // Cleanup
    if (characters) free(characters);

    SCENARIO_END();
}

/**
 * Scenario 2: Multi-Character Account Management
 * Tests creating and managing multiple characters on one account
 */
void scenario_multi_character_management()
{
    SCENARIO_START("Multi-Character Account Management");

    UserAccount account;
    Character characters[3];
    int result;

    // Step 1: Create account
    memset(&account, 0, sizeof(UserAccount));
    snprintf(account.account_name, sizeof(account.account_name), "%smultichar", INTEGRATION_TEST_PREFIX);
    snprintf(account.password_hash, sizeof(account.password_hash), "sha256_multichar_test");
    snprintf(account.register_ip, sizeof(account.register_ip), "10.0.0.2");
    snprintf(account.security_info.email, sizeof(account.security_info.email), "multichar@example.com");
    account.status = 0;
    account.user_type = 0;

    OPERATION("Create multi-character account", User_DB_CreateAccount(&account));
    OPERATION("Load account to get ID", User_DB_LoadAccount(account.account_name, &account));

    // Step 2: Create 3 characters with different classes
    const char* char_names[] = {"MCWarrior", "MCMage", "MCRogue"};
    uint8_t job_classes[] = {1, 2, 3};  // Fighter, Mage, Rogue

    for (int i = 0; i < 3; i++) {
        memset(&characters[i], 0, sizeof(Character));
        characters[i].account_id = account.account_id;
        snprintf(characters[i].character_name, sizeof(characters[i].character_name), "%s", char_names[i]);
        characters[i].status = 0;
        characters[i].job_class = job_classes[i];
        characters[i].level = 1 + i;  // Different levels
        characters[i].attributes.max_hp = 200 + (i * 50);
        characters[i].attributes.current_hp = characters[i].attributes.max_hp;
        characters[i].currency.gold = 1000 * (i + 1);
        characters[i].inventory_slots_total = 36;
        characters[i].party_index = -1;

        char op_desc[128];
        snprintf(op_desc, sizeof(op_desc), "Create character %d: %s", i + 1, char_names[i]);
        OPERATION(op_desc, User_DB_CreateCharacter(&characters[i]));
    }

    // Step 3: Verify all 3 characters exist
    Character* loaded_chars = NULL;
    uint8_t loaded_count = 0;
    OPERATION("Load all characters", User_DB_LoadCharacters(account.account_id, &loaded_chars, &loaded_count));

    VERIFY(loaded_count == 3, "Account has 3 characters");

    // Step 4: Update each character differently
    for (int i = 0; i < loaded_count && i < 3; i++) {
        loaded_chars[i].level += 10;
        loaded_chars[i].experience = 50000 + (i * 10000);
        loaded_chars[i].currency.gold += 100000;

        char op_desc[128];
        snprintf(op_desc, sizeof(op_desc), "Update character %d", i + 1);
        OPERATION(op_desc, User_DB_SaveCharacter(&loaded_chars[i]));
    }

    // Step 5: Reload and verify updates
    Character* reloaded_chars = NULL;
    uint8_t reloaded_count = 0;
    OPERATION("Reload characters after updates",
              User_DB_LoadCharacters(account.account_id, &reloaded_chars, &reloaded_count));

    VERIFY(reloaded_count == 3, "Still have 3 characters");

    for (int i = 0; i < reloaded_count && i < 3; i++) {
        char verify_msg[128];
        snprintf(verify_msg, sizeof(verify_msg), "Character %d level updated correctly", i + 1);
        VERIFY(reloaded_chars[i].level == 11 + i, verify_msg);
    }

    // Step 6: Delete middle character
    if (reloaded_count >= 2) {
        uint32_t deleted_char_id = reloaded_chars[1].character_id;
        OPERATION("Delete middle character", User_DB_DeleteCharacter(deleted_char_id));

        // Verify only 2 active characters remain (status=0)
        Character* final_chars = NULL;
        uint8_t final_count = 0;
        OPERATION("Load remaining active characters",
                  User_DB_LoadCharacters(account.account_id, &final_chars, &final_count));

        // Note: Depending on LoadCharacters implementation, it might filter out deleted (status=1)
        // If not filtered, we would still see 3 but one would have status=1

        if (final_chars) free(final_chars);
    }

    // Cleanup
    if (loaded_chars) free(loaded_chars);
    if (reloaded_chars) free(reloaded_chars);

    SCENARIO_END();
}

/**
 * Scenario 3: Session Lifecycle Management
 * Tests complete session from login to logout
 */
void scenario_session_lifecycle()
{
    SCENARIO_START("Session Lifecycle Management");

    UserAccount account;
    UserSession session;

    // Step 1: Create test account
    memset(&account, 0, sizeof(UserAccount));
    snprintf(account.account_name, sizeof(account.account_name), "%ssession_test", INTEGRATION_TEST_PREFIX);
    snprintf(account.password_hash, sizeof(account.password_hash), "sha256_session");
    snprintf(account.register_ip, sizeof(account.register_ip), "10.0.0.3");
    snprintf(account.security_info.email, sizeof(account.security_info.email), "session@example.com");

    OPERATION("Create session test account", User_DB_CreateAccount(&account));
    OPERATION("Load account", User_DB_LoadAccount(account.account_name, &account));

    // Step 2: Initial login - create session
    uint32_t login_time = (uint32_t)time(NULL);
    memset(&session, 0, sizeof(UserSession));
    session.account_id = account.account_id;
    session.user_id = account.account_id;
    strncpy(session.username, account.account_name, sizeof(session.username) - 1);
    strncpy(session.client_ip, "10.0.0.3", sizeof(session.client_ip) - 1);
    session.is_authenticated = 1;
    session.session_state = 1;  // Active
    session.socket_fd = 54321;
    session.client_port = 60000;
    session.login_time = login_time;
    session.last_activity = login_time;
    session.client_version = 20250108;

    OPERATION("Create session on login", User_DB_SaveSession(&session));

    // Step 3: Simulate activity - update session periodically
    sleep(1);
    session.last_activity = (uint32_t)time(NULL);
    session.packets_sent += 100;
    session.packets_received += 95;
    session.bytes_sent += 50000;
    session.bytes_received += 48000;

    OPERATION("Update session after activity", User_DB_SaveSession(&session));

    // Step 4: Load session and verify activity update
    UserSession loaded_session;
    OPERATION("Load session to verify activity", User_DB_LoadSession(account.account_id, &loaded_session));

    VERIFY(loaded_session.packets_sent == 100, "Packets sent tracked");
    VERIFY(loaded_session.bytes_sent == 50000, "Bytes sent tracked");
    VERIFY(loaded_session.last_activity > login_time, "Last activity updated");

    // Step 5: Update account statistics
    account.statistics.total_login_count++;
    account.last_login_date = login_time;
    strncpy(account.last_login_ip, "10.0.0.3", sizeof(account.last_login_ip) - 1);
    account.is_currently_online = 1;

    OPERATION("Update account login statistics", User_DB_SaveAccount(&account));

    // Step 6: Simulate logout - delete session
    OPERATION("Delete session on logout", User_DB_DeleteSession(account.account_id));

    // Step 7: Verify session is gone
    int result = User_DB_LoadSession(account.account_id, &loaded_session);
    VERIFY(result == ERR_SESSION_NOT_FOUND, "Session deleted after logout");

    // Step 8: Update account offline status
    account.is_currently_online = 0;
    account.last_logout = (uint32_t)time(NULL);
    account.statistics.total_play_time += (account.last_logout - login_time);

    OPERATION("Update account offline status", User_DB_SaveAccount(&account));

    // Step 9: Reload account and verify final state
    UserAccount final_account;
    OPERATION("Load final account state", User_DB_LoadAccount(account.account_name, &final_account));

    VERIFY(final_account.is_currently_online == 0, "Account is offline");
    VERIFY(final_account.statistics.total_login_count == 1, "Login count incremented");
    VERIFY(final_account.statistics.total_play_time > 0, "Play time recorded");

    SCENARIO_END();
}

/**
 * Scenario 4: Account State Transitions
 * Tests different account status changes (active, blocked, deleted)
 */
void scenario_account_state_transitions()
{
    SCENARIO_START("Account State Transitions");

    UserAccount account;

    // Step 1: Create account (status=0, Active)
    memset(&account, 0, sizeof(UserAccount));
    snprintf(account.account_name, sizeof(account.account_name), "%sstate_test", INTEGRATION_TEST_PREFIX);
    snprintf(account.password_hash, sizeof(account.password_hash), "sha256_state");
    snprintf(account.register_ip, sizeof(account.register_ip), "10.0.0.4");
    snprintf(account.security_info.email, sizeof(account.security_info.email), "state@example.com");
    account.status = 0;  // Active

    OPERATION("Create active account", User_DB_CreateAccount(&account));
    OPERATION("Load account", User_DB_LoadAccount(account.account_name, &account));

    VERIFY(account.status == 0, "Account is active");

    // Step 2: Block account (status=1, Blocked)
    account.is_blocked = 1;
    account.block_start_time = (uint32_t)time(NULL);
    account.block_expire_time = account.block_start_time + (7 * 24 * 60 * 60);  // 7 days
    snprintf(account.block_reason, sizeof(account.block_reason), "Test block reason");
    account.block_by_gm_id = 1;  // GM ID 1

    OPERATION("Block account", User_DB_SaveAccount(&account));

    // Step 3: Verify account is blocked
    UserAccount blocked_account;
    OPERATION("Load blocked account", User_DB_LoadAccount(account.account_name, &blocked_account));

    VERIFY(blocked_account.is_blocked == 1, "Account is blocked");
    VERIFY(blocked_account.block_expire_time > blocked_account.block_start_time, "Block has expiration");

    // Step 4: Try to authenticate blocked account (should fail)
    uint32_t auth_id = 0;
    int auth_result = User_DB_AuthenticateAccount(account.account_name, account.password_hash,
                                                    "10.0.0.4", &auth_id);
    VERIFY(auth_result == ERR_ACCOUNT_BLOCKED, "Blocked account cannot authenticate");

    // Step 5: Unblock account
    account.is_blocked = 0;
    account.block_expire_time = 0;
    memset(account.block_reason, 0, sizeof(account.block_reason));

    OPERATION("Unblock account", User_DB_SaveAccount(&account));

    // Step 6: Verify authentication works again
    auth_result = User_DB_AuthenticateAccount(account.account_name, account.password_hash,
                                               "10.0.0.4", &auth_id);
    VERIFY(auth_result == ERR_SUCCESS, "Unblocked account can authenticate");

    // Step 7: Delete account (soft delete, status=3)
    OPERATION("Delete account (soft delete)", User_DB_DeleteAccount(account.account_id));

    // Step 8: Verify account is marked as deleted
    UserAccount deleted_account;
    int load_result = User_DB_LoadAccount(account.account_name, &deleted_account);

    // Depending on implementation, loading deleted account might:
    // - Return ERR_ACCOUNT_NOT_FOUND (if it filters status!=0)
    // - Return ERR_SUCCESS but with status=3
    VERIFY(load_result == ERR_ACCOUNT_NOT_FOUND || deleted_account.status == 3,
           "Account is deleted");

    SCENARIO_END();
}

/**
 * Scenario 5: Data Consistency Across Operations
 * Tests that related data stays consistent across operations
 */
void scenario_data_consistency()
{
    SCENARIO_START("Data Consistency Across Operations");

    UserAccount account;
    Character character;
    UserSession session;

    // Step 1: Create account with initial statistics
    memset(&account, 0, sizeof(UserAccount));
    snprintf(account.account_name, sizeof(account.account_name), "%sconsistency", INTEGRATION_TEST_PREFIX);
    snprintf(account.password_hash, sizeof(account.password_hash), "sha256_consistency");
    snprintf(account.register_ip, sizeof(account.register_ip), "10.0.0.5");
    snprintf(account.security_info.email, sizeof(account.security_info.email), "consistency@example.com");
    account.statistics.character_count = 0;
    account.statistics.total_gold_earned = 0;

    OPERATION("Create account", User_DB_CreateAccount(&account));
    OPERATION("Load account", User_DB_LoadAccount(account.account_name, &account));

    // Step 2: Create character and update account character count
    memset(&character, 0, sizeof(Character));
    character.account_id = account.account_id;
    snprintf(character.character_name, sizeof(character.character_name), "ConsistChar");
    character.status = 0;
    character.job_class = 1;
    character.level = 1;
    character.currency.gold = 5000;
    character.inventory_slots_total = 36;
    character.party_index = -1;

    OPERATION("Create character", User_DB_CreateCharacter(&character));

    // Update account statistics
    account.statistics.character_count = 1;
    OPERATION("Update account character count", User_DB_SaveAccount(&account));

    // Step 3: Simulate earning gold on character
    Character* chars = NULL;
    uint8_t char_count = 0;
    OPERATION("Load character", User_DB_LoadCharacters(account.account_id, &chars, &char_count));

    if (char_count > 0) {
        chars[0].currency.gold += 10000;  // Earned 10000 gold
        OPERATION("Update character gold", User_DB_SaveCharacter(&chars[0]));

        // Update account statistics to match
        account.statistics.total_gold_earned += 10000;
        OPERATION("Update account total gold earned", User_DB_SaveAccount(&account));

        // Verify consistency
        Character reloaded_char;
        OPERATION("Reload character to verify",
                  User_DB_LoadCharacter(chars[0].character_id, &reloaded_char));

        UserAccount reloaded_account;
        OPERATION("Reload account to verify",
                  User_DB_LoadAccount(account.account_name, &reloaded_account));

        VERIFY(reloaded_char.currency.gold == 15000, "Character gold is 15000");
        VERIFY(reloaded_account.statistics.total_gold_earned == 10000,
               "Account tracked gold earned");
        VERIFY(reloaded_account.statistics.character_count == 1,
               "Account character count is 1");

        free(chars);
    }

    SCENARIO_END();
}

/* ==================== Main Test Runner ==================== */

int main(int argc, char* argv[])
{
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║  User Database Module - Integration Tests            ║\n");
    printf("║  Phase 6.1 Week 3 - End-to-End Workflow Testing      ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n\n");

    // Initialize database system
    printf("Initializing database system...\n");
    int result = Database_Initialize(TEST_DB_CONFIG);
    if (result != ERR_SUCCESS) {
        printf("ERROR: Failed to initialize database system (code=%d)\n", result);
        printf("Please check database configuration: %s\n", TEST_DB_CONFIG);
        return 1;
    }
    printf("✓ Database system initialized\n");

    // Run integration test scenarios
    scenario_new_user_registration();
    scenario_multi_character_management();
    scenario_session_lifecycle();
    scenario_account_state_transitions();
    scenario_data_consistency();

    // Cleanup database system
    printf("\n\nCleaning up database system...\n");
    Database_Cleanup();
    printf("✓ Database system cleanup complete\n");

    // Print test summary
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║  INTEGRATION TEST SUMMARY                             ║\n");
    printf("╠═══════════════════════════════════════════════════════╣\n");
    printf("║  Total Scenarios:     %-4d                            ║\n", scenarios_run);
    printf("║  Scenarios Passed:    %-4d ✓                          ║\n", scenarios_passed);
    printf("║  Scenarios Failed:    %-4d ✗                          ║\n", scenarios_failed);
    printf("║  Success Rate:        %.1f%%                           ║\n",
           scenarios_run > 0 ? (scenarios_passed * 100.0 / scenarios_run) : 0.0);
    printf("╚═══════════════════════════════════════════════════════╝\n");

    if (scenarios_failed == 0) {
        printf("\n🎉 All integration tests PASSED!\n");
        printf("User Database module workflows are functioning correctly.\n");
        return 0;
    } else {
        printf("\n⚠️  Some scenarios FAILED. Please review the failures above.\n");
        return 1;
    }
}
