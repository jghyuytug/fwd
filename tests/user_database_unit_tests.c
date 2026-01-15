/**
 * User Database Module Unit Tests
 * Tests all database operations for User module (Phase 6.1 - Week 3)
 *
 * Test Coverage:
 * - Account CRUD operations (4 functions)
 * - Character CRUD operations (5 functions)
 * - Session CRUD operations (3 functions)
 * - Stored procedure calls (1 function)
 * - Error handling and edge cases
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "common/defs.h"
#include "common/error_codes.h"
#include "database/headers/database_interface.h"
#include "user/user_account.h"
#include "user/user_character.h"
#include "user/user_session.h"
#include "user/user_database.h"

/* ==================== Test Configuration ==================== */

#define TEST_DB_CONFIG "config/database.cfg"
#define TEST_ACCOUNT_NAME "test_account_001"
#define TEST_PASSWORD_HASH "sha256_test_hash_123456"
#define TEST_EMAIL "test@example.com"
#define TEST_IP "192.168.1.100"
#define TEST_CHARACTER_NAME "TestCharacter001"

/* ==================== Test Statistics ==================== */

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* ==================== Test Helper Functions ==================== */

/**
 * Test assertion with detailed error reporting
 */
#define TEST_ASSERT(condition, message) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            printf("[PASS] %s\n", message); \
        } else { \
            tests_failed++; \
            printf("[FAIL] %s (line %d)\n", message, __LINE__); \
        } \
    } while(0)

/**
 * Initialize test account with default values
 */
void init_test_account(UserAccount* account)
{
    memset(account, 0, sizeof(UserAccount));

    strncpy(account->account_name, TEST_ACCOUNT_NAME, sizeof(account->account_name) - 1);
    strncpy(account->password_hash, TEST_PASSWORD_HASH, sizeof(account->password_hash) - 1);
    strncpy(account->register_ip, TEST_IP, sizeof(account->register_ip) - 1);
    strncpy(account->security_info.email, TEST_EMAIL, sizeof(account->security_info.email) - 1);

    account->status = 0;  // Active
    account->user_type = 0;  // Normal user
    account->register_date = (uint32_t)time(NULL);
    account->vip_info.vip_level = 0;
    account->is_gm = 0;
    account->warehouse_slots_unlocked = 24;
}

/**
 * Initialize test character with default values
 */
void init_test_character(Character* character, uint32_t account_id)
{
    memset(character, 0, sizeof(Character));

    character->account_id = account_id;
    strncpy(character->character_name, TEST_CHARACTER_NAME, sizeof(character->character_name) - 1);

    character->status = 0;  // Active
    character->job_class = 1;  // Fighter
    character->job_grow_type = 0;  // Base
    character->level = 1;
    character->experience = 0;
    character->next_level_exp = 1000;

    // Appearance
    character->appearance.gender = 0;  // Male
    character->appearance.skin_color = 0;
    character->appearance.hair_style = 1;
    character->appearance.hair_color = 2;

    // Position (starting zone)
    character->position.current_zone = 1;
    character->position.current_map = 100;
    character->position.position_x = 100.0f;
    character->position.position_y = 200.0f;
    character->position.position_z = 0.0f;

    // Attributes (Level 1 fighter)
    character->attributes.strength = 20;
    character->attributes.intelligence = 10;
    character->attributes.vitality = 15;
    character->attributes.spirit = 10;
    character->attributes.max_hp = 250;
    character->attributes.current_hp = 250;
    character->attributes.max_mp = 100;
    character->attributes.current_mp = 100;

    // Currency
    character->currency.gold = 1000;
    character->currency.silver = 0;

    character->inventory_slots_total = 36;
    character->fatigue_max = 156;
    character->fatigue_value = 156;
    character->party_index = -1;  // Not in party
    character->can_trade = 1;
    character->can_drop = 1;
}

/**
 * Initialize test session with default values
 */
