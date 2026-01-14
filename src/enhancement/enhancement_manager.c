/**
 * Enhancement Module - Full Implementation
 *
 * Handles equipment enhancement, amplification, and enchantment
 * Features:
 * - Equipment strengthening (+0 to +12 and beyond)
 * - Amplification system with random stats
 * - Enchantment card application
 * - Success/failure mechanics with probability tables
 * - Protection items to prevent destruction
 * - Per-user and per-item tracking
 */

#include "../../include/enhancement/enhancement_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Constants */
#define MAX_ITEMS 100000
#define MAX_USERS 10000
#define MAX_ENHANCE_LEVEL 15
#define GREAT_SUCCESS_CHANCE 5  /* 5% chance of great success */

/* Amplify ability types */
#define ABILITY_STR 0
#define ABILITY_INT 1
#define ABILITY_VIT 2
#define ABILITY_SPR 3
#define ABILITY_HP 4
#define ABILITY_MP 5
#define ABILITY_PATK 6
#define ABILITY_MATK 7
#define ABILITY_PDEF 8
#define ABILITY_MDEF 9

/* Success rate table for strengthening by target level */
static const int STRENGTHEN_SUCCESS_RATE[MAX_ENHANCE_LEVEL + 1] = {
    100, /* +0 -> +1 */
    100, /* +1 -> +2 */
    95,  /* +2 -> +3 */
    90,  /* +3 -> +4 */
    80,  /* +4 -> +5 */
    70,  /* +5 -> +6 */
    60,  /* +6 -> +7 */
    50,  /* +7 -> +8 */
    40,  /* +8 -> +9 */
    30,  /* +9 -> +10 */
    20,  /* +10 -> +11 */
    15,  /* +11 -> +12 */
    10,  /* +12 -> +13 */
    8,   /* +13 -> +14 */
    5    /* +14 -> +15 */
};

/* Destruction chance on failure by level */
static const int DESTRUCTION_CHANCE[MAX_ENHANCE_LEVEL + 1] = {
    0, 0, 0, 0, 0,     /* +0 to +4: no destruction */
    0, 10, 15, 20, 25, /* +5 to +9: increasing risk */
    30, 40, 50, 60, 70, 80  /* +10+: high risk */
};

/* Cost multiplier by level */
static const int COST_MULTIPLIER[MAX_ENHANCE_LEVEL + 1] = {
    1, 1, 2, 3, 5,
    8, 12, 18, 25, 35,
    50, 70, 100, 150, 200, 300
};

/* Item enhancement data */
typedef struct {
    unsigned int item_id;
    unsigned int owner_id;
    int strengthen_level;  /* +0 to +15 */
    int amplify_level;     /* 0 to 10 */
    AmplifyOption amplify_options[4];
    int amplify_option_count;
    int has_enchantment;
    unsigned int enchant_card_id;
    unsigned int last_enhance_time;
    int is_tracked;
} ItemEnhancementData;

/* User enhancement stats */
typedef struct {
    unsigned int user_id;
    unsigned int total_attempts;
    unsigned int total_successes;
    unsigned int total_failures;
    unsigned int items_destroyed;
    unsigned int great_successes;
    unsigned int total_gold_spent;
    int highest_enhancement_level;
} UserEnhancementStats;

/* Enhancement module global state */
static struct {
    int initialized;
    int max_concurrent_enhancements;

    /* Item data */
    ItemEnhancementData* items;
    int item_count;

    /* User stats */
    UserEnhancementStats* users;
    int user_count;

    /* Random seed */
    unsigned int random_seed;

    /* Statistics */
    struct {
        unsigned int total_strengthen_attempts;
        unsigned int total_strengthen_successes;
        unsigned int total_strengthen_failures;
        unsigned int total_amplify_attempts;
        unsigned int total_amplify_successes;
        unsigned int total_enchant_applications;
        unsigned int total_enchant_removals;
        unsigned int items_destroyed;
        unsigned int great_successes;
        unsigned int enhancements_reset;
        unsigned long long total_gold_spent;
    } stats;
} g_enhance_state = {0};

/* Forward declarations */
static ItemEnhancementData* FindOrCreateItem(unsigned int item_id);
static ItemEnhancementData* FindItem(unsigned int item_id);
static UserEnhancementStats* FindOrCreateUserStats(unsigned int user_id);
static UserEnhancementStats* FindUserStats(unsigned int user_id);
static int RollRandom(int max);
static void GenerateRandomAmplifyOption(AmplifyOption* option);

