/**
 * DNF Game Server - Main Entry Point
 *
 * 主程序入口，负责初始化所有19个模块并运行游戏服务器
 *
 * 模块依赖关系：
 *   Layer 0: Common, Security
 *   Layer 1: Database, Network (依赖 Common)
 *   Layer 2: User, Inventory, Events (依赖 Common, Network)
 *   Layer 3: Combat, Dungeon, Party, Quest, Skill, Guild, PvP, Trade, Mail, Shop, Enhancement, Achievement (依赖 Common, User)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// Common module (use relative paths from src/)
#include "common/common_interface.h"
#include "../include/common/headers/error_codes.h"

// Layer 0 modules
#include "security/security_interface.h"

// Layer 1 modules
#include "database/headers/database_interface.h"
#include "network/headers/network_interface.h"

// Layer 2 modules
#include "user/user_interface.h"
#include "inventory/inventory_interface.h"
#include "events/events_interface.h"

// Layer 3 modules (Game Systems)
#include "combat/combat_interface.h"
#include "dungeon/dungeon_interface.h"
#include "party/party_interface.h"
#include "quest/quest_interface.h"
#include "skill/skill_interface.h"
#include "guild/guild_interface.h"
#include "../../include/pvp/pvp_interface.h"
#include "../../include/trade/trade_interface.h"
#include "../../include/mail/mail_interface.h"
#include "../../include/shop/shop_interface.h"
#include "../../include/enhancement/enhancement_interface.h"
#include "../../include/achievement/achievement_interface.h"

// PVF Data module
#include "pvf/pvf_manager.h"

/* Server configuration */
#define DEFAULT_SERVER_PORT           7600
#define DEFAULT_MAX_CONNECTIONS       10000
#define DEFAULT_MAX_USERS             5000
#define DEFAULT_MAX_INVENTORIES       5000
#define DEFAULT_INVENTORY_SLOTS       90
#define DEFAULT_MAX_EVENTS            1000
#define DEFAULT_MAX_EVENT_LISTENERS   100
#define DEFAULT_MAX_COMBATS           1000
#define DEFAULT_MAX_STATUS_EFFECTS    5000
#define DEFAULT_MAX_DUNGEON_INSTANCES 500
#define DEFAULT_MAX_MONSTERS          10000
#define DEFAULT_MAX_PARTIES           1000
#define DEFAULT_MAX_ACTIVE_QUESTS     50
#define DEFAULT_MAX_SKILL_USERS       5000
#define DEFAULT_MAX_GUILDS            1000
#define DEFAULT_MAX_PVP_MATCHES       500
#define DEFAULT_MAX_RANKED_PLAYERS    10000
#define DEFAULT_MAX_CONCURRENT_TRADES 5000
#define DEFAULT_MAX_MAILS_PER_USER    100
#define DEFAULT_MAIL_RETENTION_DAYS   30
#define DEFAULT_MAX_SHOPS             500
#define DEFAULT_MAX_SHOP_TRANSACTIONS 10000
#define DEFAULT_MAX_CONCURRENT_ENHANCEMENTS 1000
#define DEFAULT_MAX_ACHIEVEMENTS      10000
#define DEFAULT_MAX_USER_ACHIEVEMENTS 50000

/* PVF data files */
#define DEFAULT_PVF_PATH              "./Script.pvf"
#define DEFAULT_STRINGTABLE_PATH      "./stringtable.bin"

/* Global server state */
static volatile int g_server_running = 1;

/**
 * Signal handler for graceful shutdown
 */
void signal_handler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\n[Server] Received shutdown signal (%d)\n", sig);
        g_server_running = 0;
    }
}

/**
 * Initialize all game modules
 * Returns ERR_SUCCESS on success, error code on failure
 */