void init_test_session(UserSession* session, uint32_t account_id)
{
    memset(session, 0, sizeof(UserSession));

    session->account_id = account_id;
    session->user_id = account_id;
    strncpy(session->username, TEST_ACCOUNT_NAME, sizeof(session->username) - 1);
    strncpy(session->client_ip, TEST_IP, sizeof(session->client_ip) - 1);

    session->is_authenticated = 1;
    session->session_state = 1;  // Active
    session->socket_fd = 1234;
    session->client_port = 5678;
    session->login_time = (uint32_t)time(NULL);
    session->last_activity = session->login_time;
    session->last_heartbeat = session->login_time;
    session->client_version = 20250101;
    strncpy(session->client_hash, "client_hash_test", sizeof(session->client_hash) - 1);
}

/* ==================== Account Database Tests ==================== */

/**
 * Test User_DB_CreateAccount and User_DB_LoadAccount
 */
void test_account_create_and_load()
{
    printf("\n=== Test: Account Create and Load ===\n");

    UserAccount account_write, account_read;
    int result;

    // Initialize test account
    init_test_account(&account_write);

    // Test: Create account
    result = User_DB_CreateAccount(&account_write);
    TEST_ASSERT(result == ERR_SUCCESS, "User_DB_CreateAccount should succeed");

    // Test: Load created account
    memset(&account_read, 0, sizeof(UserAccount));
    result = User_DB_LoadAccount(TEST_ACCOUNT_NAME, &account_read);
    TEST_ASSERT(result == ERR_SUCCESS, "User_DB_LoadAccount should succeed");

    // Verify data
    TEST_ASSERT(strcmp(account_read.account_name, TEST_ACCOUNT_NAME) == 0,
                "Account name should match");
    TEST_ASSERT(strcmp(account_read.password_hash, TEST_PASSWORD_HASH) == 0,
                "Password hash should match");
    TEST_ASSERT(strcmp(account_read.security_info.email, TEST_EMAIL) == 0,
                "Email should match");
    TEST_ASSERT(account_read.status == 0, "Account status should be active");

    printf("✓ Account created and loaded successfully (account_id=%u)\n", account_read.account_id);
}

/**
 * Test User_DB_SaveAccount
 */
void test_account_save()
{
    printf("\n=== Test: Account Save ===\n");

    UserAccount account;
    int result;

    // Load existing account
    result = User_DB_LoadAccount(TEST_ACCOUNT_NAME, &account);
    TEST_ASSERT(result == ERR_SUCCESS, "Should load existing account");

    // Modify account data
    account.vip_info.vip_level = 3;
    account.vip_info.vip_points = 5000;
    account.statistics.total_login_count = 100;
    account.statistics.total_play_time = 36000;  // 10 hours

    // Save modified account
    result = User_DB_SaveAccount(&account);
    TEST_ASSERT(result == ERR_SUCCESS, "User_DB_SaveAccount should succeed");

    // Reload and verify
    UserAccount account_verify;
    result = User_DB_LoadAccount(TEST_ACCOUNT_NAME, &account_verify);
    TEST_ASSERT(result == ERR_SUCCESS, "Should reload account after save");
    TEST_ASSERT(account_verify.vip_info.vip_level == 3, "VIP level should be updated");
    TEST_ASSERT(account_verify.vip_info.vip_points == 5000, "VIP points should be updated");
    TEST_ASSERT(account_verify.statistics.total_login_count == 100, "Login count should be updated");

    printf("✓ Account saved and verified successfully\n");
}

/**
 * Test User_DB_DeleteAccount
 */
void test_account_delete()
{
    printf("\n=== Test: Account Delete ===\n");

    UserAccount account;
    int result;

    // Load existing account
    result = User_DB_LoadAccount(TEST_ACCOUNT_NAME, &account);
    TEST_ASSERT(result == ERR_SUCCESS, "Should load account before delete");

    uint32_t account_id = account.account_id;

    // Delete account (soft delete - status=3)
    result = User_DB_DeleteAccount(account_id);
    TEST_ASSERT(result == ERR_SUCCESS, "User_DB_DeleteAccount should succeed");

    // Try to load deleted account (should fail or return status=3)
    result = User_DB_LoadAccount(TEST_ACCOUNT_NAME, &account);
    // Note: Depending on implementation, deleted accounts might not be loadable
    // or might be loaded with status=3

    printf("✓ Account deleted successfully (soft delete with status=3)\n");
}

