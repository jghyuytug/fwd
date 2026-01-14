/**
 * @file game_config.c
 * @brief Game Configuration Module - Implementation
 *
 * Phase: 6.7 - Configuration Abstraction (Stage 3)
 * Date: 2025-11-25
 *
 * Implementation Strategy:
 * - Stage 1: Return hardcoded default values (fallback)
 * - Stage 2: Hash+LRU cache for skill config (implemented)
 * - Stage 3: Load from Script.pvf (current implementation)
 *
 * All hardcoded values are centralized here for easy modification
 */

#include "../../include/config/game_config.h"
#include "../../include/config/etc_parser.h"
#include "../../include/pvf/pvf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ========================================================================
 * GLOBAL CONFIGURATION DATA
 * ======================================================================== */

static int g_initialized = 0;
static ConfigSource g_config_source = CONFIG_SOURCE_HARDCODED;
static PackSetInternal g_pvf_pack;
static int g_pvf_loaded = 0;

/**
 * Master configuration structure
 * All hardcoded values centralized here
 */
static GameConfig g_game_config = {
    /* ====================================================================
     * COMBAT SYSTEM CONFIGURATION
     * ==================================================================== */

    /* Damage formula configuration */
    .damage_formula = {
        .defense_constant = 1000,      /* Original: defense/(defense+1000) */
        .min_damage_rate = 0.01f,      /* Minimum 1% damage */
        .attribute_divisor = 200       /* Original: (str+int)/200 */
    },

    /* Character growth by job class */
    .character_growth = {
        /* Job 0 - Default/Fighter */
        [0] = {
            .base_strength = 50,
            .base_intelligence = 50,
            .base_vitality = 50,
            .base_spirit = 50,
            .strength_per_level = 2,
            .intelligence_per_level = 2,
            .vitality_per_level = 2,
            .spirit_per_level = 2,
            .base_critical_rate = 5,           /* 5% */
            .critical_rate_per_level = 0.1f,   /* +0.1% per level */
            .base_critical_damage = 50,        /* +50% crit damage */
            .base_accuracy = 95,               /* 95% accuracy */
            .base_evasion = 5                  /* 5% evasion */
        },
        /* Job 1-9: Copy defaults (can be customized later) */
        [1 ... 9] = {
            .base_strength = 50,
            .base_intelligence = 50,
            .base_vitality = 50,
            .base_spirit = 50,
            .strength_per_level = 2,
            .intelligence_per_level = 2,
            .vitality_per_level = 2,
            .spirit_per_level = 2,
            .base_critical_rate = 5,
            .critical_rate_per_level = 0.1f,
            .base_critical_damage = 50,
            .base_accuracy = 95,
            .base_evasion = 5
        }
    },

    /* Buff rules configuration */
    .buff_rules = {
        .buff_id = 0,
        .exclusion_group = 0,
        .max_stack_count = 10,             /* Original: max 10 stacks */
        .can_stack = 1,
        .default_duration_ms = 30000       /* Default 30 seconds */
    },

    /* ====================================================================
     * QUEST SYSTEM CONFIGURATION
     * ==================================================================== */

    /* Quest reward configuration */
    .quest_reward = {
        .exp_multiplier = 1.5f,            /* Original: 1.5x EXP */
        .gold_multiplier = 1.2f            /* Original: 1.2x Gold */
    },

    /* Quest timeout configuration */
    .quest_timeout = {
        .default_timeout_seconds = 3600,   /* Original: 1 hour */
        .max_timeout_seconds = 7200        /* Max 2 hours */
    },

    /* Quest limits configuration */
    .quest_limits = {
        .max_active_quests_per_character = 20,  /* Original: 20 quests */
        .max_quest_objectives = 10              /* Max 10 objectives per quest */
    },

    /* ====================================================================
     * SHOP SYSTEM CONFIGURATION
     * ==================================================================== */

    /* Shop discount configuration */
    .shop_discount = {
        .default_discount = 1.0f,          /* No discount */
        .vip_discount = 0.9f,              /* Original: VIP 10% off */
        .event_discount = 0.8f,            /* Original: Event 20% off */
        .sell_price_ratio = 0.5f           /* Original: 50% sell price */
    },

    /* Shop refresh configuration */
    .shop_refresh = {
        .daily_refresh_hour = 0,           /* Original: Midnight refresh */
        .weekly_refresh_day = 1,           /* Original: Monday refresh */
        .secret_shop_duration_seconds = 7200  /* Original: 2 hours (shop_manager.h:57) */
    },

    /* Shop limits configuration */
    .shop_limits = {
        .max_purchase_per_day = 10,        /* Original: 10 per day */
        .max_purchase_per_week = 50        /* Original: 50 per week */
    },

    /* ====================================================================
     * PARTY SYSTEM CONFIGURATION
     * ==================================================================== */

    /* Party bonus configuration */
    .party_bonus = {
        .bonus_2_members = 1.1f,           /* Original: +10% for 2 members */
        .bonus_3_members = 1.2f,           /* Original: +20% for 3 members */
        .bonus_4_members = 1.3f            /* Original: +30% for 4 members */
    },

    /* Party level gap configuration */
    .party_level_gap = {
        .max_level_gap = 10,               /* Max 10 level difference */
        .penalty_per_level = 0.05f         /* Original: -5% per level gap */
    }
};

