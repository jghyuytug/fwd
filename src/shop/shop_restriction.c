/**
 * @file shop_restriction.c
 * @brief Shop Restrictions - Purchase limit system implementation
 *
 * Phase 6.6 Shop System - Restrictions Component
 * Date: 2025-11-23
 *
 * DATA TRACEABILITY:
 * ✅ All fields from charac_cerashop_restrict ORM table
 * ✅ Support for daily/weekly/monthly/permanent restrictions
 * ❌ NO FABRICATED VALUES
 */

#include "shop/shop_restriction.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Database Integration */
#include "../../include/database/headers/db_connection_manager.h"
#include "../../src/orm/core_game/taiwan_cain/headers/charac_cerashop_restrict_orm.h"

/* Database manager for restriction persistence */
static DBConnectionManager* g_restriction_db_manager = NULL;

/**
 * Set database manager for shop restrictions
 */
void ShopRestriction_SetDatabaseManager(DBConnectionManager* db_manager) {
    g_restriction_db_manager = db_manager;
}

/* ========================================================================
 * GLOBAL STATE
 * ======================================================================== */

static RestrictionManagerState g_manager_state = {0};

/* ========================================================================
 * INTERNAL HELPER FUNCTIONS
 * ======================================================================== */

/**
 * Find player restriction data
 */
static PlayerRestrictionData* FindPlayerData(unsigned int character_id) {
    if (!g_manager_state.is_initialized) {
        return NULL;
    }

    for (int i = 0; i < g_manager_state.max_players; i++) {
        if (g_manager_state.player_data[i].character_id == character_id) {
            return &g_manager_state.player_data[i];
        }
    }

    return NULL;
}

/**
 * Allocate player restriction data
 */
static PlayerRestrictionData* AllocatePlayerData(unsigned int character_id) {
    for (int i = 0; i < g_manager_state.max_players; i++) {
        if (g_manager_state.player_data[i].character_id == 0) {
            memset(&g_manager_state.player_data[i], 0, sizeof(PlayerRestrictionData));
            g_manager_state.player_data[i].character_id = character_id;
            g_manager_state.active_players++;
            return &g_manager_state.player_data[i];
        }
    }

    return NULL;
}

/**
 * Get or create player data
 */
static PlayerRestrictionData* GetOrCreatePlayerData(unsigned int character_id) {
    PlayerRestrictionData* data = FindPlayerData(character_id);
    if (!data) {
        data = AllocatePlayerData(character_id);
    }
    return data;
}

/**
 * Find restriction record for player and item
 */
static CharacCerashopRestrict* FindRestrictionRecord(PlayerRestrictionData* player_data,
                                                      unsigned int item_id) {
    if (!player_data) return NULL;

    for (int i = 0; i < player_data->restriction_count; i++) {
        if (player_data->restrictions[i].ipg_no == item_id) {
            return &player_data->restrictions[i];
        }
    }

    return NULL;
}

/**
 * Add restriction record to player
 */
static CharacCerashopRestrict* AddRestrictionRecord(PlayerRestrictionData* player_data,
                                                     unsigned int item_id) {
    if (!player_data) return NULL;

    if (player_data->restriction_count >= SHOP_MAX_RESTRICTIONS_PER_PLAYER) {
        return NULL; /* Full */
    }

    int index = player_data->restriction_count++;
    memset(&player_data->restrictions[index], 0, sizeof(CharacCerashopRestrict));
    player_data->restrictions[index].charac_no = player_data->character_id;
    player_data->restrictions[index].ipg_no = item_id;

    return &player_data->restrictions[index];
}

/**
 * Find item restriction config
 */
static ItemRestrictionConfig* FindItemConfig(unsigned int item_id) {
    if (!g_manager_state.is_initialized) {
        return NULL;
    }

    for (int i = 0; i < g_manager_state.max_items; i++) {
        if (g_manager_state.item_configs[i].item_id == item_id) {
            return &g_manager_state.item_configs[i];
        }
    }

    return NULL;
}

