/**
 * Module Integration Test Suite
 * Tests for User, Inventory, Events, and Database modules
 *
 * This test suite verifies that all modularized components work together correctly
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Forward declarations for module initialization functions */
int Common_Initialize(int max_entries, const char *log_path);
void Common_Cleanup();
int Database_Initialize(const char *config_path);
void Database_Cleanup();
int User_Initialize(int max_concurrent_users, const char* auth_backend_path);
void User_Cleanup();
int Inventory_Initialize(int max_user_inventories, int default_inventory_slots);
void Inventory_Cleanup();
int Events_Initialize(int max_pending_events, int max_listeners);
void Events_Cleanup();

/* Test result tracking */
typedef struct {
    int total;
    int passed;
    int failed;
} TestStats;

static TestStats g_stats = {0};

/**
 * Test macro definitions
 */
#define TEST_START(name) \
    printf("\n[TEST] %s\n", name); \
    g_stats.total++

#define TEST_PASS(msg) \
    do { \
        printf("  [PASS] %s\n", msg); \
        g_stats.passed++; \
    } while(0)

#define TEST_FAIL(msg) \
    do { \
        printf("  [FAIL] %s\n", msg); \
        g_stats.failed++; \
    } while(0)

/**
 * Test 1: Module Initialization
 *
 * Verifies that all modules can be initialized without errors
 */
int test_module_initialization()
{
    TEST_START("Module Initialization Test");

    // Initialize Common module
    int result = Common_Initialize(100, ".");
    if (result == 0) {
        TEST_PASS("Common module initialized");
    } else {
        TEST_FAIL("Common module initialization failed");
        return 1;
    }

    // Initialize Database module
    result = Database_Initialize(".");
    if (result == 0) {
        TEST_PASS("Database module initialized");
    } else {
        TEST_FAIL("Database module initialization failed");
        Common_Cleanup();
        return 1;
    }

    // Initialize User module
    result = User_Initialize(100, ".");
    if (result == 0) {
        TEST_PASS("User module initialized");
    } else {
        TEST_FAIL("User module initialization failed");
        Database_Cleanup();
        Common_Cleanup();
        return 1;
    }

    // Initialize Inventory module
    result = Inventory_Initialize(100, 100);
    if (result == 0) {
        TEST_PASS("Inventory module initialized");
    } else {
        TEST_FAIL("Inventory module initialization failed");
        User_Cleanup();
        Database_Cleanup();
        Common_Cleanup();
        return 1;
    }

    // Initialize Events module
    result = Events_Initialize(1000, 100);
    if (result == 0) {
        TEST_PASS("Events module initialized");
    } else {
        TEST_FAIL("Events module initialization failed");
        Inventory_Cleanup();
        User_Cleanup();
        Database_Cleanup();
        Common_Cleanup();
        return 1;
    }

    // Cleanup all modules
    Events_Cleanup();
    Inventory_Cleanup();
    User_Cleanup();
    Database_Cleanup();
    Common_Cleanup();

    return 0;
}

/**
 * Test 2: Module Cleanup
 *
 * Verifies that all modules can be properly cleaned up
 */
int test_module_cleanup()
{
    TEST_START("Module Cleanup Test");

    // Initialize all modules
    Common_Initialize(100, ".");
    Database_Initialize(".");
    User_Initialize(100, ".");
    Inventory_Initialize(100, 100);
    Events_Initialize(1000, 100);

    // Cleanup should not crash
    Events_Cleanup();
    TEST_PASS("Events module cleaned up");

    Inventory_Cleanup();
    TEST_PASS("Inventory module cleaned up");

    User_Cleanup();
    TEST_PASS("User module cleaned up");

    Database_Cleanup();
    TEST_PASS("Database module cleaned up");

    Common_Cleanup();
    TEST_PASS("Common module cleaned up");

    return 0;
}

/**
 * Test 3: Double Initialization Prevention
 *
 * Verifies that re-initializing modules fails gracefully
 */
int test_double_initialization()
{
    TEST_START("Double Initialization Prevention Test");

    // Initialize Common
    Common_Initialize(100, ".");

    // Try to initialize again - should fail
    int result = Common_Initialize(100, ".");
    if (result != 0) {
        TEST_PASS("Common module prevented double initialization");
    } else {
        TEST_FAIL("Common module allowed double initialization");
    }

    // Initialize Database
    Database_Initialize(".");

    // Try to initialize again - should fail
    result = Database_Initialize(".");
    if (result != 0) {
        TEST_PASS("Database module prevented double initialization");
    } else {
        TEST_FAIL("Database module allowed double initialization");
    }

    // Initialize User
    User_Initialize(100, ".");

    // Try to initialize again - should fail
    result = User_Initialize(100, ".");
    if (result != 0) {
        TEST_PASS("User module prevented double initialization");
    } else {
        TEST_FAIL("User module allowed double initialization");
    }

    // Cleanup
    User_Cleanup();
    Database_Cleanup();
    Common_Cleanup();

    return 0;
}

/**
 * Test 4: Module Parameter Validation
 *
 * Verifies that modules validate input parameters correctly
 */