/* ========================================================================
 * SKILL CONFIGURATION CACHE (Stage 2: Hash Table + LRU)
 * ======================================================================== */

#define SKILL_CONFIG_CACHE_SIZE 100
#define SKILL_HASH_TABLE_SIZE 256  /* Power of 2 for fast modulo */

/* Hash table node with LRU链表 */
typedef struct SkillConfigNode {
    int skill_id;
    int level;
    SkillConfig config;

    /* Hash collision chain */
    struct SkillConfigNode* hash_next;

    /* LRU doubly-linked list */
    struct SkillConfigNode* lru_prev;
    struct SkillConfigNode* lru_next;
} SkillConfigNode;

/* Skill configuration cache with Hash+LRU */
typedef struct {
    SkillConfigNode* hash_table[SKILL_HASH_TABLE_SIZE];
    SkillConfigNode* lru_head;  /* Most recently used */
    SkillConfigNode* lru_tail;  /* Least recently used */
    SkillConfigNode nodes[SKILL_CONFIG_CACHE_SIZE];
    int node_count;

    /* Statistics */
    int hit_count;
    int miss_count;
} SkillConfigCache;

static SkillConfigCache g_skill_cache = {0};

/**
 * Hash function for skill configuration
 * Uses simple multiplicative hash with prime numbers
 */
static unsigned int SkillConfigHash(int skill_id, int level) {
    unsigned int hash = (unsigned int)skill_id;
    hash = hash * 31 + (unsigned int)level;
    return hash & (SKILL_HASH_TABLE_SIZE - 1);  /* Fast modulo for power of 2 */
}

/**
 * Move node to LRU head (most recently used)
 */
static void MoveToLRUHead(SkillConfigNode* node) {
    if (!node || node == g_skill_cache.lru_head) {
        return;  /* Already at head */
    }

    /* Remove from current position */
    if (node->lru_prev) {
        node->lru_prev->lru_next = node->lru_next;
    }
    if (node->lru_next) {
        node->lru_next->lru_prev = node->lru_prev;
    }
    if (node == g_skill_cache.lru_tail) {
        g_skill_cache.lru_tail = node->lru_prev;
    }

    /* Insert at head */
    node->lru_prev = NULL;
    node->lru_next = g_skill_cache.lru_head;
    if (g_skill_cache.lru_head) {
        g_skill_cache.lru_head->lru_prev = node;
    }
    g_skill_cache.lru_head = node;

    if (!g_skill_cache.lru_tail) {
        g_skill_cache.lru_tail = node;
    }
}

/**
 * Remove node from Hash table
 */
static void RemoveFromHashTable(SkillConfigNode* node) {
    unsigned int hash = SkillConfigHash(node->skill_id, node->level);
    SkillConfigNode** slot = &g_skill_cache.hash_table[hash];

    while (*slot) {
        if (*slot == node) {
            *slot = node->hash_next;
            node->hash_next = NULL;
            return;
        }
        slot = &(*slot)->hash_next;
    }
}

/**
 * Evict LRU tail node
 */
static SkillConfigNode* EvictLRU(void) {
    SkillConfigNode* evicted = g_skill_cache.lru_tail;
    if (!evicted) {
        return NULL;
    }

    /* Remove from LRU list */
    if (evicted->lru_prev) {
        evicted->lru_prev->lru_next = NULL;
        g_skill_cache.lru_tail = evicted->lru_prev;
    } else {
        g_skill_cache.lru_head = NULL;
        g_skill_cache.lru_tail = NULL;
    }

    /* Remove from Hash table */
    RemoveFromHashTable(evicted);

    return evicted;
}

/**
 * Insert node into Hash table and LRU head
 */
static void InsertNode(SkillConfigNode* node) {
    /* Insert into Hash table */
    unsigned int hash = SkillConfigHash(node->skill_id, node->level);
    node->hash_next = g_skill_cache.hash_table[hash];
    g_skill_cache.hash_table[hash] = node;

    /* Insert at LRU head */
    node->lru_prev = NULL;
    node->lru_next = g_skill_cache.lru_head;
    if (g_skill_cache.lru_head) {
        g_skill_cache.lru_head->lru_prev = node;
    }
    g_skill_cache.lru_head = node;

    if (!g_skill_cache.lru_tail) {
        g_skill_cache.lru_tail = node;
    }
}

/**
 * Calculate skill configuration based on hardcoded formulas
 * (Stage 1 implementation)
 *
 * Original formulas from combat_data_loader.c:172-189
 */
static void CalculateSkillConfig(int skill_id, int level, SkillConfig* out_config) {
    if (!out_config) return;

    out_config->skill_id = skill_id;
    out_config->level = level;

    /* Original formula: base_damage = (level * 100) + 500 */
    out_config->base_damage = (level * 100) + 500;

    /* Original formula: damage_multiplier = 100 + (level * 5) */
    out_config->damage_multiplier = 100 + (level * 5);

    /* Original formula: cooldown = 10000 - (level * 100), min 1000 */
    out_config->cooldown_ms = 10000 - (level * 100);
    if (out_config->cooldown_ms < 1000) {
        out_config->cooldown_ms = 1000;
    }

    /* Original formula: mp_cost = 30 + (level * 2) */
    out_config->mp_cost = 30 + (level * 2);

    /* Default cast time */
    out_config->cast_time_ms = 500;  /* 0.5 seconds */
}

