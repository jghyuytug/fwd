/*==============================================================================
 * Sign-In System (签到系统) - Protocol 534-536
 *============================================================================*/

#define SIGN_IN_MONTHLY_DAYS 31
#define SIGN_IN_STREAK_BONUS_7  1000   /* 7天连续奖励金币 */
#define SIGN_IN_STREAK_BONUS_14 3000   /* 14天连续奖励 */
#define SIGN_IN_STREAK_BONUS_21 5000   /* 21天连续奖励 */
#define SIGN_IN_STREAK_BONUS_28 10000  /* 28天连续奖励 */

/* 每日签到奖励表 */
static const uint32_t g_daily_sign_rewards[] = {
    1000, 1000, 1000, 2000, 2000, 2000, 3000,   /* Day 1-7 */
    1500, 1500, 1500, 2500, 2500, 2500, 4000,   /* Day 8-14 */
    2000, 2000, 2000, 3000, 3000, 3000, 5000,   /* Day 15-21 */
    2500, 2500, 2500, 3500, 3500, 3500, 8000,   /* Day 22-28 */
    3000, 3000, 10000                            /* Day 29-31 */
};

typedef struct {
    uint32_t charac_no;
    uint8_t  month;              /* 当前月份 1-12 */
    uint8_t  signed_days;        /* 本月已签天数 */
    uint8_t  streak;             /* 连续签到天数 */
    uint32_t last_sign_date;     /* 上次签到日期 YYYYMMDD */
    uint8_t  sign_bits[4];       /* 32位标记本月哪天签了 */
    uint8_t  makeup_used;        /* 本月已用补签次数 */
} SignInInfo;

/* DB: 加载签到信息 */
static int db_load_sign_in(uint32_t charac_no, SignInInfo* info) {
    if (!g_db_charac || !info) return -1;
    memset(info, 0, sizeof(SignInInfo));
    info->charac_no = charac_no;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT month, signed_days, streak, last_sign_date, sign_bits, makeup_used "
             "FROM game_sign_in WHERE charac_no=%u", charac_no);

    if (mysql_query(g_db_charac, query)) return 0;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            info->month = row[0] ? atoi(row[0]) : 0;
            info->signed_days = row[1] ? atoi(row[1]) : 0;
            info->streak = row[2] ? atoi(row[2]) : 0;
            info->last_sign_date = row[3] ? atoi(row[3]) : 0;
            if (row[4]) {
                unsigned long* lengths = mysql_fetch_lengths(result);
                if (lengths[4] >= 4) memcpy(info->sign_bits, row[4], 4);
            }
            info->makeup_used = row[5] ? atoi(row[5]) : 0;
        }
        mysql_free_result(result);
    }
    return 0;
}

/* DB: 保存签到信息 */
static int db_save_sign_in(SignInInfo* info) {
    if (!g_db_charac || !info) return -1;

    char bits_hex[12];
    snprintf(bits_hex, sizeof(bits_hex), "0x%02X%02X%02X%02X",
             info->sign_bits[0], info->sign_bits[1], info->sign_bits[2], info->sign_bits[3]);

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO game_sign_in (charac_no, month, signed_days, streak, last_sign_date, sign_bits, makeup_used) "
             "VALUES (%u, %d, %d, %d, %u, %s, %d) "
             "ON DUPLICATE KEY UPDATE month=%d, signed_days=%d, streak=%d, last_sign_date=%u, sign_bits=%s, makeup_used=%d",
             info->charac_no, info->month, info->signed_days, info->streak,
             info->last_sign_date, bits_hex, info->makeup_used,
             info->month, info->signed_days, info->streak, info->last_sign_date, bits_hex, info->makeup_used);

    return mysql_query(g_db_charac, query) ? -1 : 0;
}

/* 获取当前日期 YYYYMMDD */
static uint32_t get_today_date(void) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    return (t->tm_year + 1900) * 10000 + (t->tm_mon + 1) * 100 + t->tm_mday;
}

/* 检查并重置月份 */
static void check_month_reset(SignInInfo* info) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    uint8_t current_month = t->tm_mon + 1;

    if (info->month != current_month) {
        info->month = current_month;
        info->signed_days = 0;
        info->makeup_used = 0;
        memset(info->sign_bits, 0, 4);
    }
}

/* 检查今天是否已签 */
static int is_signed_today(SignInInfo* info) {
    uint32_t today = get_today_date();
    return info->last_sign_date == today;
}

/* 设置某天已签 */
static void set_day_signed(SignInInfo* info, int day) {
    if (day < 1 || day > 31) return;
    int idx = (day - 1) / 8;
    int bit = (day - 1) % 8;
    info->sign_bits[idx] |= (1 << bit);
}

/* 检查某天是否已签 */
static int is_day_signed(SignInInfo* info, int day) {
    if (day < 1 || day > 31) return 0;
    int idx = (day - 1) / 8;
    int bit = (day - 1) % 8;
    return (info->sign_bits[idx] >> bit) & 1;
}