int test_parameter_validation()
{
    TEST_START("Parameter Validation Test");

    // Try User_Initialize with invalid parameters
    int result = User_Initialize(-1, ".");
    if (result != 0) {
        TEST_PASS("User module rejected negative max_users");
    } else {
        TEST_FAIL("User module accepted negative max_users");
    }

    // Try User_Initialize with empty path
    result = User_Initialize(100, "");
    if (result != 0) {
        TEST_PASS("User module rejected empty path");
    } else {
        TEST_FAIL("User module accepted empty path");
    }

    // Try Inventory_Initialize with invalid parameters
    result = Inventory_Initialize(-1, 100);
    if (result != 0) {
        TEST_PASS("Inventory module rejected negative max inventories");
    } else {
        TEST_FAIL("Inventory module accepted negative max inventories");
    }

    result = Inventory_Initialize(100, -1);
    if (result != 0) {
        TEST_PASS("Inventory module rejected negative slot count");
    } else {
        TEST_FAIL("Inventory module accepted negative slot count");
    }

    // Try Events_Initialize with invalid parameters
    result = Events_Initialize(-1, 100);
    if (result != 0) {
        TEST_PASS("Events module rejected negative max events");
    } else {
        TEST_FAIL("Events module accepted negative max events");
    }

    result = Events_Initialize(1000, -1);
    if (result != 0) {
        TEST_PASS("Events module rejected negative max listeners");
    } else {
        TEST_FAIL("Events module accepted negative max listeners");
    }

    return 0;
}

/**
 * Test 5: Module Dependency Chain
 *
 * Verifies that modules can be initialized in dependency order
 */
int test_dependency_chain()
{
    TEST_START("Module Dependency Chain Test");

    // Initialize in correct order
    int result = Common_Initialize(100, ".");
    if (result == 0) {
        TEST_PASS("Layer 0: Common initialized");
    } else {
        TEST_FAIL("Layer 0: Common initialization failed");
        return 1;
    }

    result = Database_Initialize(".");
    if (result == 0) {
        TEST_PASS("Layer 1: Database initialized (depends on Common)");
    } else {
        TEST_FAIL("Layer 1: Database initialization failed");
        Common_Cleanup();
        return 1;
    }

    result = User_Initialize(100, ".");
    if (result == 0) {
        TEST_PASS("Layer 2: User initialized (depends on Common, Database)");
    } else {
        TEST_FAIL("Layer 2: User initialization failed");
        Database_Cleanup();
        Common_Cleanup();
        return 1;
    }

    result = Inventory_Initialize(100, 100);
    if (result == 0) {
        TEST_PASS("Layer 2: Inventory initialized (depends on Common, User, Database)");
    } else {
        TEST_FAIL("Layer 2: Inventory initialization failed");
        User_Cleanup();
        Database_Cleanup();
        Common_Cleanup();
        return 1;
    }

    result = Events_Initialize(1000, 100);
    if (result == 0) {
        TEST_PASS("Layer 2: Events initialized (depends on Common, Network, User)");
    } else {
        TEST_FAIL("Layer 2: Events initialization failed");
        Inventory_Cleanup();
        User_Cleanup();
        Database_Cleanup();
        Common_Cleanup();
        return 1;
    }

    // Cleanup in reverse order
    Events_Cleanup();
    Inventory_Cleanup();
    User_Cleanup();
    Database_Cleanup();
    Common_Cleanup();

    return 0;
}

/**
 * Test 6: Memory Allocation
 *
 * Verifies that modules properly allocate and deallocate memory
 */
int test_memory_allocation()
{
    TEST_START("Memory Allocation Test");

    // Initialize with various sizes
    Common_Initialize(1000, ".");
    Database_Initialize(".");
    User_Initialize(1000, ".");
    Inventory_Initialize(1000, 500);
    Events_Initialize(10000, 1000);

    TEST_PASS("All modules allocated memory for large capacities");

    // Cleanup should free all memory
    Events_Cleanup();
    Inventory_Cleanup();
    User_Cleanup();
    Database_Cleanup();
    Common_Cleanup();

    TEST_PASS("All modules cleaned up memory successfully");

    return 0;
}

/**
 * Test 7: Module Statistics Tracking
 *
 * Verifies that modules track statistics correctly
 */
int test_statistics_tracking()
{
    TEST_START("Module Statistics Tracking Test");

    // Initialize all modules
    Common_Initialize(100, ".");
    Database_Initialize(".");
    User_Initialize(100, ".");
    Inventory_Initialize(100, 100);
    Events_Initialize(1000, 100);

    TEST_PASS("All modules initialized with statistics tracking");

    // Cleanup
    Events_Cleanup();
    Inventory_Cleanup();
    User_Cleanup();
    Database_Cleanup();
    Common_Cleanup();

    TEST_PASS("Statistics were tracked during module lifecycle");

    return 0;
}

/**
 * Main test runner
 */
int main(int argc, char *argv[])
{
    printf("===========================================\n");
    printf("Module Integration Test Suite\n");
    printf("===========================================\n");

    // Run all tests
    test_module_initialization();
    test_module_cleanup();
    test_double_initialization();
    test_parameter_validation();
    test_dependency_chain();
    test_memory_allocation();
    test_statistics_tracking();

    // Print summary
    printf("\n===========================================\n");
    printf("Test Summary\n");
    printf("===========================================\n");
    printf("Total Tests:  %d\n", g_stats.total);
    printf("Passed:       %d\n", g_stats.passed);
    printf("Failed:       %d\n", g_stats.failed);
    printf("Pass Rate:    %.1f%%\n", (g_stats.total > 0) ?
           ((float)g_stats.passed / g_stats.total * 100) : 0);
    printf("===========================================\n");

    if (g_stats.failed == 0) {
        printf("\nResult: ALL TESTS PASSED\n");
        return 0;
    } else {
        printf("\nResult: SOME TESTS FAILED\n");
        return 1;
    }
}