/* ========================================================================
 * PUBLIC API IMPLEMENTATION
 * ======================================================================== */

/**
 * Initialize game configuration module
 */
int GameConfig_Initialize(void) {
    if (g_initialized) {
        printf("[GameConfig] Already initialized\n");
        return 0;
    }

    printf("[GameConfig] Initializing configuration module (Stage 2: Hash+LRU cache)...\n");

    /* Initialize skill config cache with Hash+LRU */
    memset(&g_skill_cache, 0, sizeof(g_skill_cache));
    g_skill_cache.node_count = 0;
    g_skill_cache.hit_count = 0;
    g_skill_cache.miss_count = 0;
    g_skill_cache.lru_head = NULL;
    g_skill_cache.lru_tail = NULL;
    for (int i = 0; i < SKILL_HASH_TABLE_SIZE; i++) {
        g_skill_cache.hash_table[i] = NULL;
    }

    g_initialized = 1;

    printf("[GameConfig] Configuration initialized successfully\n");
    printf("[GameConfig] Combat: Damage formula (def_const=%d, attr_div=%d)\n",
           g_game_config.damage_formula.defense_constant,
           g_game_config.damage_formula.attribute_divisor);
    printf("[GameConfig] Quest: Reward multipliers (exp=%.1fx, gold=%.1fx)\n",
           g_game_config.quest_reward.exp_multiplier,
           g_game_config.quest_reward.gold_multiplier);
    printf("[GameConfig] Shop: Discounts (vip=%.0f%%, event=%.0f%%)\n",
           (1.0f - g_game_config.shop_discount.vip_discount) * 100,
           (1.0f - g_game_config.shop_discount.event_discount) * 100);
    printf("[GameConfig] Party: Bonuses (2=+%.0f%%, 3=+%.0f%%, 4=+%.0f%%)\n",
           (g_game_config.party_bonus.bonus_2_members - 1.0f) * 100,
           (g_game_config.party_bonus.bonus_3_members - 1.0f) * 100,
           (g_game_config.party_bonus.bonus_4_members - 1.0f) * 100);

    return 0;
}

/**
 * Cleanup game configuration module
 */
void GameConfig_Cleanup(void) {
    if (!g_initialized) {
        return;
    }

    printf("[GameConfig] Cleaning up configuration module...\n");

    /* Print final cache statistics */
    GameConfig_PrintCacheStats();

    /* Clear skill config cache (Stage 2: Hash+LRU) */
    memset(&g_skill_cache, 0, sizeof(g_skill_cache));

    /* Close PVF if loaded (Stage 3) */
    if (g_pvf_loaded) {
        PackSet_Close(&g_pvf_pack);
        g_pvf_loaded = 0;
        printf("[GameConfig] PVF closed\n");
    }

    g_config_source = CONFIG_SOURCE_HARDCODED;
    g_initialized = 0;

    printf("[GameConfig] Configuration cleanup complete\n");
}

/**
 * Get master configuration structure
 */
const GameConfig* GameConfig_Get(void) {
    return &g_game_config;
}

/* ========================================================================
 * COMBAT CONFIGURATION API
 * ======================================================================== */

/**
 * Get damage formula configuration
 */
const DamageFormulaConfig* CombatConfig_GetDamageFormula(void) {
    return &g_game_config.damage_formula;
}

/**
 * Get character growth configuration by job class
 */
const CharacterGrowthConfig* CombatConfig_GetCharacterGrowth(int job_class) {
    if (job_class < 0 || job_class >= 10) {
        /* Return default (job 0) for invalid job class */
        return &g_game_config.character_growth[0];
    }
    return &g_game_config.character_growth[job_class];
}

/**
 * Get skill configuration (Stage 2: Hash+LRU implementation)
 */
const SkillConfig* CombatConfig_GetSkill(int skill_id, int level) {
    /* Clamp level to valid range */
    if (level < 1) level = 1;
    if (level > 60) level = 60;

    /* Hash lookup - O(1) average case */
    unsigned int hash = SkillConfigHash(skill_id, level);
    SkillConfigNode* node = g_skill_cache.hash_table[hash];

    /* Search in hash bucket */
    while (node) {
        if (node->skill_id == skill_id && node->level == level) {
            /* Cache HIT */
            g_skill_cache.hit_count++;
            MoveToLRUHead(node);  /* Update LRU */
            return &node->config;
        }
        node = node->hash_next;
    }

    /* Cache MISS */
    g_skill_cache.miss_count++;

    /* Allocate new node or evict LRU */
    SkillConfigNode* new_node;
    if (g_skill_cache.node_count < SKILL_CONFIG_CACHE_SIZE) {
        /* Use new node from pool */
        new_node = &g_skill_cache.nodes[g_skill_cache.node_count];
        g_skill_cache.node_count++;
    } else {
        /* Evict LRU node */
        new_node = EvictLRU();
        if (!new_node) {
            /* Should never happen, but fallback */
            new_node = &g_skill_cache.nodes[0];
        }
    }

    /* Calculate configuration */
    new_node->skill_id = skill_id;
    new_node->level = level;
    new_node->hash_next = NULL;
    new_node->lru_prev = NULL;
    new_node->lru_next = NULL;
    CalculateSkillConfig(skill_id, level, &new_node->config);

    /* Insert into cache */
    InsertNode(new_node);

    return &new_node->config;
}

