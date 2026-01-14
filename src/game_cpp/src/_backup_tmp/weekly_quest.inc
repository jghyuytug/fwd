/*==============================================================================
 * Weekly Quest System (周常任务)
 *============================================================================*/

#define WEEKLY_QUEST_MAX 5
#define WEEKLY_QUEST_POOL_SIZE 10

typedef struct {
    uint16_t quest_id;
    uint8_t  type;
    uint32_t target;
    uint32_t reward_gold;
    uint32_t reward_exp;
    uint16_t reward_item;
    uint8_t  reward_count;
} WeeklyQuestDef;

static WeeklyQuestDef g_weekly_quest_pool[] = {
    {5001, 1, 20, 50000, 20000, 90006, 3},   /* 通关20次副本 */
    {5002, 2, 100, 30000, 15000, 0, 0},      /* 击杀100只怪物 */
    {5003, 3, 5, 40000, 18000, 90003, 5},    /* 完成5次深渊 */
    {5004, 4, 10, 25000, 12000, 0, 0},       /* 强化10次装备 */
    {5005, 5, 3, 60000, 25000, 90004, 1},    /* 完成3次死亡之塔 */
    {5006, 6, 50, 20000, 10000, 0, 0},       /* 使用50个消耗品 */
    {5007, 7, 10, 35000, 16000, 90002, 2},   /* PVP战斗10场 */
    {5008, 8, 5, 45000, 20000, 0, 0},        /* 完成5次组队副本 */
    {0, 0, 0, 0, 0, 0, 0}
};

static int db_load_weekly_quests(uint32_t charac_no, DailyQuestProgress* quests, int max_count) {
    if (!g_db_charac || !quests) return -1;
    memset(quests, 0, sizeof(DailyQuestProgress) * max_count);

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT quest_id, progress, completed, claimed FROM weekly_quests "
             "WHERE charac_no=%u AND YEARWEEK(assign_date,1)=YEARWEEK(NOW(),1)",
             charac_no);

    if (mysql_query(g_db_charac, query)) return 0;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    MYSQL_ROW row;
    int count = 0;
    while ((row = mysql_fetch_row(result)) != NULL && count < max_count) {
        quests[count].quest_id = (uint16_t)atoi(row[0]);
        quests[count].progress = (uint32_t)atoi(row[1]);
        quests[count].completed = (uint8_t)atoi(row[2]);
        quests[count].claimed = (uint8_t)atoi(row[3]);
        count++;
    }
    mysql_free_result(result);
    return count;
}

static int db_save_weekly_quest(uint32_t charac_no, uint16_t quest_id, uint32_t progress,
                                uint8_t completed, uint8_t claimed) {
    if (!g_db_charac) return -1;
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO weekly_quests (charac_no, quest_id, progress, completed, claimed, assign_date) "
             "VALUES (%u, %u, %u, %d, %d, NOW()) "
             "ON DUPLICATE KEY UPDATE progress=%u, completed=%d, claimed=%d",
             charac_no, quest_id, progress, completed, claimed, progress, completed, claimed);
    return mysql_query(g_db_charac, query) ? -1 : 0;
}

static const WeeklyQuestDef* find_weekly_quest(uint16_t quest_id) {
    for (int i = 0; g_weekly_quest_pool[i].quest_id != 0; i++) {
        if (g_weekly_quest_pool[i].quest_id == quest_id) return &g_weekly_quest_pool[i];
    }
    return NULL;
}

/* msg_no=382: WEEKLY_QUEST_GET */
static int handle_weekly_quest_get(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[WEEKLY] GET from fd=%d\n", s->fd);

    DailyQuestProgress quests[WEEKLY_QUEST_MAX];
    int count = db_load_weekly_quests(s->character_id, quests, WEEKLY_QUEST_MAX);

    if (count == 0) {
        /* Assign new weekly quests */
        for (int i = 0; i < WEEKLY_QUEST_MAX && g_weekly_quest_pool[i].quest_id != 0; i++) {
            db_save_weekly_quest(s->character_id, g_weekly_quest_pool[i].quest_id, 0, 0, 0);
            quests[i].quest_id = g_weekly_quest_pool[i].quest_id;
            quests[i].progress = 0;
            quests[i].completed = 0;
            quests[i].claimed = 0;
            count++;
        }
    }

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 4;
    response[offset++] = (uint8_t)count;

    for (int i = 0; i < count; i++) {
        const WeeklyQuestDef* def = find_weekly_quest(quests[i].quest_id);
        memcpy(response + offset, &quests[i].quest_id, 2); offset += 2;
        response[offset++] = def ? def->type : 0;
        uint32_t target = def ? def->target : 0;
        memcpy(response + offset, &target, 4); offset += 4;
        memcpy(response + offset, &quests[i].progress, 4); offset += 4;
        response[offset++] = quests[i].completed;
        response[offset++] = quests[i].claimed;
    }

    return send_response(s, 382, response, offset);
}

/* msg_no=383: WEEKLY_QUEST_CLAIM */
static int handle_weekly_quest_claim(ClientSession* s, const uint8_t* data, int len) {
    printf("[WEEKLY] CLAIM from fd=%d\n", s->fd);
    if (len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 383, error, 4);
    }

    uint16_t quest_id;
    memcpy(&quest_id, data, 2);

    const WeeklyQuestDef* def = find_weekly_quest(quest_id);
    if (!def) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 383, error, 4);
    }

    DailyQuestProgress quests[WEEKLY_QUEST_MAX];
    db_load_weekly_quests(s->character_id, quests, WEEKLY_QUEST_MAX);

    DailyQuestProgress* q = NULL;
    for (int i = 0; i < WEEKLY_QUEST_MAX; i++) {
        if (quests[i].quest_id == quest_id) { q = &quests[i]; break; }
    }

    if (!q || !q->completed || q->claimed) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 383, error, 4);
    }

    /* Give rewards */
    if (def->reward_gold > 0) {
        s->inventory.money += def->reward_gold;
        db_update_gold(s->character_id, s->inventory.money);
    }
    if (def->reward_exp > 0) {
        db_add_exp(s->character_id, def->reward_exp);
    }
    if (def->reward_item > 0) {
        char query[256];
        snprintf(query, sizeof(query),
                 "INSERT INTO user_items (charac_no, it_id, count, slot) "
                 "SELECT %u, %u, %d, COALESCE(MAX(slot)+1, 0) FROM user_items WHERE charac_no=%u",
                 s->character_id, def->reward_item, def->reward_count, s->character_id);
        mysql_query(g_db_charac, query);
    }

    db_save_weekly_quest(s->character_id, quest_id, q->progress, 1, 1);
    printf("[WEEKLY] Claimed quest %d: gold=%u exp=%u\n", quest_id, def->reward_gold, def->reward_exp);

    uint8_t response[20];
    memset(response, 0, sizeof(response));
    int offset = 4;
    memcpy(response + offset, &quest_id, 2); offset += 2;
    memcpy(response + offset, &def->reward_gold, 4); offset += 4;
    memcpy(response + offset, &def->reward_exp, 4); offset += 4;

    return send_response(s, 383, response, offset);
}
