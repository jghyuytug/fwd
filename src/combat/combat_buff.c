/**
 * Combat Buff System - Implementation
 *
 * Phase: 6.1 - Combat System Implementation
 * Created: 2025-11-23
 */

#include "combat_buff.h"
#include <stdio.h>
#include <string.h>

/*
 * Buff storage: Fixed-size array indexed by character ID
 *
 * Current implementation: O(1) access by character ID, O(n) buff iteration
 * - Supports up to 1000 concurrent characters with buffs
 * - Each character can have MAX_BUFFS_PER_CHARACTER active buffs
 *
 * Production optimization options:
 * 1. Hash map: Better for sparse character IDs (use character_id % table_size)
 * 2. Dynamic allocation: Per-character buff list for memory efficiency
 * 3. LRU cache: For servers with >1000 concurrent buffed characters
 *
 * Current design is sufficient for typical game server loads.
 */
static ActiveBuff g_buffs[1000][MAX_BUFFS_PER_CHARACTER];
static int g_buff_counts[1000] = {0};

/**
 * Initialize buff system
 */
int CombatBuff_Initialize(void) {
    memset(g_buffs, 0, sizeof(g_buffs));
    memset(g_buff_counts, 0, sizeof(g_buff_counts));
    printf("[Combat] Buff system initialized\n");
    return 0;
}

/**
 * Check if buff can stack with existing buffs
 */
static bool CanBuffStack(int buff_id) {
    // Buffs below ID 1000 can stack
    // Buffs 1000+ are unique and don't stack
    return (buff_id < 1000);
}

/**
 * Check if two buffs are mutually exclusive
 */
static bool AreBuffsMutuallyExclusive(int buff_id1, int buff_id2) {
    // Define mutually exclusive buff groups
    // Group 1: Attack buffs (1-10)
    // Group 2: Defense buffs (11-20)
    // Group 3: Speed buffs (21-30)

    if (buff_id1 >= 1 && buff_id1 <= 10 && buff_id2 >= 1 && buff_id2 <= 10) {
        return true;  // Only one attack buff at a time
    }
    if (buff_id1 >= 11 && buff_id1 <= 20 && buff_id2 >= 11 && buff_id2 <= 20) {
        return true;  // Only one defense buff at a time
    }
    if (buff_id1 >= 21 && buff_id1 <= 30 && buff_id2 >= 21 && buff_id2 <= 30) {
        return true;  // Only one speed buff at a time
    }

    return false;
}

/**
 * Find existing buff by ID
 */
static int FindBuffIndex(int character_id, int buff_id) {
    int count = g_buff_counts[character_id];
    for (int i = 0; i < count; i++) {
        if (g_buffs[character_id][i].buff_id == buff_id) {
            return i;
        }
    }
    return -1;
}

/**
 * Add buff to character with stacking and exclusion logic
 */
int CombatBuff_Add(int character_id, int buff_id, int duration_ms) {
    if (character_id < 0 || character_id >= 1000) {
        return ERROR_INVALID_PARAMETER;
    }

    int* count = &g_buff_counts[character_id];

    // Check if buff already exists
    int existing_index = FindBuffIndex(character_id, buff_id);
    if (existing_index >= 0) {
        // Buff already exists
        if (CanBuffStack(buff_id)) {
            // Stack the buff (increase stack count, refresh duration)
            ActiveBuff* buff = &g_buffs[character_id][existing_index];
            buff->stack_count++;
            if (buff->stack_count > 10) buff->stack_count = 10;  // Max 10 stacks
            buff->duration_ms = duration_ms;  // Refresh duration
            buff->start_time = time(NULL);  // Reset timer

            printf("[Combat] Stacked buff %d on character %d (stack: %d)\n",
                buff_id, character_id, buff->stack_count);
        } else {
            // Buff doesn't stack, just refresh duration
            ActiveBuff* buff = &g_buffs[character_id][existing_index];
            buff->duration_ms = duration_ms;
            buff->start_time = time(NULL);

            printf("[Combat] Refreshed buff %d on character %d\n",
                buff_id, character_id);
        }
        return 0;
    }

    // Check for mutually exclusive buffs
    for (int i = 0; i < *count; i++) {
        if (AreBuffsMutuallyExclusive(buff_id, g_buffs[character_id][i].buff_id)) {
            // Remove conflicting buff
            int old_buff_id = g_buffs[character_id][i].buff_id;
            CombatBuff_Remove(character_id, old_buff_id);
            printf("[Combat] Removed conflicting buff %d to apply %d\n",
                old_buff_id, buff_id);
            break;
        }
    }

    // Check capacity again after potential removal
    if (*count >= MAX_BUFFS_PER_CHARACTER) {
        return ERROR_BUFFER_OVERFLOW;
    }

    // Add new buff
    ActiveBuff* buff = &g_buffs[character_id][*count];
    buff->buff_id = buff_id;
    buff->duration_ms = duration_ms;
    buff->start_time = time(NULL);
    buff->stack_count = 1;

    (*count)++;

    printf("[Combat] Added buff %d to character %d (duration: %dms)\n",
        buff_id, character_id, duration_ms);

    return 0;
}

/**
 * Remove buff from character
 */
int CombatBuff_Remove(int character_id, int buff_id) {
    if (character_id < 0 || character_id >= 1000) {
        return ERROR_INVALID_PARAMETER;
    }

    int count = g_buff_counts[character_id];
    for (int i = 0; i < count; i++) {
        if (g_buffs[character_id][i].buff_id == buff_id) {
            // Remove by shifting
            for (int j = i; j < count - 1; j++) {
                g_buffs[character_id][j] = g_buffs[character_id][j + 1];
            }
            g_buff_counts[character_id]--;
            printf("[Combat] Removed buff %d from character %d\n", buff_id, character_id);
            return 0;
        }
    }

    return ERROR_NOT_FOUND;
}

/**
 * Update buffs (remove expired)
 */
int CombatBuff_Update(int character_id) {
    if (character_id < 0 || character_id >= 1000) {
        return 0;
    }

    int count = g_buff_counts[character_id];
    int removed = 0;
    time_t now = time(NULL);

    for (int i = 0; i < count; ) {
        ActiveBuff* buff = &g_buffs[character_id][i];
        time_t elapsed = now - buff->start_time;

        if (elapsed * 1000 >= buff->duration_ms) {
            // Buff expired, remove it
            CombatBuff_Remove(character_id, buff->buff_id);
            count--;
            removed++;
        } else {
            i++;
        }
    }

    return removed;
}

/**
 * Get active buffs
 */
int CombatBuff_GetActive(int character_id, ActiveBuff* out_buffs, int max_count) {
    if (character_id < 0 || character_id >= 1000 || !out_buffs) {
        return 0;
    }

    int count = g_buff_counts[character_id];
    if (count > max_count) {
        count = max_count;
    }

    memcpy(out_buffs, g_buffs[character_id], count * sizeof(ActiveBuff));
    return count;
}

/**
 * Cleanup buff system
 */
void CombatBuff_Cleanup(void) {
    memset(g_buffs, 0, sizeof(g_buffs));
    memset(g_buff_counts, 0, sizeof(g_buff_counts));
    printf("[Combat] Buff system cleanup complete\n");
}