/**
 * Get buff rules configuration
 */
const BuffConfig* CombatConfig_GetBuffRules(void) {
    return &g_game_config.buff_rules;
}

/* ========================================================================
 * QUEST CONFIGURATION API
 * ======================================================================== */

/**
 * Get quest reward configuration
 * NOTE: Renamed to avoid conflict with quest/quest_config.h
 */
const QuestRewardConfig* GameConfig_GetQuestReward(void) {
    return &g_game_config.quest_reward;
}

/**
 * Get quest timeout configuration
 * NOTE: Renamed to avoid conflict with quest/quest_config.h
 */
const QuestTimeoutConfig* GameConfig_GetQuestTimeout(void) {
    return &g_game_config.quest_timeout;
}

/**
 * Get quest limits configuration
 * NOTE: Renamed to avoid conflict with quest/quest_config.h
 */
const QuestLimitsConfig* GameConfig_GetQuestLimits(void) {
    return &g_game_config.quest_limits;
}

/* ========================================================================
 * SHOP CONFIGURATION API
 * ======================================================================== */

/**
 * Get shop discount configuration
 */
const ShopDiscountConfig* ShopConfig_GetDiscount(void) {
    return &g_game_config.shop_discount;
}

/**
 * Get shop refresh configuration
 */
const ShopRefreshConfig* ShopConfig_GetRefresh(void) {
    return &g_game_config.shop_refresh;
}

/**
 * Get shop limits configuration
 */
const ShopLimitsConfig* ShopConfig_GetLimits(void) {
    return &g_game_config.shop_limits;
}

/* ========================================================================
 * PARTY CONFIGURATION API
 * ======================================================================== */

/**
 * Get party bonus configuration
 */
const PartyBonusConfig* PartyConfig_GetBonus(void) {
    return &g_game_config.party_bonus;
}

/**
 * Get party level gap configuration
 */
const PartyLevelGapConfig* PartyConfig_GetLevelGap(void) {
    return &g_game_config.party_level_gap;
}

/* ========================================================================
 * CACHE STATISTICS (Stage 2)
 * ======================================================================== */

/**
 * Print skill configuration cache statistics
 */
void GameConfig_PrintCacheStats(void) {
    int total_requests = g_skill_cache.hit_count + g_skill_cache.miss_count;
    float hit_rate = total_requests > 0 ?
        (float)g_skill_cache.hit_count / total_requests * 100.0f : 0.0f;

    printf("\n[GameConfig] ===== Skill Configuration Cache Statistics =====\n");
    printf("  Cache Implementation: Hash Table + LRU\n");
    printf("  Hash Table Size: %d buckets\n", SKILL_HASH_TABLE_SIZE);
    printf("  Cache Capacity: %d entries\n", SKILL_CONFIG_CACHE_SIZE);
    printf("  Current Size: %d entries\n", g_skill_cache.node_count);
    printf("\n");
    printf("  Total Requests: %d\n", total_requests);
    printf("  Cache Hits: %d\n", g_skill_cache.hit_count);
    printf("  Cache Misses: %d\n", g_skill_cache.miss_count);
    printf("  Hit Rate: %.1f%%\n", hit_rate);
    printf("\n");

    /* Calculate average bucket depth */
    int non_empty_buckets = 0;
    int max_depth = 0;
    for (int i = 0; i < SKILL_HASH_TABLE_SIZE; i++) {
        if (g_skill_cache.hash_table[i]) {
            non_empty_buckets++;
            int depth = 0;
            SkillConfigNode* node = g_skill_cache.hash_table[i];
            while (node) {
                depth++;
                node = node->hash_next;
            }
            if (depth > max_depth) {
                max_depth = depth;
            }
        }
    }

    float load_factor = g_skill_cache.node_count > 0 ?
        (float)g_skill_cache.node_count / SKILL_HASH_TABLE_SIZE : 0.0f;

    printf("  Hash Table Stats:\n");
    printf("    Load Factor: %.3f\n", load_factor);
    printf("    Non-Empty Buckets: %d / %d (%.1f%%)\n",
           non_empty_buckets, SKILL_HASH_TABLE_SIZE,
           (float)non_empty_buckets / SKILL_HASH_TABLE_SIZE * 100.0f);
    printf("    Max Bucket Depth: %d\n", max_depth);
    printf("  ======================================================\n\n");
}

/* ========================================================================
 * PVF CONFIGURATION LOADING (Stage 3)
 * ======================================================================== */

/**
 * PVF configuration file paths
 */