int initialize_all_modules(void)
{
    int result;

    printf("==========================================================\n");
    printf("DNF Game Server - Module Initialization\n");
    printf("==========================================================\n\n");

    /* Layer 0: Foundation modules */

    printf("[1/7] Initializing Common module...\n");
    result = Common_Initialize();
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Common initialization failed: 0x%04X\n", result);
        return result;
    }
    printf("      [OK] Common module initialized\n\n");

    printf("[2/7] Initializing Security module...\n");
    result = Security_Initialize();
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Security initialization failed: 0x%04X\n", result);
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Security module initialized\n\n");

    /* Layer 1: Infrastructure modules */

    printf("[3/7] Initializing Database module...\n");
    result = Database_Initialize("./config/database.conf");
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Database initialization failed: 0x%04X\n", result);
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Database module initialized\n\n");

    printf("[4/7] Initializing Network module...\n");
    result = Network_Initialize(DEFAULT_MAX_CONNECTIONS, DEFAULT_SERVER_PORT);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Network initialization failed: 0x%04X\n", result);
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Network module initialized (port=%d, max_conn=%d)\n\n",
           DEFAULT_SERVER_PORT, DEFAULT_MAX_CONNECTIONS);

    /* Layer 1.5: PVF Data module */

    printf("[4.5/20] Initializing PVF module...\n");
    result = PVF_Manager_Initialize(DEFAULT_PVF_PATH, DEFAULT_STRINGTABLE_PATH);
    if (result != PVF_MGR_SUCCESS) {
        fprintf(stderr, "[WARNING] PVF initialization failed: %d (non-fatal)\n", result);
        fprintf(stderr, "         Server will continue without PVF data support\n\n");
        /* PVF failure is non-fatal - server can run without it */
    } else {
        printf("      [OK] PVF module initialized (%u files)\n\n",
               PVF_Manager_GetFileCount());
    }

    /* Layer 2: Business logic modules */

    printf("[5/7] Initializing User module...\n");
    result = User_Initialize(DEFAULT_MAX_USERS, "./config/auth_backend.conf");
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] User initialization failed: 0x%04X\n", result);
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] User module initialized (max_users=%d)\n\n", DEFAULT_MAX_USERS);

    printf("[6/7] Initializing Inventory module...\n");
    result = Inventory_Initialize(DEFAULT_MAX_INVENTORIES, DEFAULT_INVENTORY_SLOTS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Inventory initialization failed: 0x%04X\n", result);
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Inventory module initialized (max_inv=%d, slots=%d)\n\n",
           DEFAULT_MAX_INVENTORIES, DEFAULT_INVENTORY_SLOTS);

    printf("[7/10] Initializing Events module...\n");
    result = Events_Initialize(DEFAULT_MAX_EVENTS, DEFAULT_MAX_EVENT_LISTENERS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Events initialization failed: 0x%04X\n", result);
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Events module initialized (max_events=%d)\n\n", DEFAULT_MAX_EVENTS);

    /* Layer 3: Game systems */

    printf("[8/10] Initializing Combat module...\n");
    result = Combat_Initialize(DEFAULT_MAX_COMBATS, DEFAULT_MAX_STATUS_EFFECTS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Combat initialization failed: 0x%04X\n", result);
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Combat module initialized (max_combats=%d, max_effects=%d)\n\n",
           DEFAULT_MAX_COMBATS, DEFAULT_MAX_STATUS_EFFECTS);

    printf("[9/10] Initializing Dungeon module...\n");
    result = Dungeon_Initialize(DEFAULT_MAX_DUNGEON_INSTANCES, DEFAULT_MAX_MONSTERS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Dungeon initialization failed: 0x%04X\n", result);
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Dungeon module initialized (max_instances=%d, max_monsters=%d)\n\n",
           DEFAULT_MAX_DUNGEON_INSTANCES, DEFAULT_MAX_MONSTERS);

    printf("[10/13] Initializing Party module...\n");
    result = Party_Initialize(DEFAULT_MAX_PARTIES);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Party initialization failed: 0x%04X\n", result);
        Dungeon_Cleanup();
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Party module initialized (max_parties=%d)\n\n", DEFAULT_MAX_PARTIES);

    printf("[11/13] Initializing Quest module...\n");
    result = Quest_Initialize(DEFAULT_MAX_ACTIVE_QUESTS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Quest initialization failed: 0x%04X\n", result);
        Party_Cleanup();
        Dungeon_Cleanup();
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Quest module initialized (max_active_quests=%d)\n\n", DEFAULT_MAX_ACTIVE_QUESTS);

    printf("[12/13] Initializing Skill module...\n");
    result = Skill_Initialize(DEFAULT_MAX_SKILL_USERS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Skill initialization failed: 0x%04X\n", result);
        Quest_Cleanup();
        Party_Cleanup();
        Dungeon_Cleanup();
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Skill module initialized (max_users=%d)\n\n", DEFAULT_MAX_SKILL_USERS);

    printf("[13/16] Initializing Guild module...\n");
    result = Guild_Initialize(DEFAULT_MAX_GUILDS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Guild initialization failed: 0x%04X\n", result);
        Skill_Cleanup();
        Quest_Cleanup();
        Party_Cleanup();
        Dungeon_Cleanup();
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Guild module initialized (max_guilds=%d)\n\n", DEFAULT_MAX_GUILDS);

    printf("[14/16] Initializing PvP module...\n");
    result = PvP_Initialize(DEFAULT_MAX_PVP_MATCHES, DEFAULT_MAX_RANKED_PLAYERS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] PvP initialization failed: 0x%04X\n", result);
        Guild_Cleanup();
        Skill_Cleanup();
        Quest_Cleanup();
        Party_Cleanup();
        Dungeon_Cleanup();
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] PvP module initialized (max_matches=%d, max_ranked=%d)\n\n",
           DEFAULT_MAX_PVP_MATCHES, DEFAULT_MAX_RANKED_PLAYERS);

    printf("[15/16] Initializing Trade module...\n");
    result = Trade_Initialize(DEFAULT_MAX_CONCURRENT_TRADES);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Trade initialization failed: 0x%04X\n", result);
        PvP_Cleanup();
        Guild_Cleanup();
        Skill_Cleanup();
        Quest_Cleanup();
        Party_Cleanup();
        Dungeon_Cleanup();
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Trade module initialized (max_concurrent_trades=%d)\n\n",
           DEFAULT_MAX_CONCURRENT_TRADES);

    printf("[16/19] Initializing Mail module...\n");
    result = Mail_Initialize(DEFAULT_MAX_MAILS_PER_USER, DEFAULT_MAIL_RETENTION_DAYS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Mail initialization failed: 0x%04X\n", result);
        Trade_Cleanup();
        PvP_Cleanup();
        Guild_Cleanup();
        Skill_Cleanup();
        Quest_Cleanup();
        Party_Cleanup();
        Dungeon_Cleanup();
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Mail module initialized (max_mails_per_user=%d, retention_days=%d)\n\n",
           DEFAULT_MAX_MAILS_PER_USER, DEFAULT_MAIL_RETENTION_DAYS);

    printf("[17/19] Initializing Shop module...\n");
    result = Shop_Initialize(DEFAULT_MAX_SHOPS, DEFAULT_MAX_SHOP_TRANSACTIONS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Shop initialization failed: 0x%04X\n", result);
        Mail_Cleanup();
        Trade_Cleanup();
        PvP_Cleanup();
        Guild_Cleanup();
        Skill_Cleanup();
        Quest_Cleanup();
        Party_Cleanup();
        Dungeon_Cleanup();
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Shop module initialized (max_shops=%d, max_transactions=%d)\n\n",
           DEFAULT_MAX_SHOPS, DEFAULT_MAX_SHOP_TRANSACTIONS);

    printf("[18/19] Initializing Enhancement module...\n");
    result = Enhancement_Initialize(DEFAULT_MAX_CONCURRENT_ENHANCEMENTS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Enhancement initialization failed: 0x%04X\n", result);
        Shop_Cleanup();
        Mail_Cleanup();
        Trade_Cleanup();
        PvP_Cleanup();
        Guild_Cleanup();
        Skill_Cleanup();
        Quest_Cleanup();
        Party_Cleanup();
        Dungeon_Cleanup();
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Enhancement module initialized (max_concurrent_enhancements=%d)\n\n",
           DEFAULT_MAX_CONCURRENT_ENHANCEMENTS);

    printf("[19/19] Initializing Achievement module...\n");
    result = Achievement_Initialize(DEFAULT_MAX_ACHIEVEMENTS, DEFAULT_MAX_USER_ACHIEVEMENTS);
    if (IS_FAILURE(result)) {
        fprintf(stderr, "[ERROR] Achievement initialization failed: 0x%04X\n", result);
        Enhancement_Cleanup();
        Shop_Cleanup();
        Mail_Cleanup();
        Trade_Cleanup();
        PvP_Cleanup();
        Guild_Cleanup();
        Skill_Cleanup();
        Quest_Cleanup();
        Party_Cleanup();
        Dungeon_Cleanup();
        Combat_Cleanup();
        Events_Cleanup();
        Inventory_Cleanup();
        User_Cleanup();
        Network_Cleanup();
        Database_Cleanup();
        Security_Cleanup();
        Common_Cleanup();
        return result;
    }
    printf("      [OK] Achievement module initialized (max_achievements=%d, max_user_achievements=%d)\n\n",
           DEFAULT_MAX_ACHIEVEMENTS, DEFAULT_MAX_USER_ACHIEVEMENTS);

    printf("==========================================================\n");
    printf("All 19 modules initialized successfully!\n");
    printf("==========================================================\n\n");

    return ERR_SUCCESS;
}

