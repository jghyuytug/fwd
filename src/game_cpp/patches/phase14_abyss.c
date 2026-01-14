/*==============================================================================
 * Phase 14: Abyss Dungeon System (490-499)
 * 深渊副本系统 - 入场券/特殊掉落/深渊Boss
 *============================================================================*/

/* Abyss constants */
#define ABYSS_MAX_DAILY_RUNS 5
#define ABYSS_TICKET_ITEM_ID 3001  /* 深渊入场券物品ID */
#define ABYSS_MIN_LEVEL 50
#define ABYSS_MONSTER_MULT 2.0f    /* 怪物属性倍率 */
#define ABYSS_DROP_EPIC_RATE 15    /* 史诗掉落率% */
#define ABYSS_DROP_LEGEND_RATE 3   /* 传说掉落率% */

/* Abyss dungeon types */
#define ABYSS_TYPE_NORMAL 0
#define ABYSS_TYPE_HELL 1
#define ABYSS_TYPE_EXTREME 2

/* Abyss state tracking */
typedef struct {
    uint32_t character_id;
    uint32_t dungeon_id;
    uint8_t abyss_type;
    uint8_t current_room;
    uint8_t total_rooms;
    uint32_t start_time;
    uint32_t monsters_killed;
    uint8_t boss_defeated;
} AbyssState;

static AbyssState g_abyss_states[MAX_CLIENTS];

/*------------------------------------------------------------------------------
 * DB Functions for Abyss System
 *----------------------------------------------------------------------------*/

/* Get daily abyss run count */
static int db_get_abyss_daily_count(uint32_t character_id) {
    char query[256];
    snprintf(query, sizeof(query),
        "SELECT run_count FROM abyss_daily WHERE charac_no=%u AND DATE(last_reset)=CURDATE()",
        character_id);

    MYSQL_RES* res = db_query(query);
    if (!res) return 0;

    MYSQL_ROW row = mysql_fetch_row(res);
    int count = row ? atoi(row[0]) : 0;
    mysql_free_result(res);
    return count;
}

/* Increment daily abyss run count */
static int db_increment_abyss_count(uint32_t character_id) {
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO abyss_daily (charac_no, run_count, last_reset) "
        "VALUES (%u, 1, NOW()) "
        "ON DUPLICATE KEY UPDATE "
        "run_count = IF(DATE(last_reset)=CURDATE(), run_count+1, 1), "
        "last_reset = NOW()",
        character_id);
    return db_execute(query);
}

/* Check if player has abyss ticket */
static int db_has_abyss_ticket(uint32_t character_id) {
    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM user_items WHERE charac_no=%u AND it_id=%d AND add_info>=1",
        character_id, ABYSS_TICKET_ITEM_ID);

    MYSQL_RES* res = db_query(query);
    if (!res) return 0;

    MYSQL_ROW row = mysql_fetch_row(res);
    int has = row ? atoi(row[0]) > 0 : 0;
    mysql_free_result(res);
    return has;
}

/* Consume one abyss ticket */
static int db_consume_abyss_ticket(uint32_t character_id) {
    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE user_items SET add_info=add_info-1 "
        "WHERE charac_no=%u AND it_id=%d AND add_info>=1 LIMIT 1",
        character_id, ABYSS_TICKET_ITEM_ID);
    return db_execute(query);
}

/* Log abyss clear */
static int db_log_abyss_clear(uint32_t character_id, uint32_t dungeon_id,
                              uint8_t abyss_type, uint32_t clear_time, uint8_t success) {
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO abyss_clear_log (charac_no, dungeon_id, abyss_type, clear_time, success, created_at) "
        "VALUES (%u, %u, %u, %u, %u, NOW())",
        character_id, dungeon_id, abyss_type, clear_time, success);
    return db_execute(query);
}

/* Get abyss statistics */
static int db_get_abyss_stats(uint32_t character_id, uint32_t* total_clears, uint32_t* best_time) {
    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*), MIN(clear_time) FROM abyss_clear_log "
        "WHERE charac_no=%u AND success=1",
        character_id);

    MYSQL_RES* res = db_query(query);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        *total_clears = row[0] ? atoi(row[0]) : 0;
        *best_time = row[1] ? atoi(row[1]) : 0;
    }
    mysql_free_result(res);
    return 0;
}

/*------------------------------------------------------------------------------
 * Abyss Helper Functions
 *----------------------------------------------------------------------------*/