static const char* PVF_CONFIG_COMBAT = "etc/combat.etc";
static const char* PVF_CONFIG_QUEST = "etc/quest.etc";
static const char* PVF_CONFIG_SHOP = "etc/shop.etc";
static const char* PVF_CONFIG_PARTY = "etc/party.etc";

/**
 * Load combat configuration from PVF
 */
static int LoadCombatConfigFromPVF(void) {
    if (!g_pvf_loaded) return -1;

    PVFIndexEntry* entry = PackSet_GetFile(&g_pvf_pack, PVF_CONFIG_COMBAT);
    if (!entry) {
        printf("[GameConfig] Combat config not found in PVF: %s\n", PVF_CONFIG_COMBAT);
        return -1;
    }

    char* buffer = (char*)malloc(entry->size + 1);
    if (!buffer) return -1;

    int result = PackSet_ExtractFile(&g_pvf_pack, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        free(buffer);
        return -1;
    }
    buffer[entry->size] = '\0';

    ETCDocument doc;
    if (ETC_Parse(&doc, buffer, entry->size) == ETC_SUCCESS) {
        /* Load damage formula */
        g_game_config.damage_formula.defense_constant =
            ETC_GetInt(&doc, "defense_constant", 1000);
        g_game_config.damage_formula.min_damage_rate =
            ETC_GetFloat(&doc, "min_damage_rate", 0.01f);
        g_game_config.damage_formula.attribute_divisor =
            ETC_GetInt(&doc, "attribute_divisor", 200);

        /* Load buff rules */
        g_game_config.buff_rules.max_stack_count =
            ETC_GetInt(&doc, "max_buff_stack", 10);
        g_game_config.buff_rules.default_duration_ms =
            ETC_GetInt(&doc, "default_buff_duration", 30000);

        printf("[GameConfig] Loaded combat config from PVF (%d entries)\n",
               ETC_GetEntryCount(&doc));
        ETC_Free(&doc);
    }

    free(buffer);
    return 0;
}

/**
 * Load quest configuration from PVF
 */
static int LoadQuestConfigFromPVF(void) {
    if (!g_pvf_loaded) return -1;

    PVFIndexEntry* entry = PackSet_GetFile(&g_pvf_pack, PVF_CONFIG_QUEST);
    if (!entry) {
        printf("[GameConfig] Quest config not found in PVF: %s\n", PVF_CONFIG_QUEST);
        return -1;
    }

    char* buffer = (char*)malloc(entry->size + 1);
    if (!buffer) return -1;

    int result = PackSet_ExtractFile(&g_pvf_pack, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        free(buffer);
        return -1;
    }
    buffer[entry->size] = '\0';

    ETCDocument doc;
    if (ETC_Parse(&doc, buffer, entry->size) == ETC_SUCCESS) {
        g_game_config.quest_reward.exp_multiplier =
            ETC_GetFloat(&doc, "exp_multiplier", 1.5f);
        g_game_config.quest_reward.gold_multiplier =
            ETC_GetFloat(&doc, "gold_multiplier", 1.2f);
        g_game_config.quest_timeout.default_timeout_seconds =
            ETC_GetInt(&doc, "default_timeout", 3600);
        g_game_config.quest_timeout.max_timeout_seconds =
            ETC_GetInt(&doc, "max_timeout", 7200);
        g_game_config.quest_limits.max_active_quests_per_character =
            ETC_GetInt(&doc, "max_active_quests", 20);
        g_game_config.quest_limits.max_quest_objectives =
            ETC_GetInt(&doc, "max_objectives", 10);

        printf("[GameConfig] Loaded quest config from PVF (%d entries)\n",
               ETC_GetEntryCount(&doc));
        ETC_Free(&doc);
    }

    free(buffer);
    return 0;
}

/**
 * Load shop configuration from PVF
 */
static int LoadShopConfigFromPVF(void) {
    if (!g_pvf_loaded) return -1;

    PVFIndexEntry* entry = PackSet_GetFile(&g_pvf_pack, PVF_CONFIG_SHOP);
    if (!entry) {
        printf("[GameConfig] Shop config not found in PVF: %s\n", PVF_CONFIG_SHOP);
        return -1;
    }

    char* buffer = (char*)malloc(entry->size + 1);
    if (!buffer) return -1;

    int result = PackSet_ExtractFile(&g_pvf_pack, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        free(buffer);
        return -1;
    }
    buffer[entry->size] = '\0';

    ETCDocument doc;
    if (ETC_Parse(&doc, buffer, entry->size) == ETC_SUCCESS) {
        g_game_config.shop_discount.default_discount =
            ETC_GetFloat(&doc, "default_discount", 1.0f);
        g_game_config.shop_discount.vip_discount =
            ETC_GetFloat(&doc, "vip_discount", 0.9f);
        g_game_config.shop_discount.event_discount =
            ETC_GetFloat(&doc, "event_discount", 0.8f);
        g_game_config.shop_discount.sell_price_ratio =
            ETC_GetFloat(&doc, "sell_price_ratio", 0.5f);
        g_game_config.shop_refresh.daily_refresh_hour =
            ETC_GetInt(&doc, "daily_refresh_hour", 0);
        g_game_config.shop_refresh.weekly_refresh_day =
            ETC_GetInt(&doc, "weekly_refresh_day", 1);
        g_game_config.shop_refresh.secret_shop_duration_seconds =
            ETC_GetInt(&doc, "secret_shop_duration", 7200);
        g_game_config.shop_limits.max_purchase_per_day =
            ETC_GetInt(&doc, "max_purchase_per_day", 10);
        g_game_config.shop_limits.max_purchase_per_week =
            ETC_GetInt(&doc, "max_purchase_per_week", 50);

        printf("[GameConfig] Loaded shop config from PVF (%d entries)\n",
               ETC_GetEntryCount(&doc));
        ETC_Free(&doc);
    }

    free(buffer);
    return 0;
}