/**
 * Allocate item config
 */
static ItemRestrictionConfig* AllocateItemConfig(unsigned int item_id) {
    for (int i = 0; i < g_manager_state.max_items; i++) {
        if (g_manager_state.item_configs[i].item_id == 0) {
            memset(&g_manager_state.item_configs[i], 0, sizeof(ItemRestrictionConfig));
            g_manager_state.item_configs[i].item_id = item_id;
            g_manager_state.active_configs++;
            return &g_manager_state.item_configs[i];
        }
    }

    return NULL;
}

/* ========================================================================
 * CORE API IMPLEMENTATION
 * ======================================================================== */

int ShopRestriction_Initialize(int max_players, int max_items,
                                DBConnectionManager* db_manager) {
    if (g_manager_state.is_initialized) {
        return -1;
    }

    /* Allocate player data */
    g_manager_state.player_data = calloc(max_players, sizeof(PlayerRestrictionData));
    if (!g_manager_state.player_data) {
        return -2;
    }

    /* Allocate item configs */
    g_manager_state.item_configs = calloc(max_items, sizeof(ItemRestrictionConfig));
    if (!g_manager_state.item_configs) {
        free(g_manager_state.player_data);
        return -2;
    }

    g_manager_state.max_players = max_players;
    g_manager_state.max_items = max_items;
    g_manager_state.active_players = 0;
    g_manager_state.active_configs = 0;
    g_manager_state.db_manager = db_manager;

    memset(&g_manager_state.global_stats, 0, sizeof(RestrictionStatistics));

    g_manager_state.is_initialized = 1;
    return 0;
}

void ShopRestriction_Cleanup(void) {
    if (!g_manager_state.is_initialized) {
        return;
    }

    free(g_manager_state.player_data);
    free(g_manager_state.item_configs);

    memset(&g_manager_state, 0, sizeof(RestrictionManagerState));
}

/* ========================================================================
 * RESTRICTION CONFIGURATION API IMPLEMENTATION
 * ======================================================================== */

int ShopRestriction_ConfigureItem(unsigned int item_id, RestrictionType type,
                                  RestrictionPeriod period,
                                  unsigned int max_count,
                                  unsigned int cooldown_seconds) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    ItemRestrictionConfig* config = FindItemConfig(item_id);
    if (!config) {
        config = AllocateItemConfig(item_id);
        if (!config) {
            return -2;
        }
    }

    config->type = type;
    config->period = period;
    config->max_count = max_count;
    config->cooldown_seconds = cooldown_seconds;

    /* Calculate period duration */
    switch (period) {
        case RESTRICTION_PERIOD_DAILY:
            config->period_duration = 86400; /* 1 day */
            break;
        case RESTRICTION_PERIOD_WEEKLY:
            config->period_duration = 604800; /* 7 days */
            break;
        case RESTRICTION_PERIOD_MONTHLY:
            config->period_duration = 2592000; /* 30 days */
            break;
        default:
            config->period_duration = 0;
            break;
    }

    return 0;
}

int ShopRestriction_RemoveItemConfig(unsigned int item_id) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    ItemRestrictionConfig* config = FindItemConfig(item_id);
    if (!config) {
        return -2;
    }

    memset(config, 0, sizeof(ItemRestrictionConfig));
    g_manager_state.active_configs--;

    return 0;
}

int ShopRestriction_GetItemConfig(unsigned int item_id, ItemRestrictionConfig* out_config) {
    if (!g_manager_state.is_initialized || !out_config) {
        return -1;
    }

    ItemRestrictionConfig* config = FindItemConfig(item_id);
    if (!config) {
        return -2;
    }

    memcpy(out_config, config, sizeof(ItemRestrictionConfig));
    return 0;
}

/* ========================================================================
 * RESTRICTION CHECKING API IMPLEMENTATION
 * ======================================================================== */