/* Find abyss state by character */
static AbyssState* find_abyss_state(uint32_t character_id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (g_abyss_states[i].character_id == character_id) {
            return &g_abyss_states[i];
        }
    }
    return NULL;
}

/* Allocate new abyss state */
static AbyssState* alloc_abyss_state(uint32_t character_id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (g_abyss_states[i].character_id == 0) {
            memset(&g_abyss_states[i], 0, sizeof(AbyssState));
            g_abyss_states[i].character_id = character_id;
            return &g_abyss_states[i];
        }
    }
    return NULL;
}

/* Free abyss state */
static void free_abyss_state(uint32_t character_id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (g_abyss_states[i].character_id == character_id) {
            memset(&g_abyss_states[i], 0, sizeof(AbyssState));
            return;
        }
    }
}

/* Generate abyss loot */
static int generate_abyss_loot(uint32_t character_id, uint8_t abyss_type,
                               uint32_t* item_ids, uint8_t* item_count) {
    *item_count = 0;
    int rng = rand() % 100;

    /* Base drops - abyss stones */
    int abyss_stone_count = 3 + (abyss_type * 2) + (rand() % 3);
    item_ids[(*item_count)++] = 3002; /* Abyss Stone ID */

    /* Epic drop chance */
    int epic_rate = ABYSS_DROP_EPIC_RATE + (abyss_type * 5);
    if (rng < epic_rate && *item_count < 8) {
        /* Generate random epic item */
        item_ids[(*item_count)++] = 100000 + (rand() % 1000); /* Epic range */
    }

    /* Legendary drop chance */
    int legend_rate = ABYSS_DROP_LEGEND_RATE + abyss_type;
    rng = rand() % 100;
    if (rng < legend_rate && *item_count < 8) {
        /* Generate random legendary item */
        item_ids[(*item_count)++] = 200000 + (rand() % 500); /* Legend range */
    }

    /* Gold reward */
    item_ids[(*item_count)++] = 0; /* 0 = gold marker */

    return abyss_stone_count;
}

/*------------------------------------------------------------------------------
 * Protocol Handlers
 *----------------------------------------------------------------------------*/

/* msg_no=490: ABYSS_ENTER - 进入深渊副本 */
static int handle_abyss_enter(ClientSession* s, const uint8_t* data, int len) {
    printf("[ABYSS] ENTER from fd=%d charac=%u\n", s->fd, s->character_id);

    if (len < 5) {
        uint8_t error[4] = {1, 0, 0, 0}; /* Invalid data */
        return send_response(s, 490, error, 4);
    }

    uint32_t dungeon_id;
    uint8_t abyss_type;
    memcpy(&dungeon_id, data, 4);
    abyss_type = data[4];

    /* Check level requirement */
    if (s->level < ABYSS_MIN_LEVEL) {
        printf("[ABYSS] Level too low: %d < %d\n", s->level, ABYSS_MIN_LEVEL);
        uint8_t error[4] = {2, 0, 0, 0}; /* Level too low */
        return send_response(s, 490, error, 4);
    }

    /* Check daily limit */
    int daily_count = db_get_abyss_daily_count(s->character_id);
    if (daily_count >= ABYSS_MAX_DAILY_RUNS) {
        printf("[ABYSS] Daily limit reached: %d/%d\n", daily_count, ABYSS_MAX_DAILY_RUNS);
        uint8_t error[4] = {3, 0, 0, 0}; /* Daily limit */
        return send_response(s, 490, error, 4);
    }

    /* Check ticket */
    if (!db_has_abyss_ticket(s->character_id)) {
        printf("[ABYSS] No ticket\n");
        uint8_t error[4] = {4, 0, 0, 0}; /* No ticket */
        return send_response(s, 490, error, 4);
    }

    /* Check not already in abyss */
    if (find_abyss_state(s->character_id)) {
        uint8_t error[4] = {5, 0, 0, 0}; /* Already in abyss */
        return send_response(s, 490, error, 4);
    }

    /* Consume ticket and increment count */
    db_consume_abyss_ticket(s->character_id);
    db_increment_abyss_count(s->character_id);

    /* Create abyss state */
    AbyssState* state = alloc_abyss_state(s->character_id);
    if (!state) {
        uint8_t error[4] = {6, 0, 0, 0}; /* Server full */
        return send_response(s, 490, error, 4);
    }

    state->dungeon_id = dungeon_id;
    state->abyss_type = abyss_type;
    state->current_room = 0;
    state->total_rooms = 4 + abyss_type; /* 4-6 rooms based on type */
    state->start_time = (uint32_t)time(NULL);
    state->monsters_killed = 0;
    state->boss_defeated = 0;

    printf("[ABYSS] Entered dungeon %u type %d rooms %d\n",
           dungeon_id, abyss_type, state->total_rooms);

    /* Response: success + dungeon info */
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    memcpy(response + offset, &dungeon_id, 4); offset += 4;
    response[offset++] = abyss_type;
    response[offset++] = state->total_rooms;
    response[offset++] = (uint8_t)(daily_count + 1); /* Current run count */
    response[offset++] = ABYSS_MAX_DAILY_RUNS;       /* Max runs */

    return send_response(s, 490, response, offset);
}