/**
 * Load party configuration from PVF
 */
static int LoadPartyConfigFromPVF(void) {
    if (!g_pvf_loaded) return -1;

    PVFIndexEntry* entry = PackSet_GetFile(&g_pvf_pack, PVF_CONFIG_PARTY);
    if (!entry) {
        printf("[GameConfig] Party config not found in PVF: %s\n", PVF_CONFIG_PARTY);
        return -1;
    }

    char* buffer = (char*)malloc(entry->size + 1);
    if (!buffer) return -1;

    int result = PackSet_ExtractFile(&g_pvf_pack, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        free(buffer);
        return -1;
    }
    buffer[entry->size] = '\0';

    ETCDocument doc;
    if (ETC_Parse(&doc, buffer, entry->size) == ETC_SUCCESS) {
        g_game_config.party_bonus.bonus_2_members =
            ETC_GetFloat(&doc, "bonus_2_members", 1.1f);
        g_game_config.party_bonus.bonus_3_members =
            ETC_GetFloat(&doc, "bonus_3_members", 1.2f);
        g_game_config.party_bonus.bonus_4_members =
            ETC_GetFloat(&doc, "bonus_4_members", 1.3f);
        g_game_config.party_level_gap.max_level_gap =
            ETC_GetInt(&doc, "max_level_gap", 10);
        g_game_config.party_level_gap.penalty_per_level =
            ETC_GetFloat(&doc, "penalty_per_level", 0.05f);

        printf("[GameConfig] Loaded party config from PVF (%d entries)\n",
               ETC_GetEntryCount(&doc));
        ETC_Free(&doc);
    }

    free(buffer);
    return 0;
}

/**
 * Initialize skill config cache
 */
static void InitSkillCache(void) {
    memset(&g_skill_cache, 0, sizeof(g_skill_cache));
    g_skill_cache.node_count = 0;
    g_skill_cache.hit_count = 0;
    g_skill_cache.miss_count = 0;
    g_skill_cache.lru_head = NULL;
    g_skill_cache.lru_tail = NULL;
    for (int i = 0; i < SKILL_HASH_TABLE_SIZE; i++) {
        g_skill_cache.hash_table[i] = NULL;
    }
}

/**
 * Get current configuration source
 */
ConfigSource GameConfig_GetSource(void) {
    return g_config_source;
}

/**
 * Initialize game configuration from PVF file
 */
int GameConfig_InitializeFromPVF(const char* pvf_path) {
    if (g_initialized) {
        printf("[GameConfig] Already initialized\n");
        return 0;
    }

    printf("[GameConfig] Initializing from PVF: %s\n", pvf_path);

    /* Initialize CRC32 table */
    PVF_InitCRC32Table();

    /* Try to open PVF */
    int result = PackSet_Open(pvf_path, &g_pvf_pack);
    if (result != PVF_SUCCESS) {
        printf("[GameConfig] Failed to open PVF (error %d), using hardcoded values\n", result);
        g_config_source = CONFIG_SOURCE_HARDCODED;
        g_pvf_loaded = 0;
    } else {
        printf("[GameConfig] PVF opened: %u files\n", g_pvf_pack.header.index_count);
        g_config_source = CONFIG_SOURCE_PVF;
        g_pvf_loaded = 1;

        /* Load configurations from PVF */
        LoadCombatConfigFromPVF();
        LoadQuestConfigFromPVF();
        LoadShopConfigFromPVF();
        LoadPartyConfigFromPVF();
    }

    /* Initialize skill config cache */
    InitSkillCache();

    g_initialized = 1;

    printf("[GameConfig] Configuration initialized (source: %s)\n",
           g_config_source == CONFIG_SOURCE_PVF ? "PVF" : "HARDCODED");
    printf("[GameConfig] Combat: Damage formula (def_const=%d, attr_div=%d)\n",
           g_game_config.damage_formula.defense_constant,
           g_game_config.damage_formula.attribute_divisor);
    printf("[GameConfig] Quest: Reward multipliers (exp=%.1fx, gold=%.1fx)\n",
           g_game_config.quest_reward.exp_multiplier,
           g_game_config.quest_reward.gold_multiplier);
    printf("[GameConfig] Shop: Discounts (vip=%.0f%%, event=%.0f%%)\n",
           (1.0f - g_game_config.shop_discount.vip_discount) * 100,
           (1.0f - g_game_config.shop_discount.event_discount) * 100);
    printf("[GameConfig] Party: Bonuses (2=+%.0f%%, 3=+%.0f%%, 4=+%.0f%%)\n",
           (g_game_config.party_bonus.bonus_2_members - 1.0f) * 100,
           (g_game_config.party_bonus.bonus_3_members - 1.0f) * 100,
           (g_game_config.party_bonus.bonus_4_members - 1.0f) * 100);

    return g_config_source == CONFIG_SOURCE_PVF ? 0 : -1;
}