int ShopRestriction_CheckPurchase(unsigned int character_id, unsigned int item_id,
                                  unsigned int quantity, time_t current_time) {
    if (!g_manager_state.is_initialized) {
        return RESTRICTION_ERROR_NOT_FOUND;
    }

    /* Get item config */
    ItemRestrictionConfig* config = FindItemConfig(item_id);
    if (!config) {
        return RESTRICTION_OK; /* No restriction configured */
    }

    /* Get or create player data */
    PlayerRestrictionData* player_data = GetOrCreatePlayerData(character_id);
    if (!player_data) {
        return RESTRICTION_ERROR_NOT_FOUND;
    }

    /* Get or create restriction record */
    CharacCerashopRestrict* record = FindRestrictionRecord(player_data, item_id);
    if (!record) {
        /* First purchase - allowed */
        return RESTRICTION_OK;
    }

    /* Check count-based restrictions */
    if (config->type == RESTRICTION_TYPE_COUNT || config->type == RESTRICTION_TYPE_BOTH) {
        /* Check if period expired */
        if (ShopRestriction_IsPeriodExpired(record, current_time)) {
            /* Period expired - reset count and allow */
            record->count = 0;
            record->end_date = (unsigned int)ShopRestriction_CalculatePeriodEnd(
                config->period, current_time);
            return RESTRICTION_OK;
        }

        /* Check count limit */
        if (config->max_count > 0 && record->count + quantity > config->max_count) {
            g_manager_state.global_stats.total_denied_purchases++;
            return RESTRICTION_ERROR_LIMIT_REACHED;
        }
    }

    /* Check time-based restrictions (cooldown) */
    if (config->type == RESTRICTION_TYPE_TIME || config->type == RESTRICTION_TYPE_BOTH) {
        if (current_time < record->next_date) {
            g_manager_state.global_stats.total_denied_purchases++;
            return RESTRICTION_ERROR_TIME_LOCKED;
        }
    }

    return RESTRICTION_OK;
}

int ShopRestriction_GetRemainingPurchases(unsigned int character_id, unsigned int item_id) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    ItemRestrictionConfig* config = FindItemConfig(item_id);
    if (!config || config->max_count == 0) {
        return 999999; /* Unlimited */
    }

    PlayerRestrictionData* player_data = FindPlayerData(character_id);
    if (!player_data) {
        return config->max_count; /* Never purchased */
    }

    CharacCerashopRestrict* record = FindRestrictionRecord(player_data, item_id);
    if (!record) {
        return config->max_count;
    }

    /* Check if period expired */
    if (ShopRestriction_IsPeriodExpired(record, time(NULL))) {
        return config->max_count; /* Reset */
    }

    int remaining = (int)config->max_count - (int)record->count;
    return (remaining > 0) ? remaining : 0;
}

int ShopRestriction_GetTimeUntilAvailable(unsigned int character_id, unsigned int item_id,
                                          time_t current_time) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    ItemRestrictionConfig* config = FindItemConfig(item_id);
    if (!config) {
        return 0; /* No restriction */
    }

    PlayerRestrictionData* player_data = FindPlayerData(character_id);
    if (!player_data) {
        return 0; /* Never purchased */
    }

    CharacCerashopRestrict* record = FindRestrictionRecord(player_data, item_id);
    if (!record) {
        return 0;
    }

    if (current_time >= record->next_date) {
        return 0; /* Available now */
    }

    return (int)(record->next_date - current_time);
}

/* ========================================================================
 * RESTRICTION RECORDING API IMPLEMENTATION
 * ======================================================================== */