/**
 * Cleanup all game modules (reverse order of initialization)
 */
void cleanup_all_modules(void)
{
    printf("\n==========================================================\n");
    printf("DNF Game Server - Module Cleanup\n");
    printf("==========================================================\n\n");

    printf("[19/19] Cleaning up Achievement module...\n");
    Achievement_Cleanup();

    printf("[18/19] Cleaning up Enhancement module...\n");
    Enhancement_Cleanup();

    printf("[17/19] Cleaning up Shop module...\n");
    Shop_Cleanup();

    printf("[16/19] Cleaning up Mail module...\n");
    Mail_Cleanup();

    printf("[15/19] Cleaning up Trade module...\n");
    Trade_Cleanup();

    printf("[14/19] Cleaning up PvP module...\n");
    PvP_Cleanup();

    printf("[13/19] Cleaning up Guild module...\n");
    Guild_Cleanup();

    printf("[12/19] Cleaning up Skill module...\n");
    Skill_Cleanup();

    printf("[11/19] Cleaning up Quest module...\n");
    Quest_Cleanup();

    printf("[10/19] Cleaning up Party module...\n");
    Party_Cleanup();

    printf("[9/19] Cleaning up Dungeon module...\n");
    Dungeon_Cleanup();

    printf("[8/19] Cleaning up Combat module...\n");
    Combat_Cleanup();

    printf("[7/19] Cleaning up Events module...\n");
    Events_Cleanup();

    printf("[6/19] Cleaning up Inventory module...\n");
    Inventory_Cleanup();

    printf("[5/19] Cleaning up User module...\n");
    User_Cleanup();

    printf("[4.5/20] Cleaning up PVF module...\n");
    if (PVF_Manager_IsInitialized()) {
        PVF_Manager_Cleanup();
    }

    printf("[4/20] Cleaning up Network module...\n");
    Network_Cleanup();

    printf("[3/20] Cleaning up Database module...\n");
    Database_Cleanup();

    printf("[2/19] Cleaning up Security module...\n");
    Security_Cleanup();

    printf("[1/19] Cleaning up Common module...\n");
    Common_Cleanup();

    printf("\n==========================================================\n");
    printf("All modules cleaned up successfully\n");
    printf("==========================================================\n");
}