/**
 * Initialize Enhancement Module
 */
int Enhancement_Initialize(int max_concurrent_enhancements)
{
    if (g_enhance_state.initialized) {
        fprintf(stderr, "[Enhancement] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_concurrent_enhancements <= 0 || max_concurrent_enhancements > 50000) {
        fprintf(stderr, "[Enhancement] Invalid max concurrent enhancements: %d\n",
                max_concurrent_enhancements);
        return ERR_INVALID_PARAMETER;
    }

    printf("[Enhancement] Initializing enhancement module...\n");

    /* Allocate item data */
    g_enhance_state.items = (ItemEnhancementData*)calloc(MAX_ITEMS, sizeof(ItemEnhancementData));
    if (!g_enhance_state.items) {
        fprintf(stderr, "[Enhancement] Failed to allocate item data\n");
        return ERR_MEMORY;
    }

    /* Allocate user stats */
    g_enhance_state.users = (UserEnhancementStats*)calloc(MAX_USERS, sizeof(UserEnhancementStats));
    if (!g_enhance_state.users) {
        free(g_enhance_state.items);
        g_enhance_state.items = NULL;
        fprintf(stderr, "[Enhancement] Failed to allocate user stats\n");
        return ERR_MEMORY;
    }

    g_enhance_state.max_concurrent_enhancements = max_concurrent_enhancements;
    g_enhance_state.item_count = 0;
    g_enhance_state.user_count = 0;
    g_enhance_state.random_seed = (unsigned int)time(NULL);
    g_enhance_state.initialized = 1;

    printf("[Enhancement] Module initialized (max_concurrent=%d)\n",
           max_concurrent_enhancements);

    return ERR_SUCCESS;
}

/**
 * Cleanup Enhancement Module
 */
void Enhancement_Cleanup()
{
    if (!g_enhance_state.initialized) {
        return;
    }

    printf("[Enhancement] Cleaning up enhancement module...\n");
    printf("[Enhancement] Statistics:\n");
    printf("  - Total Strengthen Attempts: %u\n", g_enhance_state.stats.total_strengthen_attempts);
    printf("  - Total Strengthen Successes: %u\n", g_enhance_state.stats.total_strengthen_successes);
    printf("  - Total Strengthen Failures: %u\n", g_enhance_state.stats.total_strengthen_failures);
    printf("  - Total Amplify Attempts: %u\n", g_enhance_state.stats.total_amplify_attempts);
    printf("  - Total Amplify Successes: %u\n", g_enhance_state.stats.total_amplify_successes);
    printf("  - Total Enchant Applications: %u\n", g_enhance_state.stats.total_enchant_applications);
    printf("  - Total Enchant Removals: %u\n", g_enhance_state.stats.total_enchant_removals);
    printf("  - Items Destroyed: %u\n", g_enhance_state.stats.items_destroyed);
    printf("  - Great Successes: %u\n", g_enhance_state.stats.great_successes);
    printf("  - Enhancements Reset: %u\n", g_enhance_state.stats.enhancements_reset);
    printf("  - Total Gold Spent: %llu\n", g_enhance_state.stats.total_gold_spent);
    printf("  - Tracked Items: %d\n", g_enhance_state.item_count);
    printf("  - Active Users: %d\n", g_enhance_state.user_count);

    if (g_enhance_state.items) {
        free(g_enhance_state.items);
        g_enhance_state.items = NULL;
    }

    if (g_enhance_state.users) {
        free(g_enhance_state.users);
        g_enhance_state.users = NULL;
    }

    memset(&g_enhance_state, 0, sizeof(g_enhance_state));

    printf("[Enhancement] Module cleanup complete\n");
}

/**
 * Strengthen Equipment
 */
int Enhancement_Strengthen(unsigned int user_id, unsigned int item_id,
                           int use_protection)
{
    if (!g_enhance_state.initialized) {
        return -ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || item_id == 0) {
        return -ERR_INVALID_PARAMETER;
    }

    ItemEnhancementData* item = FindOrCreateItem(item_id);
    if (!item) {
        return -ERR_MEMORY;
    }

    item->owner_id = user_id;
    item->last_enhance_time = (unsigned int)time(NULL);

    /* Check max level */
    if (item->strengthen_level >= MAX_ENHANCE_LEVEL) {
        return -ERR_LIMIT_EXCEEDED;
    }

    int target_level = item->strengthen_level + 1;

    /* Calculate cost */
    unsigned int cost = Enhancement_CalculateCost(item_id, ENHANCE_TYPE_STRENGTHEN, target_level);

    /*
     * Gold deduction: Caller's responsibility
     *
     * Before calling this function, the caller must:
     * 1. Check if user has enough gold: User_GetCurrency(user_id, CURRENCY_GOLD) >= cost
     * 2. Deduct gold: User_RemoveCurrency(user_id, CURRENCY_GOLD, cost)
     *
     * This separation ensures:
     * - Transaction atomicity (gold deducted only if enhancement attempted)
     * - Proper error handling at the calling layer
     * - Database persistence via User module's ORM integration
     *
     * If enhancement module must handle deduction directly, integrate with:
     *   #include "user/user_module.h"
     *   User_RemoveCurrency(user_id, CURRENCY_GOLD, cost);
     */

    /* Update stats */
    g_enhance_state.stats.total_strengthen_attempts++;
    g_enhance_state.stats.total_gold_spent += cost;

    UserEnhancementStats* user_stats = FindOrCreateUserStats(user_id);
    if (user_stats) {
        user_stats->total_attempts++;
        user_stats->total_gold_spent += cost;
    }

    /* Roll for success */
    int success_rate = STRENGTHEN_SUCCESS_RATE[target_level];
    int roll = RollRandom(100);

    if (roll < success_rate) {
        /* Success */
        item->strengthen_level = target_level;
        g_enhance_state.stats.total_strengthen_successes++;

        if (user_stats) {
            user_stats->total_successes++;
            if (target_level > user_stats->highest_enhancement_level) {
                user_stats->highest_enhancement_level = target_level;
            }
        }

        /* Check for great success (bonus +1) */
        if (RollRandom(100) < GREAT_SUCCESS_CHANCE && target_level < MAX_ENHANCE_LEVEL) {
            item->strengthen_level++;
            g_enhance_state.stats.great_successes++;
            if (user_stats) {
                user_stats->great_successes++;
            }
            return ENHANCE_RESULT_GREAT_SUCCESS;
        }

        return ENHANCE_RESULT_SUCCESS;
    } else {
        /* Failure */
        g_enhance_state.stats.total_strengthen_failures++;
        if (user_stats) {
            user_stats->total_failures++;
        }

        /* Check for destruction */
        int destruction_chance = DESTRUCTION_CHANCE[target_level];
        if (destruction_chance > 0 && RollRandom(100) < destruction_chance) {
            if (use_protection) {
                /* Protected - degrade by 1 instead */
                if (item->strengthen_level > 0) {
                    item->strengthen_level--;
                }
                return ENHANCE_RESULT_PROTECTED;
            } else {
                /* Item destroyed */
                item->strengthen_level = 0;
                item->is_tracked = 0;  /* Remove from tracking */
                g_enhance_state.stats.items_destroyed++;
                if (user_stats) {
                    user_stats->items_destroyed++;
                }
                return ENHANCE_RESULT_DESTROYED;
            }
        }

        /* Normal failure - degrade by 1 */
        if (item->strengthen_level > 0) {
            item->strengthen_level--;
        }

        return ENHANCE_RESULT_FAILURE;
    }
}

/**
 * Amplify Equipment
 */
int Enhancement_Amplify(unsigned int user_id, unsigned int item_id,
                        int amplify_level)
{
    if (!g_enhance_state.initialized) {
        return -ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || item_id == 0) {
        return -ERR_INVALID_PARAMETER;
    }

    if (amplify_level <= 0 || amplify_level > 10) {
        return -ERR_INVALID_PARAMETER;
    }

    ItemEnhancementData* item = FindOrCreateItem(item_id);
    if (!item) {
        return -ERR_MEMORY;
    }

    item->owner_id = user_id;
    item->last_enhance_time = (unsigned int)time(NULL);

    /* Calculate cost */
    unsigned int cost = Enhancement_CalculateCost(item_id, ENHANCE_TYPE_AMPLIFY, amplify_level);

    /*
     * Gold deduction for amplification: Caller's responsibility
     * Same pattern as strengthen - see Enhancement_Strengthen comments.
     */

    g_enhance_state.stats.total_amplify_attempts++;
    g_enhance_state.stats.total_gold_spent += cost;

    /* Amplify success rate decreases with level */
    int success_rate = 100 - (amplify_level * 8);
    if (success_rate < 20) success_rate = 20;

    int roll = RollRandom(100);

    if (roll < success_rate) {
        /* Success - generate random stats */
        item->amplify_level = amplify_level;

        /* Generate amplify options based on level */
        int option_count = (amplify_level / 3) + 1;
        if (option_count > 4) option_count = 4;

        item->amplify_option_count = option_count;
        for (int i = 0; i < option_count; i++) {
            GenerateRandomAmplifyOption(&item->amplify_options[i]);
            item->amplify_options[i].ability_value *= amplify_level;
        }

        g_enhance_state.stats.total_amplify_successes++;

        return ENHANCE_RESULT_SUCCESS;
    } else {
        /* Failure - reset amplify */
        item->amplify_level = 0;
        item->amplify_option_count = 0;
        memset(item->amplify_options, 0, sizeof(item->amplify_options));

        return ENHANCE_RESULT_FAILURE;
    }
}

/**
 * Apply Enchantment
 */
int Enhancement_ApplyEnchant(unsigned int user_id, unsigned int item_id,
                             unsigned int enchant_card_id)
{
    if (!g_enhance_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || item_id == 0 || enchant_card_id == 0) {
        return ERR_INVALID_PARAMETER;
    }

    ItemEnhancementData* item = FindOrCreateItem(item_id);
    if (!item) {
        return ERR_MEMORY;
    }

    /* Check if already enchanted */
    if (item->has_enchantment) {
        return ERR_INVALID_STATE;
    }

    item->owner_id = user_id;
    item->has_enchantment = 1;
    item->enchant_card_id = enchant_card_id;
    item->last_enhance_time = (unsigned int)time(NULL);

    g_enhance_state.stats.total_enchant_applications++;

    /*
     * Enchant card consumption: Caller's responsibility
     *
     * Before calling this function, the caller should:
     * 1. Verify player has the enchant_card_id in inventory
     * 2. After success, remove the card: Inventory_RemoveItem(user_id, enchant_card_id, 1)
     *
     * This separation allows the enhancement module to remain independent
     * of the inventory implementation.
     */

    return ERR_SUCCESS;
}

/**
 * Remove Enchantment
 */
int Enhancement_RemoveEnchant(unsigned int user_id, unsigned int item_id)
{
    if (!g_enhance_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || item_id == 0) {
        return ERR_INVALID_PARAMETER;
    }

    ItemEnhancementData* item = FindItem(item_id);
    if (!item) {
        return ERR_NOT_FOUND;
    }

    if (item->owner_id != user_id) {
        return ERR_PERMISSION_DENIED;
    }

    if (!item->has_enchantment) {
        return ERR_INVALID_STATE;
    }

    item->has_enchantment = 0;
    item->enchant_card_id = 0;
    item->last_enhance_time = (unsigned int)time(NULL);

    g_enhance_state.stats.total_enchant_removals++;

    return ERR_SUCCESS;
}

/**
 * Get Enhancement Info
 */
int Enhancement_GetInfo(unsigned int item_id, EnhancementInfo* enhance_info)
{
    if (!g_enhance_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!enhance_info) {
        return ERR_INVALID_PARAMETER;
    }

    memset(enhance_info, 0, sizeof(EnhancementInfo));

    ItemEnhancementData* item = FindItem(item_id);
    if (!item) {
        enhance_info->item_id = item_id;
        return ERR_SUCCESS;  /* Return default info for untracked items */
    }

    enhance_info->item_id = item->item_id;
    enhance_info->user_id = item->owner_id;
    enhance_info->enhance_type = ENHANCE_TYPE_STRENGTHEN;
    enhance_info->current_level = item->strengthen_level;
    enhance_info->target_level = item->strengthen_level + 1;
    enhance_info->amplify_level = item->amplify_level;
    memcpy(enhance_info->amplify_options, item->amplify_options, sizeof(item->amplify_options));
    enhance_info->has_enchantment = item->has_enchantment;
    enhance_info->enchant_card_id = item->enchant_card_id;
    enhance_info->last_enhance_time = item->last_enhance_time;

    return ERR_SUCCESS;
}

/**
 * Calculate Success Rate
 */
int Enhancement_CalculateSuccessRate(unsigned int item_id,
                                      EnhancementType enhance_type,
                                      int target_level)
{
    if (!g_enhance_state.initialized) {
        return 0;
    }

    (void)item_id;  /* Rate doesn't depend on specific item currently */

    if (target_level < 0 || target_level > MAX_ENHANCE_LEVEL) {
        return 0;
    }

    switch (enhance_type) {
        case ENHANCE_TYPE_STRENGTHEN:
            return STRENGTHEN_SUCCESS_RATE[target_level];

        case ENHANCE_TYPE_AMPLIFY:
            {
                int rate = 100 - (target_level * 8);
                return (rate < 20) ? 20 : rate;
            }

        case ENHANCE_TYPE_ENCHANT:
            return 100;  /* Enchant always succeeds */

        case ENHANCE_TYPE_REFINE:
            return 80;  /* Fixed 80% for refinement */

        default:
            return 0;
    }
}

/**
 * Calculate Enhancement Cost
 */
unsigned int Enhancement_CalculateCost(unsigned int item_id,
                                        EnhancementType enhance_type,
                                        int target_level)
{
    if (!g_enhance_state.initialized) {
        return 0;
    }

    (void)item_id;

    if (target_level < 0 || target_level > MAX_ENHANCE_LEVEL) {
        return 0;
    }

    unsigned int base_cost = 0;

    switch (enhance_type) {
        case ENHANCE_TYPE_STRENGTHEN:
            base_cost = 5000;
            break;

        case ENHANCE_TYPE_AMPLIFY:
            base_cost = 10000;
            break;

        case ENHANCE_TYPE_ENCHANT:
            return 1000;  /* Fixed cost for enchanting */

        case ENHANCE_TYPE_REFINE:
            base_cost = 20000;
            break;

        default:
            return 0;
    }

    return base_cost * COST_MULTIPLIER[target_level];
}

/**
 * Get Enhancement Statistics
 */
int Enhancement_GetStatistics(unsigned int user_id, EnhancementStats* stats)
{
    if (!g_enhance_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!stats) {
        return ERR_INVALID_PARAMETER;
    }

    memset(stats, 0, sizeof(EnhancementStats));

    UserEnhancementStats* user_stats = FindUserStats(user_id);
    if (user_stats) {
        stats->total_attempts = user_stats->total_attempts;
        stats->total_successes = user_stats->total_successes;
        stats->total_failures = user_stats->total_failures;
        stats->items_destroyed = user_stats->items_destroyed;
        stats->great_successes = user_stats->great_successes;
        stats->total_gold_spent = user_stats->total_gold_spent;
        stats->highest_enhancement_level = user_stats->highest_enhancement_level;

        if (stats->total_attempts > 0) {
            stats->success_rate =
                (float)stats->total_successes / stats->total_attempts * 100.0f;
        }
    }

    return ERR_SUCCESS;
}

/**
 * Get Amplify Options
 */
int Enhancement_GetAmplifyOptions(unsigned int item_id,
                                   AmplifyOption* options,
                                   int max_options)
{
    if (!g_enhance_state.initialized) {
        return 0;
    }

    if (!options || max_options <= 0) {
        return 0;
    }

    ItemEnhancementData* item = FindItem(item_id);
    if (!item) {
        return 0;
    }

    int count = (item->amplify_option_count < max_options) ?
                item->amplify_option_count : max_options;

    memcpy(options, item->amplify_options, count * sizeof(AmplifyOption));

    return count;
}

/**
 * Reset Enhancement
 */
int Enhancement_Reset(unsigned int user_id, unsigned int item_id)
{
    if (!g_enhance_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || item_id == 0) {
        return ERR_INVALID_PARAMETER;
    }

    ItemEnhancementData* item = FindItem(item_id);
    if (!item) {
        return ERR_NOT_FOUND;
    }

    if (item->owner_id != user_id) {
        return ERR_PERMISSION_DENIED;
    }

    /* Reset all enhancement data */
    item->strengthen_level = 0;
    item->amplify_level = 0;
    item->amplify_option_count = 0;
    memset(item->amplify_options, 0, sizeof(item->amplify_options));
    item->has_enchantment = 0;
    item->enchant_card_id = 0;
    item->last_enhance_time = (unsigned int)time(NULL);

    g_enhance_state.stats.enhancements_reset++;

    return ERR_SUCCESS;
}

/**
 * Update Enhancement State
 */
int Enhancement_Update(int delta_time_ms)
{
    if (!g_enhance_state.initialized) {
        return 0;
    }

    (void)delta_time_ms;

    /* No periodic processing needed currently */
    return 0;
}

/**
 * Print Statistics
 */
void Enhancement_PrintStats()
{
    if (!g_enhance_state.initialized) {
        printf("[Enhancement] Module not initialized\n");
        return;
    }

    printf("=== Enhancement System Statistics ===\n");
    printf("Tracked Items: %d\n", g_enhance_state.item_count);
    printf("Active Users: %d\n", g_enhance_state.user_count);
    printf("\nGlobal Stats:\n");
    printf("  Strengthen: %u attempts, %u success, %u fail\n",
           g_enhance_state.stats.total_strengthen_attempts,
           g_enhance_state.stats.total_strengthen_successes,
           g_enhance_state.stats.total_strengthen_failures);
    printf("  Amplify: %u attempts, %u success\n",
           g_enhance_state.stats.total_amplify_attempts,
           g_enhance_state.stats.total_amplify_successes);
    printf("  Enchant: %u applied, %u removed\n",
           g_enhance_state.stats.total_enchant_applications,
           g_enhance_state.stats.total_enchant_removals);
    printf("  Destroyed: %u\n", g_enhance_state.stats.items_destroyed);
    printf("  Great Success: %u\n", g_enhance_state.stats.great_successes);
    printf("  Gold Spent: %llu\n", g_enhance_state.stats.total_gold_spent);
}

/* ============================================================
 * Internal Helper Functions
 * ============================================================ */

static ItemEnhancementData* FindOrCreateItem(unsigned int item_id)
{
    /* Find existing */
    for (int i = 0; i < g_enhance_state.item_count; i++) {
        if (g_enhance_state.items[i].is_tracked &&
            g_enhance_state.items[i].item_id == item_id) {
            return &g_enhance_state.items[i];
        }
    }

    /* Create new */
    if (g_enhance_state.item_count >= MAX_ITEMS) {
        return NULL;
    }

    /* Find free slot */
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (!g_enhance_state.items[i].is_tracked) {
            ItemEnhancementData* item = &g_enhance_state.items[i];
            memset(item, 0, sizeof(ItemEnhancementData));
            item->item_id = item_id;
            item->is_tracked = 1;
            g_enhance_state.item_count++;
            return item;
        }
    }

    return NULL;
}

