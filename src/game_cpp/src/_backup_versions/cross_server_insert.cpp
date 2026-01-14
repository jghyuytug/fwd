
/* Cross-Server Matching 550-555 */
static int handle_cross_match_queue(ClientSession* s, const uint8_t* d, int l) {
    printf("[CROSS] QUEUE fd=%d\n", s->fd);
    if (l < 1) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 550, e, 4);
    }
    uint8_t mode = d[0];
    printf("[CROSS] Queue mode=%d\n", mode);
    uint8_t r[12];
    memset(r, 0, 12);
    uint32_t queue_id = (uint32_t)time(NULL);
    memcpy(r + 4, &queue_id, 4);
    r[8] = mode;
    return send_response(s, 550, r, 9);
}

static int handle_cross_match_cancel(ClientSession* s, const uint8_t* d, int l) {
    (void)d; (void)l;
    printf("[CROSS] CANCEL fd=%d\n", s->fd);
    uint8_t r[4] = {0, 0, 0, 0};
    return send_response(s, 551, r, 4);
}

static int handle_cross_match_status(ClientSession* s, const uint8_t* d, int l) {
    (void)d; (void)l;
    printf("[CROSS] STATUS fd=%d\n", s->fd);
    uint8_t r[16];
    memset(r, 0, 16);
    uint8_t in_queue = 0;
    uint32_t wait_time = 0;
    uint8_t players_found = 0;
    r[4] = in_queue;
    memcpy(r + 5, &wait_time, 4);
    r[9] = players_found;
    return send_response(s, 552, r, 10);
}

static int handle_cross_match_accept(ClientSession* s, const uint8_t* d, int l) {
    printf("[CROSS] ACCEPT fd=%d\n", s->fd);
    if (l < 4) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 553, e, 4);
    }
    uint32_t match_id;
    memcpy(&match_id, d, 4);
    printf("[CROSS] Accept match=%u\n", match_id);
    uint8_t r[8];
    memset(r, 0, 8);
    memcpy(r + 4, &match_id, 4);
    return send_response(s, 553, r, 8);
}

static int handle_cross_match_decline(ClientSession* s, const uint8_t* d, int l) {
    printf("[CROSS] DECLINE fd=%d\n", s->fd);
    if (l < 4) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 554, e, 4);
    }
    uint32_t match_id;
    memcpy(&match_id, d, 4);
    printf("[CROSS] Decline match=%u\n", match_id);
    uint8_t r[8];
    memset(r, 0, 8);
    memcpy(r + 4, &match_id, 4);
    return send_response(s, 554, r, 8);
}

static int handle_cross_match_result(ClientSession* s, const uint8_t* d, int l) {
    printf("[CROSS] RESULT fd=%d\n", s->fd);
    if (l < 8) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 555, e, 4);
    }
    uint32_t match_id, result;
    memcpy(&match_id, d, 4);
    memcpy(&result, d + 4, 4);
    printf("[CROSS] Match=%u result=%u\n", match_id, result);
    uint8_t r[16];
    memset(r, 0, 16);
    memcpy(r + 4, &match_id, 4);
    memcpy(r + 8, &result, 4);
    return send_response(s, 555, r, 12);
}