/**
 * Main server loop
 */
void run_server_loop(void)
{
    unsigned long tick_count = 0;

    printf("\n==========================================================\n");
    printf("DNF Game Server - Running\n");
    printf("==========================================================\n");
    printf("Server is now accepting connections on port %d\n", DEFAULT_SERVER_PORT);
    printf("Press Ctrl+C to shutdown gracefully\n");
    printf("==========================================================\n\n");

    while (g_server_running) {
        tick_count++;

        /*
         * Server main loop: Production implementation steps
         *
         * 1. Network events (epoll):
         *    Network_ProcessEvents(100);  // 100ms timeout
         *    - Handles incoming connections, packet receive/send
         *    - Dispatches packets to PacketHandler functions
         *
         * 2. Game tick update:
         *    Combat_Update(delta_ms);     // Process combat timers
         *    Dungeon_Update(delta_ms);    // Dungeon instance timers
         *    Quest_Update(delta_ms);      // Quest expiration checks
         *    Event_Update(delta_ms);      // Scheduled events
         *
         * 3. Periodic tasks (by tick interval):
         *    if (tick_count % 60 == 0) {  // Every minute
         *        Session_CleanupInactive();
         *        Database_FlushPendingWrites();
         *    }
         *    if (tick_count % 3600 == 0) { // Every hour
         *        Quest_ResetDailyQuests();
         *        Achievement_ProcessPeriodic();
         *    }
         *
         * 4. Database query processing:
         *    Database_ProcessAsyncResults();  // Handle completed async queries
         *
         * Current: Simplified heartbeat loop for testing
         */

        /* Heartbeat output every 10 seconds */
        if (tick_count % 10 == 0) {
            printf("[Server] Heartbeat... tick=%lu, active_users=%d\n",
                   tick_count, User_GetActiveSessionCount());
        }

        // 模拟tick间隔 (1秒)
        sleep(1);
    }

    printf("\n[Server] Main loop exited\n");
}