static ItemEnhancementData* FindItem(unsigned int item_id)
{
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (g_enhance_state.items[i].is_tracked &&
            g_enhance_state.items[i].item_id == item_id) {
            return &g_enhance_state.items[i];
        }
    }
    return NULL;
}

static UserEnhancementStats* FindOrCreateUserStats(unsigned int user_id)
{
    /* Find existing */
    for (int i = 0; i < g_enhance_state.user_count; i++) {
        if (g_enhance_state.users[i].user_id == user_id) {
            return &g_enhance_state.users[i];
        }
    }

    /* Create new */
    if (g_enhance_state.user_count >= MAX_USERS) {
        return NULL;
    }

    UserEnhancementStats* user = &g_enhance_state.users[g_enhance_state.user_count];
    memset(user, 0, sizeof(UserEnhancementStats));
    user->user_id = user_id;
    g_enhance_state.user_count++;

    return user;
}

static UserEnhancementStats* FindUserStats(unsigned int user_id)
{
    for (int i = 0; i < g_enhance_state.user_count; i++) {
        if (g_enhance_state.users[i].user_id == user_id) {
            return &g_enhance_state.users[i];
        }
    }
    return NULL;
}

static int RollRandom(int max)
{
    g_enhance_state.random_seed = g_enhance_state.random_seed * 1103515245 + 12345;
    return (int)((g_enhance_state.random_seed >> 16) % max);
}

static void GenerateRandomAmplifyOption(AmplifyOption* option)
{
    /* Random ability type */
    option->ability_type = RollRandom(10);  /* 0-9 for different stats */

    /* Random base value (1-5) */
    option->ability_value = RollRandom(5) + 1;
}
