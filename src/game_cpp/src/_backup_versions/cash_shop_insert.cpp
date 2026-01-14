
/* Cash Shop 510-514 */
static int handle_cash_shop_get_list(ClientSession* s, const uint8_t* d, int l) {
    (void)d;(void)l;
    printf("[SHOP] GET_LIST fd=%d\n", s->fd);
    uint8_t r[64];
    memset(r, 0, 64);
    int o = 4;
    r[o++] = 3;
    uint32_t i = 90001, p = 1000;
    memcpy(r + o, &i, 4); o += 4;
    memcpy(r + o, &p, 4); o += 4;
    r[o++] = 0;
    i = 90002; p = 500;
    memcpy(r + o, &i, 4); o += 4;
    memcpy(r + o, &p, 4); o += 4;
    r[o++] = 0;
    i = 90003; p = 200;
    memcpy(r + o, &i, 4); o += 4;
    memcpy(r + o, &p, 4); o += 4;
    r[o++] = 1;
    return send_response(s, 510, r, o);
}

static int handle_cash_shop_buy(ClientSession* s, const uint8_t* d, int l) {
    printf("[SHOP] BUY fd=%d\n", s->fd);
    if (l < 5) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 511, e, 4);
    }
    uint32_t id;
    memcpy(&id, d, 4);
    printf("[SHOP] Buy item=%u\n", id);
    uint8_t r[12];
    memset(r, 0, 12);
    memcpy(r + 4, &id, 4);
    return send_response(s, 511, r, 8);
}

static int handle_cash_shop_get_balance(ClientSession* s, const uint8_t* d, int l) {
    (void)d; (void)l;
    printf("[SHOP] GET_BALANCE fd=%d\n", s->fd);
    uint8_t r[16];
    memset(r, 0, 16);
    uint32_t cash = 10000, pts = 5000;
    memcpy(r + 4, &cash, 4);
    memcpy(r + 8, &pts, 4);
    return send_response(s, 512, r, 12);
}

static int handle_cash_shop_recharge(ClientSession* s, const uint8_t* d, int l) {
    printf("[SHOP] RECHARGE fd=%d\n", s->fd);
    if (l < 4) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 513, e, 4);
    }
    uint32_t amt;
    memcpy(&amt, d, 4);
    printf("[SHOP] Amount=%u\n", amt);
    uint8_t r[12];
    memset(r, 0, 12);
    memcpy(r + 4, &amt, 4);
    return send_response(s, 513, r, 8);
}

static int handle_cash_shop_gift(ClientSession* s, const uint8_t* d, int l) {
    printf("[SHOP] GIFT fd=%d\n", s->fd);
    if (l < 8) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 514, e, 4);
    }
    uint32_t target, item;
    memcpy(&target, d, 4);
    memcpy(&item, d + 4, 4);
    printf("[SHOP] Gift item=%u to=%u\n", item, target);
    uint8_t r[16];
    memset(r, 0, 16);
    memcpy(r + 4, &target, 4);
    memcpy(r + 8, &item, 4);
    return send_response(s, 514, r, 12);
}