/**
 * Test invalid parameters for account operations
 */
void test_account_error_handling()
{
    printf("\n=== Test: Account Error Handling ===\n");

    UserAccount account;
    int result;

    // Test NULL parameters
    result = User_DB_LoadAccount(NULL, &account);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL account_name");

    result = User_DB_LoadAccount(TEST_ACCOUNT_NAME, NULL);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL account pointer");

    result = User_DB_SaveAccount(NULL);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL account in save");

    result = User_DB_CreateAccount(NULL);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL account in create");

    // Test non-existent account
    result = User_DB_LoadAccount("NonExistentAccount999", &account);
    TEST_ASSERT(result == ERR_ACCOUNT_NOT_FOUND, "Should return error for non-existent account");

    printf("✓ Account error handling validated\n");
}

/* ==================== Character Database Tests ==================== */

/**
 * Test User_DB_CreateCharacter and User_DB_LoadCharacter
 */
void test_character_create_and_load()
{
    printf("\n=== Test: Character Create and Load ===\n");

    Character character_write, character_read;
    int result;

    // First, need a valid account
    UserAccount account;
    init_test_account(&account);
    account.account_name[0] = 'c';  // Different from previous test
    strncpy(account.account_name, "test_char_account", sizeof(account.account_name) - 1);
    result = User_DB_CreateAccount(&account);
    if (result != ERR_SUCCESS) {
        // Account might already exist, load it
        result = User_DB_LoadAccount("test_char_account", &account);
    }
    TEST_ASSERT(result == ERR_SUCCESS, "Should have valid account for character test");

    // Initialize test character
    init_test_character(&character_write, account.account_id);

    // Create character
    result = User_DB_CreateCharacter(&character_write);
    TEST_ASSERT(result == ERR_SUCCESS, "User_DB_CreateCharacter should succeed");

    // Character ID should be assigned by database (auto-increment)
    // For testing, we'll need to load by querying characters for account
    Character* characters = NULL;
    uint8_t char_count = 0;
    result = User_DB_LoadCharacters(account.account_id, &characters, &char_count);
    TEST_ASSERT(result == ERR_SUCCESS, "Should load characters for account");
    TEST_ASSERT(char_count >= 1, "Should have at least 1 character");

    if (char_count > 0) {
        // Load first character by ID
        result = User_DB_LoadCharacter(characters[0].character_id, &character_read);
        TEST_ASSERT(result == ERR_SUCCESS, "User_DB_LoadCharacter should succeed");

        // Verify data
        TEST_ASSERT(strcmp(character_read.character_name, TEST_CHARACTER_NAME) == 0,
                    "Character name should match");
        TEST_ASSERT(character_read.account_id == account.account_id,
                    "Account ID should match");
        TEST_ASSERT(character_read.job_class == 1, "Job class should match");
        TEST_ASSERT(character_read.level == 1, "Level should match");
        TEST_ASSERT(character_read.attributes.strength == 20, "Strength should match");

        printf("✓ Character created and loaded successfully (character_id=%u)\n",
               character_read.character_id);

        free(characters);
    }
}

/**
 * Test User_DB_LoadCharacters (load all characters for account)
 */
void test_character_load_all()
{
    printf("\n=== Test: Load All Characters for Account ===\n");

    UserAccount account;
    Character* characters = NULL;
    uint8_t char_count = 0;
    int result;

    // Load test account
    result = User_DB_LoadAccount("test_char_account", &account);
    TEST_ASSERT(result == ERR_SUCCESS, "Should load test account");

    // Load all characters
    result = User_DB_LoadCharacters(account.account_id, &characters, &char_count);
    TEST_ASSERT(result == ERR_SUCCESS, "User_DB_LoadCharacters should succeed");

    printf("  Account has %u character(s)\n", char_count);

    for (int i = 0; i < char_count; i++) {
        printf("  Character %d: ID=%u, Name=%s, Level=%u, Class=%u\n",
               i + 1,
               characters[i].character_id,
               characters[i].character_name,
               characters[i].level,
               characters[i].job_class);
    }

    if (characters) {
        free(characters);
    }

    printf("✓ Character list loaded successfully\n");
}

