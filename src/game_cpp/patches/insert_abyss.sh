#!/bin/bash
# Insert abyss system code into main_db.cpp

cd /tmp/game_cpp_new/src

# Create handler code to insert
cat > /tmp/abyss_handlers.txt << 'HANDLERS_EOF'

/*==============================================================================
 * Phase 14: Abyss Dungeon System (490-499)
 * 深渊副本系统 - 入场券/特殊掉落/深渊Boss
 *============================================================================*/

/* Abyss constants */
#define ABYSS_MAX_DAILY_RUNS 5
#define ABYSS_TICKET_ITEM_ID 3001
#define ABYSS_MIN_LEVEL 50
#define ABYSS_MONSTER_MULT 2.0f
#define ABYSS_DROP_EPIC_RATE 15
#define ABYSS_DROP_LEGEND_RATE 3

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

/* DB: Get daily abyss run count */
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

/* DB: Increment daily abyss run count */
static int db_increment_abyss_count(uint32_t character_id) {
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO abyss_daily (charac_no, run_count, last_reset) "
        "VALUES (%u, 1, NOW()) ON DUPLICATE KEY UPDATE "
        "run_count = IF(DATE(last_reset)=CURDATE(), run_count+1, 1), last_reset = NOW()",
        character_id);
    return db_execute(query);
}

/* DB: Check if player has abyss ticket */
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

/* DB: Consume one abyss ticket */
static int db_consume_abyss_ticket(uint32_t character_id) {
    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE user_items SET add_info=add_info-1 WHERE charac_no=%u AND it_id=%d AND add_info>=1 LIMIT 1",
        character_id, ABYSS_TICKET_ITEM_ID);
    return db_execute(query);
}

/* DB: Log abyss clear */
static int db_log_abyss_clear(uint32_t character_id, uint32_t dungeon_id,
                              uint8_t abyss_type, uint32_t clear_time, uint8_t success) {
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO abyss_clear_log (charac_no, dungeon_id, abyss_type, clear_time, success, created_at) "
        "VALUES (%u, %u, %u, %u, %u, NOW())",
        character_id, dungeon_id, abyss_type, clear_time, success);
    return db_execute(query);
}

/* DB: Get abyss statistics */
static int db_get_abyss_stats(uint32_t character_id, uint32_t* total_clears, uint32_t* best_time) {
    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*), MIN(clear_time) FROM abyss_clear_log WHERE charac_no=%u AND success=1",
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

/* Find abyss state by character */
static AbyssState* find_abyss_state(uint32_t character_id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (g_abyss_states[i].character_id == character_id) return &g_abyss_states[i];
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

/* msg_no=490: ABYSS_ENTER */
static int handle_abyss_enter(ClientSession* s, const uint8_t* data, int len) {
    printf("[ABYSS] ENTER from fd=%d charac=%u\n", s->fd, s->character_id);
    if (len < 5) { uint8_t e[4]={1,0,0,0}; return send_response(s,490,e,4); }
    uint32_t dungeon_id; uint8_t abyss_type;
    memcpy(&dungeon_id, data, 4); abyss_type = data[4];
    if (s->level < ABYSS_MIN_LEVEL) { uint8_t e[4]={2,0,0,0}; return send_response(s,490,e,4); }
    int daily_count = db_get_abyss_daily_count(s->character_id);
    if (daily_count >= ABYSS_MAX_DAILY_RUNS) { uint8_t e[4]={3,0,0,0}; return send_response(s,490,e,4); }
    if (!db_has_abyss_ticket(s->character_id)) { uint8_t e[4]={4,0,0,0}; return send_response(s,490,e,4); }
    if (find_abyss_state(s->character_id)) { uint8_t e[4]={5,0,0,0}; return send_response(s,490,e,4); }
    db_consume_abyss_ticket(s->character_id);
    db_increment_abyss_count(s->character_id);
    AbyssState* state = alloc_abyss_state(s->character_id);
    if (!state) { uint8_t e[4]={6,0,0,0}; return send_response(s,490,e,4); }
    state->dungeon_id = dungeon_id; state->abyss_type = abyss_type;
    state->current_room = 0; state->total_rooms = 4 + abyss_type;
    state->start_time = (uint32_t)time(NULL); state->monsters_killed = 0; state->boss_defeated = 0;
    uint8_t response[16]; memset(response,0,sizeof(response)); int offset=0;
    uint32_t result=0; memcpy(response+offset,&result,4); offset+=4;
    memcpy(response+offset,&dungeon_id,4); offset+=4;
    response[offset++]=abyss_type; response[offset++]=state->total_rooms;
    response[offset++]=(uint8_t)(daily_count+1); response[offset++]=ABYSS_MAX_DAILY_RUNS;
    return send_response(s, 490, response, offset);
}

/* msg_no=491: ABYSS_EXIT */
static int handle_abyss_exit(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ABYSS] EXIT from fd=%d charac=%u\n", s->fd, s->character_id);
    AbyssState* state = find_abyss_state(s->character_id);
    if (!state) { uint8_t e[4]={1,0,0,0}; return send_response(s,491,e,4); }
    uint32_t elapsed = (uint32_t)time(NULL) - state->start_time;
    db_log_abyss_clear(s->character_id, state->dungeon_id, state->abyss_type, elapsed, 0);
    free_abyss_state(s->character_id);
    uint8_t response[4]; memset(response,0,4); return send_response(s,491,response,4);
}