/**
 * Main entry point
 */
int main(int argc, char *argv[])
{
    int result;

    // 打印启动信息
    printf("\n");
    printf("██████╗ ███╗   ██╗███████╗     ██████╗  █████╗ ███╗   ███╗███████╗\n");
    printf("██╔══██╗████╗  ██║██╔════╝    ██╔════╝ ██╔══██╗████╗ ████║██╔════╝\n");
    printf("██║  ██║██╔██╗ ██║█████╗      ██║  ███╗███████║██╔████╔██║█████╗  \n");
    printf("██║  ██║██║╚██╗██║██╔══╝      ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  \n");
    printf("██████╔╝██║ ╚████║██║         ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗\n");
    printf("╚═════╝ ╚═╝  ╚═══╝╚═╝          ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝\n");
    printf("\n");
    printf("DNF Game Server v0.1.0 - Phase 5.8 Integration Build\n");
    printf("Modularized Architecture - 19 Modules\n");
    printf("Build Date: 2025-11-07\n");
    printf("\n");

    // 安装信号处理器
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // 初始化所有模块
    result = initialize_all_modules();
    if (IS_FAILURE(result)) {
        fprintf(stderr, "\n[FATAL] Server initialization failed with error: 0x%04X\n", result);
        fprintf(stderr, "[FATAL] Server cannot start\n\n");
        return EXIT_FAILURE;
    }

    // 运行主服务器循环
    run_server_loop();

    // 清理所有模块
    cleanup_all_modules();

    printf("\n[Server] Shutdown complete\n\n");
    return EXIT_SUCCESS;
}