/* ========================================================================
 * FILE CONFIGURATION LOADING (Stage 4)
 * ======================================================================== */

static char g_config_base_path[512] = "";

/**
 * Read file contents into buffer
 */
static char* ReadConfigFile(const char* filepath, size_t* out_size) {
    FILE* fp = fopen(filepath, "rb");
    if (!fp) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size <= 0) {
        fclose(fp);
        return NULL;
    }

    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }

    size_t read = fread(buffer, 1, size, fp);
    fclose(fp);

    buffer[read] = '\0';
    if (out_size) *out_size = read;

    return buffer;
}

/**
 * Load combat configuration from file
 */
static int LoadCombatConfigFromFile(const char* base_path) {
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/combat.etc", base_path);

    size_t size;
    char* buffer = ReadConfigFile(filepath, &size);
    if (!buffer) {
        printf("[GameConfig] Combat config file not found: %s\n", filepath);
        return -1;
    }

    ETCDocument doc;
    if (ETC_Parse(&doc, buffer, size) == ETC_SUCCESS) {
        g_game_config.damage_formula.defense_constant =
            ETC_GetInt(&doc, "defense_constant", 1000);
        g_game_config.damage_formula.min_damage_rate =
            ETC_GetFloat(&doc, "min_damage_rate", 0.01f);
        g_game_config.damage_formula.attribute_divisor =
            ETC_GetInt(&doc, "attribute_divisor", 200);
        g_game_config.buff_rules.max_stack_count =
            ETC_GetInt(&doc, "max_buff_stack", 10);
        g_game_config.buff_rules.default_duration_ms =
            ETC_GetInt(&doc, "default_buff_duration", 30000);

        printf("[GameConfig] Loaded combat config from file (%d entries)\n",
               ETC_GetEntryCount(&doc));
        ETC_Free(&doc);
    }

    free(buffer);
    return 0;
}

/**
 * Load quest configuration from file
 */
static int LoadQuestConfigFromFile(const char* base_path) {
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/quest.etc", base_path);

    size_t size;
    char* buffer = ReadConfigFile(filepath, &size);
    if (!buffer) {
        printf("[GameConfig] Quest config file not found: %s\n", filepath);
        return -1;
    }

    ETCDocument doc;
    if (ETC_Parse(&doc, buffer, size) == ETC_SUCCESS) {
        g_game_config.quest_reward.exp_multiplier =
            ETC_GetFloat(&doc, "exp_multiplier", 1.5f);
        g_game_config.quest_reward.gold_multiplier =
            ETC_GetFloat(&doc, "gold_multiplier", 1.2f);
        g_game_config.quest_timeout.default_timeout_seconds =
            ETC_GetInt(&doc, "default_timeout", 3600);
        g_game_config.quest_timeout.max_timeout_seconds =
            ETC_GetInt(&doc, "max_timeout", 7200);
        g_game_config.quest_limits.max_active_quests_per_character =
            ETC_GetInt(&doc, "max_active_quests", 20);
        g_game_config.quest_limits.max_quest_objectives =
            ETC_GetInt(&doc, "max_objectives", 10);

        printf("[GameConfig] Loaded quest config from file (%d entries)\n",
               ETC_GetEntryCount(&doc));
        ETC_Free(&doc);
    }

    free(buffer);
    return 0;
}

/**
 * Load shop configuration from file
 */
static int LoadShopConfigFromFile(const char* base_path) {
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/shop.etc", base_path);

    size_t size;
    char* buffer = ReadConfigFile(filepath, &size);
    if (!buffer) {
        printf("[GameConfig] Shop config file not found: %s\n", filepath);
        return -1;
    }

    ETCDocument doc;
    if (ETC_Parse(&doc, buffer, size) == ETC_SUCCESS) {
        g_game_config.shop_discount.default_discount =
            ETC_GetFloat(&doc, "default_discount", 1.0f);
        g_game_config.shop_discount.vip_discount =
            ETC_GetFloat(&doc, "vip_discount", 0.9f);
        g_game_config.shop_discount.event_discount =
            ETC_GetFloat(&doc, "event_discount", 0.8f);
        g_game_config.shop_discount.sell_price_ratio =
            ETC_GetFloat(&doc, "sell_price_ratio", 0.5f);
        g_game_config.shop_refresh.daily_refresh_hour =
            ETC_GetInt(&doc, "daily_refresh_hour", 0);
        g_game_config.shop_refresh.weekly_refresh_day =
            ETC_GetInt(&doc, "weekly_refresh_day", 1);
        g_game_config.shop_refresh.secret_shop_duration_seconds =
            ETC_GetInt(&doc, "secret_shop_duration", 7200);
        g_game_config.shop_limits.max_purchase_per_day =
            ETC_GetInt(&doc, "max_purchase_per_day", 10);
        g_game_config.shop_limits.max_purchase_per_week =
            ETC_GetInt(&doc, "max_purchase_per_week", 50);

        printf("[GameConfig] Loaded shop config from file (%d entries)\n",
               ETC_GetEntryCount(&doc));
        ETC_Free(&doc);
    }

    free(buffer);
    return 0;
}