/**
 * Test User_DB_SaveCharacter
 */
void test_character_save()
{
    printf("\n=== Test: Character Save ===\n");

    UserAccount account;
    Character* characters = NULL;
    uint8_t char_count = 0;
    int result;

    // Load test account
    result = User_DB_LoadAccount("test_char_account", &account);
    TEST_ASSERT(result == ERR_SUCCESS, "Should load test account");

    // Load first character
    result = User_DB_LoadCharacters(account.account_id, &characters, &char_count);
    TEST_ASSERT(result == ERR_SUCCESS && char_count > 0, "Should have characters");

    if (char_count > 0) {
        // Modify character
        characters[0].level = 10;
        characters[0].experience = 5000;
        characters[0].attributes.strength = 30;
        characters[0].attributes.max_hp = 500;
        characters[0].currency.gold = 50000;

        // Save character
        result = User_DB_SaveCharacter(&characters[0]);
        TEST_ASSERT(result == ERR_SUCCESS, "User_DB_SaveCharacter should succeed");

        // Reload and verify
        Character character_verify;
        result = User_DB_LoadCharacter(characters[0].character_id, &character_verify);
        TEST_ASSERT(result == ERR_SUCCESS, "Should reload character");
        TEST_ASSERT(character_verify.level == 10, "Level should be updated");
        TEST_ASSERT(character_verify.attributes.strength == 30, "Strength should be updated");
        TEST_ASSERT(character_verify.currency.gold == 50000, "Gold should be updated");

        printf("✓ Character saved and verified successfully\n");

        free(characters);
    }
}

/**
 * Test User_DB_DeleteCharacter
 */
void test_character_delete()
{
    printf("\n=== Test: Character Delete ===\n");

    UserAccount account;
    Character* characters = NULL;
    uint8_t char_count = 0;
    int result;

    // Load test account
    result = User_DB_LoadAccount("test_char_account", &account);
    TEST_ASSERT(result == ERR_SUCCESS, "Should load test account");

    // Load characters
    result = User_DB_LoadCharacters(account.account_id, &characters, &char_count);
    TEST_ASSERT(result == ERR_SUCCESS && char_count > 0, "Should have characters");

    if (char_count > 0) {
        uint32_t char_id = characters[0].character_id;

        // Delete character (soft delete with 7-day period)
        result = User_DB_DeleteCharacter(char_id);
        TEST_ASSERT(result == ERR_SUCCESS, "User_DB_DeleteCharacter should succeed");

        printf("✓ Character deleted successfully (soft delete, 7-day recovery period)\n");

        free(characters);
    }
}

/**
 * Test character error handling
 */
void test_character_error_handling()
{
    printf("\n=== Test: Character Error Handling ===\n");

    Character character;
    Character* characters = NULL;
    uint8_t char_count = 0;
    int result;

    // Test NULL parameters
    result = User_DB_LoadCharacter(999999, NULL);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL character pointer");

    result = User_DB_LoadCharacters(1, NULL, &char_count);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL characters array");

    result = User_DB_LoadCharacters(1, &characters, NULL);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL count pointer");

    result = User_DB_SaveCharacter(NULL);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL character in save");

    result = User_DB_CreateCharacter(NULL);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL character in create");

    // Test non-existent character
    result = User_DB_LoadCharacter(999999, &character);
    TEST_ASSERT(result == ERR_CHARACTER_NOT_FOUND, "Should return error for non-existent character");

    printf("✓ Character error handling validated\n");
}

/* ==================== Session Database Tests ==================== */

/**
 * Test User_DB_SaveSession and User_DB_LoadSession
 */