int ShopRestriction_RecordPurchase(unsigned int character_id, unsigned int item_id,
                                   unsigned int quantity, time_t current_time) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    ItemRestrictionConfig* config = FindItemConfig(item_id);
    if (!config) {
        return 0; /* No restriction to record */
    }

    PlayerRestrictionData* player_data = GetOrCreatePlayerData(character_id);
    if (!player_data) {
        return -2;
    }

    CharacCerashopRestrict* record = FindRestrictionRecord(player_data, item_id);
    if (!record) {
        record = AddRestrictionRecord(player_data, item_id);
        if (!record) {
            return -3; /* Full */
        }

        /* Initialize */
        record->end_date = (unsigned int)ShopRestriction_CalculatePeriodEnd(
            config->period, current_time);
    }

    /* Update count */
    record->count += quantity;

    /* Update timestamps */
    record->last_access_date = (unsigned int)current_time;

    /* Set next available time (cooldown) */
    if (config->cooldown_seconds > 0) {
        record->next_date = (unsigned int)(current_time + config->cooldown_seconds);
    } else {
        record->next_date = (unsigned int)current_time;
    }

    /* Update statistics */
    g_manager_state.global_stats.total_restrictions++;

    switch (config->period) {
        case RESTRICTION_PERIOD_DAILY:
            g_manager_state.global_stats.daily_restrictions++;
            break;
        case RESTRICTION_PERIOD_WEEKLY:
            g_manager_state.global_stats.weekly_restrictions++;
            break;
        case RESTRICTION_PERIOD_MONTHLY:
            g_manager_state.global_stats.monthly_restrictions++;
            break;
        case RESTRICTION_PERIOD_PERMANENT:
            g_manager_state.global_stats.permanent_restrictions++;
            break;
        default:
            break;
    }

    /* Sync to charac_cerashop_restrict database */
    if (g_restriction_db_manager && record) {
        CharacCerashopRestrict db_record;
        db_record.charac_no = character_id;
        db_record.ipg_no = item_id;
        db_record.count = record->count;
        db_record.next_date = record->next_date;
        db_record.end_date = record->end_date;
        db_record.last_access_date = record->last_access_date;
        CharacCerashopRestrict_Add(g_restriction_db_manager, &db_record);
    }

    return 0;
}

int ShopRestriction_GetRecord(unsigned int character_id, unsigned int item_id,
                              CharacCerashopRestrict* out_record) {
    if (!g_manager_state.is_initialized || !out_record) {
        return -1;
    }

    PlayerRestrictionData* player_data = FindPlayerData(character_id);
    if (!player_data) {
        return -2;
    }

    CharacCerashopRestrict* record = FindRestrictionRecord(player_data, item_id);
    if (!record) {
        return -3;
    }

    memcpy(out_record, record, sizeof(CharacCerashopRestrict));
    return 0;
}

/* ========================================================================
 * RESTRICTION RESET API IMPLEMENTATION
 * ======================================================================== */

int ShopRestriction_ResetDaily(void) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    int reset_count = 0;

    for (int p = 0; p < g_manager_state.max_players; p++) {
        PlayerRestrictionData* player = &g_manager_state.player_data[p];
        if (player->character_id == 0) continue;

        for (int r = 0; r < player->restriction_count; r++) {
            CharacCerashopRestrict* record = &player->restrictions[r];
            ItemRestrictionConfig* config = FindItemConfig(record->ipg_no);

            if (config && config->period == RESTRICTION_PERIOD_DAILY) {
                record->count = 0;
                record->end_date = (unsigned int)ShopRestriction_CalculatePeriodEnd(
                    RESTRICTION_PERIOD_DAILY, time(NULL));
                reset_count++;
            }
        }
    }

    return reset_count;
}

int ShopRestriction_ResetWeekly(void) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    int reset_count = 0;

    for (int p = 0; p < g_manager_state.max_players; p++) {
        PlayerRestrictionData* player = &g_manager_state.player_data[p];
        if (player->character_id == 0) continue;

        for (int r = 0; r < player->restriction_count; r++) {
            CharacCerashopRestrict* record = &player->restrictions[r];
            ItemRestrictionConfig* config = FindItemConfig(record->ipg_no);

            if (config && config->period == RESTRICTION_PERIOD_WEEKLY) {
                record->count = 0;
                record->end_date = (unsigned int)ShopRestriction_CalculatePeriodEnd(
                    RESTRICTION_PERIOD_WEEKLY, time(NULL));
                reset_count++;
            }
        }
    }

    return reset_count;
}

