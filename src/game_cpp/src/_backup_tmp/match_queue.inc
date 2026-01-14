/*==============================================================================
 * Cross-Server Match System (单服匹配队列)
 *============================================================================*/

#define MATCH_QUEUE_MAX 100
#define MATCH_LEVEL_RANGE 5
#define MATCH_TIMEOUT_SEC 300

typedef struct {
    uint32_t charac_no;
    uint32_t account_id;
    int      fd;
    uint8_t  level;
    uint8_t  mode;       /* 1=1v1 2=3v3 3=随机副本 */
    uint32_t queue_time;
    uint8_t  active;
} MatchQueueEntry;

typedef struct {
    uint32_t match_id;
    uint32_t players[6];
    uint8_t  player_count;
    uint8_t  mode;
    uint8_t  accepted[6];
    uint8_t  status;     /* 0=waiting 1=ready 2=started 3=cancelled */
    uint32_t create_time;
} MatchSession;

static MatchQueueEntry g_match_queue[MATCH_QUEUE_MAX];
static int g_queue_count = 0;
static MatchSession g_matches[20];
static int g_match_count = 0;
static uint32_t g_next_match_id = 1;

/* 初始化匹配系统 */
static void match_system_init(void) {
    memset(g_match_queue, 0, sizeof(g_match_queue));
    memset(g_matches, 0, sizeof(g_matches));
    g_queue_count = 0;
    g_match_count = 0;
}

/* 查找队列中的玩家 */
static MatchQueueEntry* find_queue_entry(uint32_t charac_no) {
    for (int i = 0; i < MATCH_QUEUE_MAX; i++) {
        if (g_match_queue[i].active && g_match_queue[i].charac_no == charac_no)
            return &g_match_queue[i];
    }
    return NULL;
}

/* 加入队列 */
static int match_queue_join(ClientSession* s, uint8_t mode) {
    if (find_queue_entry(s->character_id)) return -1; /* 已在队列 */

    for (int i = 0; i < MATCH_QUEUE_MAX; i++) {
        if (!g_match_queue[i].active) {
            g_match_queue[i].charac_no = s->character_id;
            g_match_queue[i].account_id = s->account_id;
            g_match_queue[i].fd = s->fd;
            g_match_queue[i].level = s->level;
            g_match_queue[i].mode = mode;
            g_match_queue[i].queue_time = (uint32_t)time(NULL);
            g_match_queue[i].active = 1;
            g_queue_count++;
            printf("[MATCH] %u joined queue mode=%d level=%d\n",
                   s->character_id, mode, s->level);
            return 0;
        }
    }
    return -2; /* 队列满 */
}

/* 离开队列 */
static int match_queue_leave(uint32_t charac_no) {
    MatchQueueEntry* e = find_queue_entry(charac_no);
    if (e) {
        e->active = 0;
        g_queue_count--;
        printf("[MATCH] %u left queue\n", charac_no);
        return 0;
    }
    return -1;
}

/* 尝试匹配 */
static MatchSession* try_match(MatchQueueEntry* entry) {
    int required = (entry->mode == 2) ? 6 : 2; /* 3v3需要6人，1v1需要2人 */
    MatchQueueEntry* candidates[6];
    int found = 0;
    candidates[found++] = entry;

    for (int i = 0; i < MATCH_QUEUE_MAX && found < required; i++) {
        MatchQueueEntry* other = &g_match_queue[i];
        if (!other->active || other == entry) continue;
        if (other->mode != entry->mode) continue;

        /* 等级差距检查 */
        int level_diff = (int)other->level - (int)entry->level;
        if (level_diff < 0) level_diff = -level_diff;
        if (level_diff > MATCH_LEVEL_RANGE) continue;

        candidates[found++] = other;
    }

    if (found < required) return NULL;

    /* 创建匹配 */
    for (int i = 0; i < 20; i++) {
        if (g_matches[i].status == 0 || g_matches[i].status >= 3) {
            MatchSession* m = &g_matches[i];
            m->match_id = g_next_match_id++;
            m->mode = entry->mode;
            m->player_count = found;
            m->status = 1; /* ready */
            m->create_time = (uint32_t)time(NULL);
            memset(m->accepted, 0, sizeof(m->accepted));

            for (int j = 0; j < found; j++) {
                m->players[j] = candidates[j]->charac_no;
                candidates[j]->active = 0;
                g_queue_count--;
            }

            g_match_count++;
            printf("[MATCH] Created match %u with %d players\n", m->match_id, found);
            return m;
        }
    }
    return NULL;
}

/* 查找匹配会话 */
static MatchSession* find_match(uint32_t match_id) {
    for (int i = 0; i < 20; i++) {
        if (g_matches[i].match_id == match_id && g_matches[i].status > 0)
            return &g_matches[i];
    }
    return NULL;
}

/* 检查玩家是否在匹配中 */
static int find_player_in_match(MatchSession* m, uint32_t charac_no) {
    for (int i = 0; i < m->player_count; i++) {
        if (m->players[i] == charac_no) return i;
    }
    return -1;
}