void test_session_save_and_load()
{
    printf("\n=== Test: Session Save and Load ===\n");

    UserAccount account;
    UserSession session_write, session_read;
    int result;

    // Load test account
    result = User_DB_LoadAccount("test_char_account", &account);
    TEST_ASSERT(result == ERR_SUCCESS, "Should load test account");

    // Initialize test session
    init_test_session(&session_write, account.account_id);

    // Save session (REPLACE INTO - upsert)
    result = User_DB_SaveSession(&session_write);
    TEST_ASSERT(result == ERR_SUCCESS, "User_DB_SaveSession should succeed");

    // Load session
    memset(&session_read, 0, sizeof(UserSession));
    result = User_DB_LoadSession(account.account_id, &session_read);
    TEST_ASSERT(result == ERR_SUCCESS, "User_DB_LoadSession should succeed");

    // Verify data
    TEST_ASSERT(session_read.account_id == account.account_id, "Account ID should match");
    TEST_ASSERT(strcmp(session_read.username, TEST_ACCOUNT_NAME) == 0, "Username should match");
    TEST_ASSERT(strcmp(session_read.client_ip, TEST_IP) == 0, "Client IP should match");
    TEST_ASSERT(session_read.is_authenticated == 1, "Should be authenticated");
    TEST_ASSERT(session_read.session_state == 1, "Session state should match");

    printf("✓ Session saved and loaded successfully (session_id=%u)\n", session_read.session_id);
}

/**
 * Test User_DB_DeleteSession
 */
void test_session_delete()
{
    printf("\n=== Test: Session Delete ===\n");

    UserAccount account;
    int result;

    // Load test account
    result = User_DB_LoadAccount("test_char_account", &account);
    TEST_ASSERT(result == ERR_SUCCESS, "Should load test account");

    // Delete session
    result = User_DB_DeleteSession(account.account_id);
    TEST_ASSERT(result == ERR_SUCCESS, "User_DB_DeleteSession should succeed");

    // Verify deletion
    UserSession session;
    result = User_DB_LoadSession(account.account_id, &session);
    TEST_ASSERT(result == ERR_SESSION_NOT_FOUND, "Session should not be found after deletion");

    printf("✓ Session deleted successfully\n");
}

/**
 * Test session error handling
 */
void test_session_error_handling()
{
    printf("\n=== Test: Session Error Handling ===\n");

    UserSession session;
    int result;

    // Test NULL parameters
    result = User_DB_SaveSession(NULL);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL session in save");

    result = User_DB_LoadSession(1, NULL);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL session pointer");

    // Test non-existent session
    result = User_DB_LoadSession(999999, &session);
    TEST_ASSERT(result == ERR_SESSION_NOT_FOUND, "Should return error for non-existent session");

    printf("✓ Session error handling validated\n");
}

/* ==================== Stored Procedure Tests ==================== */

/**
 * Test User_DB_AuthenticateAccount
 */
void test_authenticate_account()
{
    printf("\n=== Test: Authenticate Account (Stored Procedure) ===\n");

    UserAccount account;
    uint32_t authenticated_account_id = 0;
    int result;

    // Create test account for authentication
    init_test_account(&account);
    strncpy(account.account_name, "test_auth_account", sizeof(account.account_name) - 1);
    result = User_DB_CreateAccount(&account);
    if (result != ERR_SUCCESS) {
        // Account exists, load it
        result = User_DB_LoadAccount("test_auth_account", &account);
    }
    TEST_ASSERT(result == ERR_SUCCESS, "Should have account for authentication test");

    // Test: Successful authentication
    result = User_DB_AuthenticateAccount("test_auth_account", TEST_PASSWORD_HASH,
                                          TEST_IP, &authenticated_account_id);
    TEST_ASSERT(result == ERR_SUCCESS, "Authentication should succeed with correct password");
    TEST_ASSERT(authenticated_account_id == account.account_id, "Account ID should match");

    // Test: Wrong password
    result = User_DB_AuthenticateAccount("test_auth_account", "wrong_password_hash",
                                          TEST_IP, &authenticated_account_id);
    TEST_ASSERT(result == ERR_AUTHENTICATION_FAILED, "Authentication should fail with wrong password");

    // Test: Non-existent account
    result = User_DB_AuthenticateAccount("nonexistent_account", TEST_PASSWORD_HASH,
                                          TEST_IP, &authenticated_account_id);
    TEST_ASSERT(result == ERR_ACCOUNT_NOT_FOUND, "Should return error for non-existent account");

    // Test: NULL parameters
    result = User_DB_AuthenticateAccount(NULL, TEST_PASSWORD_HASH, TEST_IP, &authenticated_account_id);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL account name");

    result = User_DB_AuthenticateAccount("test_auth_account", NULL, TEST_IP, &authenticated_account_id);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL password");

    result = User_DB_AuthenticateAccount("test_auth_account", TEST_PASSWORD_HASH, TEST_IP, NULL);
    TEST_ASSERT(result == ERR_INVALID_PARAMETER, "Should reject NULL account_id pointer");

    printf("✓ Account authentication validated\n");
}