int ShopRestriction_ResetMonthly(void) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    int reset_count = 0;

    for (int p = 0; p < g_manager_state.max_players; p++) {
        PlayerRestrictionData* player = &g_manager_state.player_data[p];
        if (player->character_id == 0) continue;

        for (int r = 0; r < player->restriction_count; r++) {
            CharacCerashopRestrict* record = &player->restrictions[r];
            ItemRestrictionConfig* config = FindItemConfig(record->ipg_no);

            if (config && config->period == RESTRICTION_PERIOD_MONTHLY) {
                record->count = 0;
                record->end_date = (unsigned int)ShopRestriction_CalculatePeriodEnd(
                    RESTRICTION_PERIOD_MONTHLY, time(NULL));
                reset_count++;
            }
        }
    }

    return reset_count;
}

int ShopRestriction_ResetPlayer(unsigned int character_id) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    PlayerRestrictionData* player_data = FindPlayerData(character_id);
    if (!player_data) {
        return -2;
    }

    player_data->restriction_count = 0;
    memset(player_data->restrictions, 0, sizeof(player_data->restrictions));

    return 0;
}

int ShopRestriction_ResetPlayerItem(unsigned int character_id, unsigned int item_id) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    PlayerRestrictionData* player_data = FindPlayerData(character_id);
    if (!player_data) {
        return -2;
    }

    /* Find and remove restriction */
    for (int i = 0; i < player_data->restriction_count; i++) {
        if (player_data->restrictions[i].ipg_no == item_id) {
            /* Shift remaining restrictions */
            for (int j = i; j < player_data->restriction_count - 1; j++) {
                player_data->restrictions[j] = player_data->restrictions[j + 1];
            }
            player_data->restriction_count--;
            return 0;
        }
    }

    return -3; /* Not found */
}

/* ========================================================================
 * BATCH UPDATE API IMPLEMENTATION
 * ======================================================================== */

int ShopRestriction_UpdateAll(time_t current_time) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    int updated_count = 0;

    for (int p = 0; p < g_manager_state.max_players; p++) {
        PlayerRestrictionData* player = &g_manager_state.player_data[p];
        if (player->character_id == 0) continue;

        for (int r = 0; r < player->restriction_count; r++) {
            CharacCerashopRestrict* record = &player->restrictions[r];

            /* Check if period expired */
            if (ShopRestriction_IsPeriodExpired(record, current_time)) {
                ItemRestrictionConfig* config = FindItemConfig(record->ipg_no);
                if (config) {
                    record->count = 0;
                    record->end_date = (unsigned int)ShopRestriction_CalculatePeriodEnd(
                        config->period, current_time);
                    updated_count++;
                }
            }
        }
    }

    return updated_count;
}

/* ========================================================================
 * QUERY API IMPLEMENTATION
 * ======================================================================== */

int ShopRestriction_GetPlayerRestrictions(unsigned int character_id,
                                          CharacCerashopRestrict* out_records,
                                          int max_records) {
    if (!g_manager_state.is_initialized || !out_records) {
        return -1;
    }

    PlayerRestrictionData* player_data = FindPlayerData(character_id);
    if (!player_data) {
        return 0;
    }

    int count = (player_data->restriction_count < max_records) ?
                player_data->restriction_count : max_records;

    memcpy(out_records, player_data->restrictions, count * sizeof(CharacCerashopRestrict));

    return count;
}

