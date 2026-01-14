/**
 * Quick Party Matching System - Header
 * Purpose: Automatic party matching for dungeons
 *
 * Phase: 6.3 - Party System Implementation
 * Created: 2025-11-23
 */

#ifndef INCLUDE_PARTY_QUICK_PARTY_H_
#define INCLUDE_PARTY_QUICK_PARTY_H_

#include <common/defs.h>
#include "../../src/party/party_interface.h"

/**
 * Dungeon type for matching
 */
typedef enum {
    DUNGEON_TYPE_ANY = 0,
    DUNGEON_TYPE_NORMAL = 1,
    DUNGEON_TYPE_HARD = 2,
    DUNGEON_TYPE_EXPERT = 3,
    DUNGEON_TYPE_MASTER = 4
} DungeonType;

/**
 * Job role for matching
 */
typedef enum {
    JOB_ROLE_ANY = 0,
    JOB_ROLE_TANK = 1,
    JOB_ROLE_DPS = 2,
    JOB_ROLE_SUPPORT = 3
} JobRole;

/**
 * Match request structure
 */
typedef struct {
    int character_id;
    int level;
    int job_class;
    JobRole job_role;
    DungeonType dungeon_type;
    int dungeon_id;
    unsigned int queue_time;
    int active;
} MatchRequest;

/**
 * Match result structure
 */
typedef struct {
    int party_id;
    int member_ids[PARTY_MAX_MEMBERS];
    int member_count;
} MatchResult;

/**
 * Initialize quick party system
 * @return 0 on success
 */
int QuickParty_Initialize(void);

/**
 * Enter matchmaking queue
 * @param character_id Character ID
 * @param dungeon_type Dungeon type
 * @param dungeon_id Specific dungeon ID (0 = any)
 * @return 0 on success
 */
int QuickParty_EnterQueue(int character_id, DungeonType dungeon_type, int dungeon_id);

/**
 * Leave matchmaking queue
 * @param character_id Character ID
 * @return 0 on success
 */
int QuickParty_LeaveQueue(int character_id);

/**
 * Process matchmaking (should be called periodically)
 * @param out_matches Output array of match results
 * @param max_matches Maximum matches to process
 * @return Number of matches found
 */
int QuickParty_ProcessMatching(MatchResult* out_matches, int max_matches);

/**
 * Get current queue position
 * @param character_id Character ID
 * @return Queue position (1-based), or 0 if not in queue
 */
int QuickParty_GetQueuePosition(int character_id);

/**
 * Get estimated wait time
 * @param character_id Character ID
 * @return Estimated wait time in seconds, or -1 if not in queue
 */
int QuickParty_GetEstimatedWaitTime(int character_id);

/**
 * Get current queue size
 * @return Number of characters in queue
 */
int QuickParty_GetQueueSize(void);

/**
 * Cleanup quick party system
 */
void QuickParty_Cleanup(void);

#endif /* INCLUDE_PARTY_QUICK_PARTY_H_ */