/* msg_no=534: SIGN_IN_INFO - 获取签到信息 */
static int handle_sign_in_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[SIGNIN] INFO from fd=%d charac=%u\n", s->fd, s->character_id);

    SignInInfo info;
    db_load_sign_in(s->character_id, &info);
    check_month_reset(&info);

    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 4;
    response[offset++] = info.month;
    response[offset++] = info.signed_days;
    response[offset++] = info.streak;
    response[offset++] = is_signed_today(&info) ? 1 : 0;
    memcpy(response + offset, info.sign_bits, 4); offset += 4;
    response[offset++] = info.makeup_used;
    response[offset++] = 3 - info.makeup_used;  /* 剩余补签次数 */

    return send_response(s, 534, response, offset);
}

/* msg_no=535: SIGN_IN_DO - 执行签到 */
static int handle_sign_in_do(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[SIGNIN] DO from fd=%d charac=%u\n", s->fd, s->character_id);

    SignInInfo info;
    db_load_sign_in(s->character_id, &info);
    check_month_reset(&info);

    uint8_t response[32];
    memset(response, 0, sizeof(response));

    if (is_signed_today(&info)) {
        uint32_t err = 1;  /* 今天已签 */
        memcpy(response, &err, 4);
        return send_response(s, 535, response, 4);
    }

    /* 更新连续签到 */
    uint32_t today = get_today_date();
    uint32_t yesterday = today - 1;
    if (info.last_sign_date == yesterday) {
        info.streak++;
    } else {
        info.streak = 1;
    }

    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    int day = t->tm_mday;

    set_day_signed(&info, day);
    info.signed_days++;
    info.last_sign_date = today;

    /* 计算奖励 */
    uint32_t reward = g_daily_sign_rewards[day <= 31 ? day - 1 : 30];
    uint32_t streak_bonus = 0;
    if (info.streak == 7) streak_bonus = SIGN_IN_STREAK_BONUS_7;
    else if (info.streak == 14) streak_bonus = SIGN_IN_STREAK_BONUS_14;
    else if (info.streak == 21) streak_bonus = SIGN_IN_STREAK_BONUS_21;
    else if (info.streak == 28) streak_bonus = SIGN_IN_STREAK_BONUS_28;

    uint32_t total_reward = reward + streak_bonus;

    /* 发放奖励 */
    s->inventory.money += total_reward;
    db_update_gold(s->character_id, s->inventory.money);
    db_save_sign_in(&info);

    printf("[SIGNIN] Day %d signed, streak=%d, reward=%u+%u\n",
           day, info.streak, reward, streak_bonus);

    int offset = 4;
    response[offset++] = info.signed_days;
    response[offset++] = info.streak;
    memcpy(response + offset, &reward, 4); offset += 4;
    memcpy(response + offset, &streak_bonus, 4); offset += 4;

    return send_response(s, 535, response, offset);
}

/* msg_no=536: SIGN_IN_MAKEUP - 补签 */
static int handle_sign_in_makeup(ClientSession* s, const uint8_t* data, int len) {
    printf("[SIGNIN] MAKEUP from fd=%d\n", s->fd);
    if (len < 1) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 536, e, 4);
    }

    uint8_t target_day = data[0];
    if (target_day < 1 || target_day > 31) {
        uint8_t e[4] = {2, 0, 0, 0};
        return send_response(s, 536, e, 4);
    }

    SignInInfo info;
    db_load_sign_in(s->character_id, &info);
    check_month_reset(&info);

    uint8_t response[16];
    memset(response, 0, sizeof(response));

    /* 检查补签次数 */
    if (info.makeup_used >= 3) {
        uint32_t err = 3;  /* 补签次数用完 */
        memcpy(response, &err, 4);
        return send_response(s, 536, response, 4);
    }

    /* 检查目标日期是否已签 */
    if (is_day_signed(&info, target_day)) {
        uint32_t err = 4;  /* 已签过 */
        memcpy(response, &err, 4);
        return send_response(s, 536, response, 4);
    }

    /* 补签消耗：1000金币 */
    uint32_t cost = 1000;
    if (s->inventory.money < cost) {
        uint32_t err = 5;  /* 金币不足 */
        memcpy(response, &err, 4);
        return send_response(s, 536, response, 4);
    }

    /* 执行补签 */
    s->inventory.money -= cost;
    db_update_gold(s->character_id, s->inventory.money);

    set_day_signed(&info, target_day);
    info.signed_days++;
    info.makeup_used++;

    uint32_t reward = g_daily_sign_rewards[target_day <= 31 ? target_day - 1 : 30];
    s->inventory.money += reward;
    db_update_gold(s->character_id, s->inventory.money);
    db_save_sign_in(&info);

    printf("[SIGNIN] Makeup day %d, cost=%u, reward=%u\n", target_day, cost, reward);

    int offset = 4;
    response[offset++] = target_day;
    response[offset++] = info.signed_days;
    response[offset++] = 3 - info.makeup_used;
    memcpy(response + offset, &reward, 4); offset += 4;

    return send_response(s, 536, response, offset);
}