int ShopRestriction_GetItemRestrictions(unsigned int item_id,
                                        CharacCerashopRestrict* out_records,
                                        int max_records) {
    if (!g_manager_state.is_initialized || !out_records) {
        return -1;
    }

    int count = 0;

    for (int p = 0; p < g_manager_state.max_players && count < max_records; p++) {
        PlayerRestrictionData* player = &g_manager_state.player_data[p];
        if (player->character_id == 0) continue;

        CharacCerashopRestrict* record = FindRestrictionRecord(player, item_id);
        if (record) {
            memcpy(&out_records[count], record, sizeof(CharacCerashopRestrict));
            count++;
        }
    }

    return count;
}

/* ========================================================================
 * STATISTICS API IMPLEMENTATION
 * ======================================================================== */

void ShopRestriction_GetStatistics(RestrictionStatistics* out_stats) {
    if (!g_manager_state.is_initialized || !out_stats) {
        return;
    }

    memcpy(out_stats, &g_manager_state.global_stats, sizeof(RestrictionStatistics));
}

unsigned int ShopRestriction_GetMostRestrictedItem(void) {
    /*
     * Most restricted item tracking:
     * This would require a denial counter per item_id in RestrictionStatistics.
     * Current implementation returns 0 (no tracking).
     *
     * To implement: Add denial_counts[MAX_ITEMS] array and increment
     * in ShopRestriction_CheckPurchaseAllowed when returning denial.
     */
    return 0;
}

/* ========================================================================
 * DATABASE INTEGRATION API IMPLEMENTATION
 * ======================================================================== */

int ShopRestriction_LoadFromDB(unsigned int character_id, unsigned int item_id,
                               CharacCerashopRestrict* out_record) {
    if (!out_record) {
        return -1;
    }

    if (g_restriction_db_manager) {
        return CharacCerashopRestrict_Get(g_restriction_db_manager, character_id, item_id, out_record);
    }

    return -1;
}

int ShopRestriction_SaveToDB(const CharacCerashopRestrict* record) {
    if (!record) {
        return -1;
    }

    if (g_restriction_db_manager) {
        return CharacCerashopRestrict_Add(g_restriction_db_manager, record);
    }

    return -1;
}

int ShopRestriction_DeleteFromDB(unsigned int character_id, unsigned int item_id) {
    if (g_restriction_db_manager) {
        return CharacCerashopRestrict_Delete(g_restriction_db_manager, character_id, item_id);
    }

    return -1;
}

/* ========================================================================
 * UTILITY API IMPLEMENTATION
 * ======================================================================== */

const char* ShopRestriction_GetPeriodName(RestrictionPeriod period) {
    switch (period) {
        case RESTRICTION_PERIOD_NONE: return "None";
        case RESTRICTION_PERIOD_DAILY: return "Daily";
        case RESTRICTION_PERIOD_WEEKLY: return "Weekly";
        case RESTRICTION_PERIOD_MONTHLY: return "Monthly";
        case RESTRICTION_PERIOD_PERMANENT: return "Permanent";
        case RESTRICTION_PERIOD_CUSTOM: return "Custom";
        default: return "Unknown";
    }
}

const char* ShopRestriction_GetTypeName(RestrictionType type) {
    switch (type) {
        case RESTRICTION_TYPE_COUNT: return "Count";
        case RESTRICTION_TYPE_TIME: return "Time";
        case RESTRICTION_TYPE_BOTH: return "Both";
        default: return "Unknown";
    }
}

time_t ShopRestriction_CalculatePeriodEnd(RestrictionPeriod period, time_t start_time) {
    switch (period) {
        case RESTRICTION_PERIOD_DAILY:
            return start_time + 86400;
        case RESTRICTION_PERIOD_WEEKLY:
            return start_time + 604800;
        case RESTRICTION_PERIOD_MONTHLY:
            return start_time + 2592000; /* 30 days */
        case RESTRICTION_PERIOD_PERMANENT:
            return start_time + 315360000; /* ~10 years */
        default:
            return start_time;
    }
}

int ShopRestriction_IsPeriodExpired(const CharacCerashopRestrict* record, time_t current_time) {
    if (!record) return 1;

    return (current_time >= record->end_date) ? 1 : 0;
}