/**
 * Load party configuration from file
 */
static int LoadPartyConfigFromFile(const char* base_path) {
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/party.etc", base_path);

    size_t size;
    char* buffer = ReadConfigFile(filepath, &size);
    if (!buffer) {
        printf("[GameConfig] Party config file not found: %s\n", filepath);
        return -1;
    }

    ETCDocument doc;
    if (ETC_Parse(&doc, buffer, size) == ETC_SUCCESS) {
        g_game_config.party_bonus.bonus_2_members =
            ETC_GetFloat(&doc, "bonus_2_members", 1.1f);
        g_game_config.party_bonus.bonus_3_members =
            ETC_GetFloat(&doc, "bonus_3_members", 1.2f);
        g_game_config.party_bonus.bonus_4_members =
            ETC_GetFloat(&doc, "bonus_4_members", 1.3f);
        g_game_config.party_level_gap.max_level_gap =
            ETC_GetInt(&doc, "max_level_gap", 10);
        g_game_config.party_level_gap.penalty_per_level =
            ETC_GetFloat(&doc, "penalty_per_level", 0.05f);

        printf("[GameConfig] Loaded party config from file (%d entries)\n",
               ETC_GetEntryCount(&doc));
        ETC_Free(&doc);
    }

    free(buffer);
    return 0;
}

/**
 * Initialize game configuration from config files
 *
 * Parameters:
 *   config_dir: Path to directory containing .etc config files
 *               (e.g., "config/etc" or "/home/dxf/game/config/etc")
 *
 * Expected files in config_dir:
 *   - combat.etc
 *   - quest.etc
 *   - shop.etc
 *   - party.etc
 */
int GameConfig_InitializeFromFile(const char* config_dir) {
    if (g_initialized) {
        printf("[GameConfig] Already initialized\n");
        return 0;
    }

    printf("[GameConfig] Initializing from files: %s\n", config_dir);

    /* Store base path */
    strncpy(g_config_base_path, config_dir, sizeof(g_config_base_path) - 1);
    g_config_base_path[sizeof(g_config_base_path) - 1] = '\0';

    /* Load configurations from files */
    int loaded = 0;
    if (LoadCombatConfigFromFile(config_dir) == 0) loaded++;
    if (LoadQuestConfigFromFile(config_dir) == 0) loaded++;
    if (LoadShopConfigFromFile(config_dir) == 0) loaded++;
    if (LoadPartyConfigFromFile(config_dir) == 0) loaded++;

    if (loaded > 0) {
        g_config_source = CONFIG_SOURCE_FILE;
        printf("[GameConfig] Loaded %d/4 config files\n", loaded);
    } else {
        g_config_source = CONFIG_SOURCE_HARDCODED;
        printf("[GameConfig] No config files loaded, using hardcoded values\n");
    }

    /* Initialize skill config cache */
    InitSkillCache();

    g_initialized = 1;

    printf("[GameConfig] Configuration initialized (source: %s)\n",
           g_config_source == CONFIG_SOURCE_FILE ? "FILE" : "HARDCODED");
    printf("[GameConfig] Combat: Damage formula (def_const=%d, attr_div=%d)\n",
           g_game_config.damage_formula.defense_constant,
           g_game_config.damage_formula.attribute_divisor);
    printf("[GameConfig] Quest: Reward multipliers (exp=%.1fx, gold=%.1fx)\n",
           g_game_config.quest_reward.exp_multiplier,
           g_game_config.quest_reward.gold_multiplier);
    printf("[GameConfig] Shop: Discounts (vip=%.0f%%, event=%.0f%%)\n",
           (1.0f - g_game_config.shop_discount.vip_discount) * 100,
           (1.0f - g_game_config.shop_discount.event_discount) * 100);
    printf("[GameConfig] Party: Bonuses (2=+%.0f%%, 3=+%.0f%%, 4=+%.0f%%)\n",
           (g_game_config.party_bonus.bonus_2_members - 1.0f) * 100,
           (g_game_config.party_bonus.bonus_3_members - 1.0f) * 100,
           (g_game_config.party_bonus.bonus_4_members - 1.0f) * 100);

    return g_config_source == CONFIG_SOURCE_FILE ? 0 : -1;
}

/**
 * Initialize game configuration with specified source
 */
int GameConfig_InitializeEx(ConfigSource source, const char* path) {
    switch (source) {
        case CONFIG_SOURCE_PVF:
            if (!path) {
                printf("[GameConfig] PVF path required\n");
                return -1;
            }
            return GameConfig_InitializeFromPVF(path);

        case CONFIG_SOURCE_FILE:
            if (!path) {
                printf("[GameConfig] Config directory path required\n");
                return -1;
            }
            return GameConfig_InitializeFromFile(path);

        case CONFIG_SOURCE_HARDCODED:
        default:
            return GameConfig_Initialize();
    }
}