/* msg_no=492: ABYSS_GET_INFO */
static int handle_abyss_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ABYSS] GET_INFO from fd=%d\n", s->fd);
    int daily_count = db_get_abyss_daily_count(s->character_id);
    int has_ticket = db_has_abyss_ticket(s->character_id);
    uint32_t total_clears=0, best_time=0;
    db_get_abyss_stats(s->character_id, &total_clears, &best_time);
    AbyssState* state = find_abyss_state(s->character_id);
    uint8_t response[32]; memset(response,0,sizeof(response)); int offset=0;
    uint32_t result=0; memcpy(response+offset,&result,4); offset+=4;
    response[offset++]=(uint8_t)daily_count; response[offset++]=ABYSS_MAX_DAILY_RUNS;
    response[offset++]=(uint8_t)has_ticket; response[offset++]=state?1:0;
    memcpy(response+offset,&total_clears,4); offset+=4;
    memcpy(response+offset,&best_time,4); offset+=4;
    if(state) { response[offset++]=state->current_room; response[offset++]=state->total_rooms; response[offset++]=state->abyss_type; }
    return send_response(s, 492, response, offset);
}

/* msg_no=493: ABYSS_CLEAR */
static int handle_abyss_clear(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ABYSS] CLEAR from fd=%d\n", s->fd);
    AbyssState* state = find_abyss_state(s->character_id);
    if (!state) { uint8_t e[4]={1,0,0,0}; return send_response(s,493,e,4); }
    if (!state->boss_defeated) { uint8_t e[4]={2,0,0,0}; return send_response(s,493,e,4); }
    uint32_t clear_time = (uint32_t)time(NULL) - state->start_time;
    uint32_t gold_reward = 10000 + (state->abyss_type * 5000) + (state->monsters_killed * 100);
    uint32_t exp_reward = 5000 + (state->abyss_type * 2500);
    int abyss_stones = 3 + (state->abyss_type * 2) + (rand() % 3);
    db_add_gold(s->character_id, gold_reward);
    db_add_exp(s->character_id, exp_reward);
    db_add_item(s->character_id, 3002, abyss_stones);
    db_log_abyss_clear(s->character_id, state->dungeon_id, state->abyss_type, clear_time, 1);
    uint32_t dungeon_id = state->dungeon_id; uint8_t abyss_type = state->abyss_type;
    free_abyss_state(s->character_id);
    uint8_t response[32]; memset(response,0,sizeof(response)); int offset=0;
    uint32_t result=0; memcpy(response+offset,&result,4); offset+=4;
    memcpy(response+offset,&dungeon_id,4); offset+=4; response[offset++]=abyss_type;
    memcpy(response+offset,&clear_time,4); offset+=4;
    memcpy(response+offset,&gold_reward,4); offset+=4;
    memcpy(response+offset,&exp_reward,4); offset+=4;
    response[offset++]=(uint8_t)abyss_stones;
    return send_response(s, 493, response, offset);
}

/* msg_no=494: ABYSS_ROOM_CLEAR */
static int handle_abyss_room_clear(ClientSession* s, const uint8_t* data, int len) {
    printf("[ABYSS] ROOM_CLEAR from fd=%d\n", s->fd);
    if (len < 4) { uint8_t e[4]={1,0,0,0}; return send_response(s,494,e,4); }
    AbyssState* state = find_abyss_state(s->character_id);
    if (!state) { uint8_t e[4]={2,0,0,0}; return send_response(s,494,e,4); }
    uint32_t monsters_killed; memcpy(&monsters_killed, data, 4);
    state->monsters_killed += monsters_killed; state->current_room++;
    uint8_t is_boss_room = (state->current_room >= state->total_rooms) ? 1 : 0;
    if (is_boss_room) state->boss_defeated = 1;
    uint8_t response[12]; memset(response,0,sizeof(response)); int offset=0;
    uint32_t result=0; memcpy(response+offset,&result,4); offset+=4;
    response[offset++]=state->current_room; response[offset++]=state->total_rooms;
    response[offset++]=is_boss_room; response[offset++]=state->boss_defeated;
    return send_response(s, 494, response, offset);
}