/* msg_no=491: ABYSS_EXIT - 退出深渊副本 */
static int handle_abyss_exit(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ABYSS] EXIT from fd=%d charac=%u\n", s->fd, s->character_id);

    AbyssState* state = find_abyss_state(s->character_id);
    if (!state) {
        uint8_t error[4] = {1, 0, 0, 0}; /* Not in abyss */
        return send_response(s, 491, error, 4);
    }

    /* Log the exit (failed run) */
    uint32_t elapsed = (uint32_t)time(NULL) - state->start_time;
    db_log_abyss_clear(s->character_id, state->dungeon_id,
                       state->abyss_type, elapsed, 0);

    /* Free state */
    free_abyss_state(s->character_id);

    printf("[ABYSS] Exited (failed) after %u seconds\n", elapsed);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    memcpy(response, &result, 4);

    return send_response(s, 491, response, 4);
}

/* msg_no=492: ABYSS_GET_INFO - 获取深渊信息 */
static int handle_abyss_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ABYSS] GET_INFO from fd=%d charac=%u\n", s->fd, s->character_id);

    int daily_count = db_get_abyss_daily_count(s->character_id);
    int has_ticket = db_has_abyss_ticket(s->character_id);
    uint32_t total_clears = 0, best_time = 0;
    db_get_abyss_stats(s->character_id, &total_clears, &best_time);

    /* Check if currently in abyss */
    AbyssState* state = find_abyss_state(s->character_id);

    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    response[offset++] = (uint8_t)daily_count;       /* Today's runs */
    response[offset++] = ABYSS_MAX_DAILY_RUNS;       /* Max daily */
    response[offset++] = (uint8_t)has_ticket;        /* Has ticket */
    response[offset++] = state ? 1 : 0;              /* In abyss */
    memcpy(response + offset, &total_clears, 4); offset += 4;
    memcpy(response + offset, &best_time, 4); offset += 4;

    if (state) {
        response[offset++] = state->current_room;
        response[offset++] = state->total_rooms;
        response[offset++] = state->abyss_type;
    }

    return send_response(s, 492, response, offset);
}

/* msg_no=493: ABYSS_CLEAR - 深渊通关 */
static int handle_abyss_clear(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ABYSS] CLEAR from fd=%d charac=%u\n", s->fd, s->character_id);

    AbyssState* state = find_abyss_state(s->character_id);
    if (!state) {
        uint8_t error[4] = {1, 0, 0, 0}; /* Not in abyss */
        return send_response(s, 493, error, 4);
    }

    if (!state->boss_defeated) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Boss not defeated */
        return send_response(s, 493, error, 4);
    }

    /* Calculate clear time */
    uint32_t clear_time = (uint32_t)time(NULL) - state->start_time;

    /* Generate loot */
    uint32_t item_ids[8];
    uint8_t item_count = 0;
    int abyss_stones = generate_abyss_loot(s->character_id, state->abyss_type,
                                           item_ids, &item_count);

    /* Calculate gold reward */
    uint32_t gold_reward = 10000 + (state->abyss_type * 5000) +
                           (state->monsters_killed * 100);

    /* Calculate exp reward */
    uint32_t exp_reward = 5000 + (state->abyss_type * 2500);

    /* Give rewards */
    db_add_gold(s->character_id, gold_reward);
    db_add_exp(s->character_id, exp_reward);

    /* Give items */
    for (int i = 0; i < item_count; i++) {
        if (item_ids[i] > 0) {
            db_add_item(s->character_id, item_ids[i], 1);
        }
    }

    /* Give abyss stones */
    db_add_item(s->character_id, 3002, abyss_stones);

    /* Log success */
    db_log_abyss_clear(s->character_id, state->dungeon_id,
                       state->abyss_type, clear_time, 1);

    printf("[ABYSS] Cleared in %u sec, gold=%u, exp=%u, stones=%d, items=%d\n",
           clear_time, gold_reward, exp_reward, abyss_stones, item_count);

    /* Free state */
    uint32_t dungeon_id = state->dungeon_id;
    uint8_t abyss_type = state->abyss_type;
    free_abyss_state(s->character_id);

    /* Response */
    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    memcpy(response + offset, &dungeon_id, 4); offset += 4;
    response[offset++] = abyss_type;
    memcpy(response + offset, &clear_time, 4); offset += 4;
    memcpy(response + offset, &gold_reward, 4); offset += 4;
    memcpy(response + offset, &exp_reward, 4); offset += 4;
    response[offset++] = (uint8_t)abyss_stones;
    response[offset++] = item_count;

    for (int i = 0; i < item_count && offset < 60; i++) {
        memcpy(response + offset, &item_ids[i], 4); offset += 4;
    }

    return send_response(s, 493, response, offset);
}

