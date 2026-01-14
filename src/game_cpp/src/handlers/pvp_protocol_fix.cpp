/* PVP Protocol Fix - msg_no=53-56
 * Phase 17 P0 Day 1
 *
 * 将以下代码替换 handler_gameplay.cpp 中对应的空壳函数
 */

/* msg_no=53: MAKE_PVP_ROOM - Production protocol */
int handle_pvp_create_room(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] MAKE_PVP_ROOM from fd=%d, charac=%u\n", s->fd, s->character_id);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[8] = {1, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 53, error, 8);
    }
    if (s->pvp_match_id != 0) {
        uint8_t error[8] = {2, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 53, error, 8);
    }

    uint8_t mode = 0;
    if (len >= 1) {
        mode = data[0];
        if (mode > 3) mode = 0;
    }

    uint32_t room_id = create_pvp_room(s->character_id, mode);
    if (room_id == 0) {
        uint8_t error[8] = {3, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 53, error, 8);
    }

    s->pvp_match_id = room_id;
    s->pvp_state = 1;
    s->pvp_team = 0;
    s->pvp_team_index = 0;
    printf("[PVP] Room %u created, mode=%u\n", room_id, mode);

    uint8_t response[8];
    memset(response, 0, 8);
    memcpy(response + 4, &room_id, 4);
    return send_response(s, 53, response, 8);
}

/* msg_no=54: ENTER_PVP_ROOM - Production protocol */
int handle_pvp_enter_room(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] ENTER_PVP_ROOM from fd=%d, charac=%u\n", s->fd, s->character_id);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[12] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 54, error, 12);
    }
    if (s->pvp_match_id != 0) {
        uint8_t error[12] = {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 54, error, 12);
    }
    if (len < 4) {
        uint8_t error[12] = {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 54, error, 12);
    }

    uint32_t room_id;
    memcpy(&room_id, data, 4);

    PVPRoom* room = find_pvp_room(room_id);
    if (!room) {
        uint8_t error[12] = {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 54, error, 12);
    }

    int result = join_pvp_room(room_id, s->character_id);
    if (result < 0) {
        uint8_t error[12];
        memset(error, 0, 12);
        error[0] = (result == -2) ? 5 : 6;
        return send_response(s, 54, error, 12);
    }

    s->pvp_match_id = room_id;
    s->pvp_state = 1;
    s->pvp_team = room->player_count % 2;
    s->pvp_team_index = room->player_count - 1;
    printf("[PVP] Player %u joined room %u\n", s->character_id, room_id);

    uint8_t response[12];
    memset(response, 0, 12);
    memcpy(response + 4, &room_id, 4);
    response[8] = s->pvp_team;
    response[9] = room->player_count;
    response[10] = room->mode;
    response[11] = room->state;
    return send_response(s, 54, response, 12);
}

/* msg_no=55: SET_PVP_SEAT_STATE - Production protocol */
int handle_pvp_set_seat(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] SET_PVP_SEAT_STATE from fd=%d, charac=%u\n", s->fd, s->character_id);

    if (s->state < STATE_IN_GAME || s->pvp_match_id == 0) {
        uint8_t error[8] = {1, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 55, error, 8);
    }

    uint8_t seat_index = 0;
    uint8_t team = 0;
    if (len >= 1) seat_index = data[0];
    if (len >= 2) team = data[1];
    if (team > 1) team = 0;

    PVPRoom* room = find_pvp_room(s->pvp_match_id);
    if (!room) {
        uint8_t error[8] = {2, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 55, error, 8);
    }
    if (room->state != 0) {
        uint8_t error[8] = {3, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 55, error, 8);
    }

    int i;
    for (i = 0; i < room->player_count; i++) {
        if (room->players[i] == s->character_id) {
            room->team[i] = team;
            s->pvp_team = team;
            s->pvp_team_index = seat_index;
            break;
        }
    }
    printf("[PVP] Player %u set team=%u seat=%u\n", s->character_id, team, seat_index);

    uint8_t response[8];
    memset(response, 0, 8);
    response[4] = team;
    response[5] = seat_index;
    return send_response(s, 55, response, 8);
}

/* msg_no=56: SET_PVP_READY_STATE - Production protocol
 * 注意: 这个对应 handler_dispatch.cpp 中 case 56
 * 原来的 handle_pvp_ready 在 msg_no=135
 */
int handle_pvp_ready_state(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] SET_PVP_READY_STATE from fd=%d, charac=%u\n", s->fd, s->character_id);

    if (s->state < STATE_IN_GAME || s->pvp_match_id == 0) {
        uint8_t error[8] = {1, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 56, error, 8);
    }

    PVPRoom* room = find_pvp_room(s->pvp_match_id);
    if (!room) {
        uint8_t error[8] = {2, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 56, error, 8);
    }
    if (room->state != 0) {
        uint8_t error[8] = {3, 0, 0, 0, 0, 0, 0, 0};
        return send_response(s, 56, error, 8);
    }

    int all_ready = pvp_set_ready(s->pvp_match_id, s->character_id);
    printf("[PVP] Player %u set ready, all_ready=%d\n", s->character_id, all_ready);

    /* Response: [result:4][ready_state:1][all_ready:1][player_count:1][unused:1] */
    uint8_t response[8];
    memset(response, 0, 8);
    response[4] = 1;  /* This player is ready */
    response[5] = (all_ready > 0) ? 1 : 0;
    response[6] = room->player_count;
    return send_response(s, 56, response, 8);
}