/* msg_no=495: ABYSS_GET_RANKING */
static int handle_abyss_get_ranking(ClientSession* s, const uint8_t* data, int len) {
    printf("[ABYSS] GET_RANKING from fd=%d\n", s->fd);
    uint8_t abyss_type = (len >= 1) ? data[0] : 0;
    char query[512];
    snprintf(query, sizeof(query),
        "SELECT c.charac_name, a.charac_no, MIN(a.clear_time), COUNT(*) "
        "FROM abyss_clear_log a JOIN charac_info c ON a.charac_no=c.charac_no "
        "WHERE a.success=1 AND a.abyss_type=%d GROUP BY a.charac_no ORDER BY MIN(a.clear_time) LIMIT 10",
        abyss_type);
    MYSQL_RES* res = db_query(query);
    uint8_t response[256]; memset(response,0,sizeof(response)); int offset=0;
    uint32_t result=0; memcpy(response+offset,&result,4); offset+=4;
    response[offset++]=abyss_type;
    int count_offset=offset++; int rank_count=0;
    if(res) {
        MYSQL_ROW row;
        while((row=mysql_fetch_row(res)) && rank_count<10) {
            strncpy((char*)(response+offset), row[0]?row[0]:"Unknown", 15); offset+=16;
            uint32_t cid=row[1]?atoi(row[1]):0; memcpy(response+offset,&cid,4); offset+=4;
            uint32_t bt=row[2]?atoi(row[2]):0; memcpy(response+offset,&bt,4); offset+=4;
            uint32_t cc=row[3]?atoi(row[3]):0; memcpy(response+offset,&cc,4); offset+=4;
            rank_count++;
        }
        mysql_free_result(res);
    }
    response[count_offset]=(uint8_t)rank_count;
    return send_response(s, 495, response, offset);
}

HANDLERS_EOF

# Create case statements to insert
cat > /tmp/abyss_cases.txt << 'CASES_EOF'

        /* ========== Phase 14: Abyss System (490-499) ========== */
        case 490: return handle_abyss_enter(s, data, data_len);      /* 进入深渊 */
        case 491: return handle_abyss_exit(s, data, data_len);       /* 退出深渊 */
        case 492: return handle_abyss_get_info(s, data, data_len);   /* 获取深渊信息 */
        case 493: return handle_abyss_clear(s, data, data_len);      /* 深渊通关 */
        case 494: return handle_abyss_room_clear(s, data, data_len); /* 房间清理 */
        case 495: return handle_abyss_get_ranking(s, data, data_len);/* 深渊排行榜 */
CASES_EOF

# Find insertion points
HANDLER_LINE=$(grep -n "^/\* Default handler \*/" main_db.cpp | head -1 | cut -d: -f1)
CASE_LINE=$(grep -n "case 482:" main_db.cpp | head -1 | cut -d: -f1)

echo "Inserting handlers at line $HANDLER_LINE"
echo "Inserting cases after line $CASE_LINE"

# Insert handlers before "/* Default handler */"
head -n $((HANDLER_LINE - 1)) main_db.cpp > main_db_new.cpp
cat /tmp/abyss_handlers.txt >> main_db_new.cpp
tail -n +$HANDLER_LINE main_db.cpp >> main_db_new.cpp

# Now find the new case line position
CASE_LINE_NEW=$(grep -n "case 482:" main_db_new.cpp | head -1 | cut -d: -f1)
echo "New case line: $CASE_LINE_NEW"

# Insert case statements after case 482
head -n $CASE_LINE_NEW main_db_new.cpp > main_db_final.cpp
cat /tmp/abyss_cases.txt >> main_db_final.cpp
tail -n +$((CASE_LINE_NEW + 1)) main_db_new.cpp >> main_db_final.cpp

# Replace original
mv main_db_final.cpp main_db.cpp
rm -f main_db_new.cpp /tmp/abyss_handlers.txt /tmp/abyss_cases.txt

echo "Done! Abyss system code inserted."