/* msg_no=494: ABYSS_ROOM_CLEAR - 深渊房间清理 */
static int handle_abyss_room_clear(ClientSession* s, const uint8_t* data, int len) {
    printf("[ABYSS] ROOM_CLEAR from fd=%d charac=%u\n", s->fd, s->character_id);

    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 494, error, 4);
    }

    AbyssState* state = find_abyss_state(s->character_id);
    if (!state) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not in abyss */
        return send_response(s, 494, error, 4);
    }

    uint32_t monsters_killed;
    memcpy(&monsters_killed, data, 4);

    state->monsters_killed += monsters_killed;
    state->current_room++;

    /* Check if boss room */
    uint8_t is_boss_room = (state->current_room >= state->total_rooms) ? 1 : 0;
    if (is_boss_room) {
        state->boss_defeated = 1;
    }

    printf("[ABYSS] Room %d/%d cleared, killed=%u, boss=%d\n",
           state->current_room, state->total_rooms, monsters_killed, is_boss_room);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    response[offset++] = state->current_room;
    response[offset++] = state->total_rooms;
    response[offset++] = is_boss_room;
    response[offset++] = state->boss_defeated;

    return send_response(s, 494, response, offset);
}

/* msg_no=495: ABYSS_GET_RANKING - 深渊排行榜 */
static int handle_abyss_get_ranking(ClientSession* s, const uint8_t* data, int len) {
    printf("[ABYSS] GET_RANKING from fd=%d\n", s->fd);

    uint8_t abyss_type = 0;
    if (len >= 1) {
        abyss_type = data[0];
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT c.charac_name, a.charac_no, MIN(a.clear_time), COUNT(*) "
        "FROM abyss_clear_log a "
        "JOIN charac_info c ON a.charac_no = c.charac_no "
        "WHERE a.success=1 AND a.abyss_type=%d "
        "GROUP BY a.charac_no "
        "ORDER BY MIN(a.clear_time) ASC "
        "LIMIT 10",
        abyss_type);

    MYSQL_RES* res = db_query(query);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    response[offset++] = abyss_type;

    int rank_count = 0;
    int count_offset = offset++;

    if (res) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) && rank_count < 10) {
            /* Name (16 bytes max) */
            strncpy((char*)(response + offset), row[0] ? row[0] : "Unknown", 15);
            offset += 16;

            /* Character ID */
            uint32_t char_id = row[1] ? atoi(row[1]) : 0;
            memcpy(response + offset, &char_id, 4); offset += 4;

            /* Best time */
            uint32_t best_time = row[2] ? atoi(row[2]) : 0;
            memcpy(response + offset, &best_time, 4); offset += 4;

            /* Clear count */
            uint32_t clear_count = row[3] ? atoi(row[3]) : 0;
            memcpy(response + offset, &clear_count, 4); offset += 4;

            rank_count++;
        }
        mysql_free_result(res);
    }

    response[count_offset] = (uint8_t)rank_count;

    return send_response(s, 495, response, offset);
}

/*==============================================================================
 * End of Phase 14 Abyss System
 *============================================================================*/