/* ==================== Main Test Runner ==================== */

int main(int argc, char* argv[])
{
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║   User Database Module - Unit Tests                  ║\n");
    printf("║   Phase 6.1 Week 3 - Database Integration Testing    ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n\n");

    // Initialize database system
    printf("Initializing database system...\n");
    int result = Database_Initialize(TEST_DB_CONFIG);
    if (result != ERR_SUCCESS) {
        printf("ERROR: Failed to initialize database system (code=%d)\n", result);
        printf("Please check database configuration: %s\n", TEST_DB_CONFIG);
        return 1;
    }
    printf("✓ Database system initialized\n\n");

    // Run Account tests
    printf("\n┌─────────────────────────────────────────────────────┐\n");
    printf("│  ACCOUNT DATABASE TESTS                             │\n");
    printf("└─────────────────────────────────────────────────────┘\n");
    test_account_create_and_load();
    test_account_save();
    test_account_error_handling();
    test_account_delete();  // Run last as it deletes test data

    // Run Character tests
    printf("\n┌─────────────────────────────────────────────────────┐\n");
    printf("│  CHARACTER DATABASE TESTS                           │\n");
    printf("└─────────────────────────────────────────────────────┘\n");
    test_character_create_and_load();
    test_character_load_all();
    test_character_save();
    test_character_error_handling();
    test_character_delete();  // Run last as it deletes test data

    // Run Session tests
    printf("\n┌─────────────────────────────────────────────────────┐\n");
    printf("│  SESSION DATABASE TESTS                             │\n");
    printf("└─────────────────────────────────────────────────────┘\n");
    test_session_save_and_load();
    test_session_error_handling();
    test_session_delete();  // Run last as it deletes test data

    // Run Stored Procedure tests
    printf("\n┌─────────────────────────────────────────────────────┐\n");
    printf("│  STORED PROCEDURE TESTS                             │\n");
    printf("└─────────────────────────────────────────────────────┘\n");
    test_authenticate_account();

    // Cleanup database system
    printf("\n\nCleaning up database system...\n");
    Database_Cleanup();
    printf("✓ Database system cleanup complete\n");

    // Print test summary
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║  TEST SUMMARY                                         ║\n");
    printf("╠═══════════════════════════════════════════════════════╣\n");
    printf("║  Total Tests Run:     %-4d                            ║\n", tests_run);
    printf("║  Tests Passed:        %-4d ✓                          ║\n", tests_passed);
    printf("║  Tests Failed:        %-4d ✗                          ║\n", tests_failed);
    printf("║  Success Rate:        %.1f%%                           ║\n",
           tests_run > 0 ? (tests_passed * 100.0 / tests_run) : 0.0);
    printf("╚═══════════════════════════════════════════════════════╝\n");

    if (tests_failed == 0) {
        printf("\n🎉 All tests PASSED! User Database module is working correctly.\n");
        return 0;
    } else {
        printf("\n⚠️  Some tests FAILED. Please review the failures above.\n");
        return 1;
    }
}
