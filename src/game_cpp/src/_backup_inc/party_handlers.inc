/* msg_no=120: PARTY_CREATE */
static int handle_party_create(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PARTY] PARTY_CREATE from fd=%d\n", s->fd);

    if (s->party_id != 0) {
        uint8_t error[4] = {1, 0, 0, 0}; /* Already in party */
        return send_response(s, 120, error, 4);
    }

    PartyInfo* party = create_party(s->character_id);
    if (!party) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Cannot create party */
        return send_response(s, 120, error, 4);
    }

    s->party_id = party->party_id;

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &party->party_id, 4);
    return send_response(s, 120, response, 8);
}

/* msg_no=121: PARTY_INVITE */
static int handle_party_invite(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_INVITE from fd=%d\n", s->fd);
    if (len < 4 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 121, error, 4);
    }

    PartyInfo* party = find_party(s->party_id);
    if (!party || party->leader_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not leader */
        return send_response(s, 121, error, 4);
    }

    if (party->member_count >= MAX_PARTY_MEMBERS) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Party full */
        return send_response(s, 121, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    /* Find and notify target */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            if (g_sessions[i].party_id != 0) {
                uint8_t error[4] = {4, 0, 0, 0}; /* Target in party */
                return send_response(s, 121, error, 4);
            }
            uint8_t notify[64];
            memset(notify, 0, sizeof(notify));
            memcpy(notify + 4, &s->party_id, 4);
            memcpy(notify + 8, &s->character_id, 4);
            memcpy(notify + 12, s->character_name, 20);
            send_response(&g_sessions[i], 121, notify, 32);
            printf("[PARTY] Invited %u to party %u\n", target_id, s->party_id);
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 121, response, 8);
}

/* msg_no=122: PARTY_JOIN */
static int handle_party_join(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_JOIN from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 122, error, 4);
    }

    if (s->party_id != 0) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Already in party */
        return send_response(s, 122, error, 4);
    }

    uint32_t party_id;
    memcpy(&party_id, data, 4);

    PartyInfo* party = find_party(party_id);
    if (!party) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Party not found */
        return send_response(s, 122, error, 4);
    }

    if (party_add_member(party, s->character_id) < 0) {
        uint8_t error[4] = {4, 0, 0, 0}; /* Cannot join */
        return send_response(s, 122, error, 4);
    }

    s->party_id = party_id;

    /* Notify all party members */
    uint8_t notify[32];
    memset(notify, 0, sizeof(notify));
    uint32_t action = 1; /* Member joined */
    memcpy(notify, &action, 4);
    memcpy(notify + 4, &s->character_id, 4);
    memcpy(notify + 8, s->character_name, 20);
    party_notify_members(party, 125, notify, 28);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &party_id, 4);
    return send_response(s, 122, response, 8);
}

/* msg_no=123: PARTY_LEAVE */
static int handle_party_leave(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PARTY] PARTY_LEAVE from fd=%d\n", s->fd);

    if (s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 123, error, 4);
    }

    uint32_t old_party = s->party_id;
    PartyInfo* party = find_party(s->party_id);

    s->party_id = 0;

    if (party) {
        party_remove_member(party, s->character_id);

        /* Notify remaining members */
        if (party->active) {
            uint8_t notify[12];
            memset(notify, 0, sizeof(notify));
            uint32_t action = 2; /* Member left */
            memcpy(notify, &action, 4);
            memcpy(notify + 4, &s->character_id, 4);
            party_notify_members(party, 125, notify, 12);
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &old_party, 4);
    return send_response(s, 123, response, 8);
}

/* msg_no=124: PARTY_KICK */
static int handle_party_kick(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_KICK from fd=%d\n", s->fd);
    if (len < 4 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 124, error, 4);
    }

    PartyInfo* party = find_party(s->party_id);
    if (!party || party->leader_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not leader */
        return send_response(s, 124, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    if (target_id == s->character_id) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Cannot kick self */
        return send_response(s, 124, error, 4);
    }

    /* Remove from party */
    party_remove_member(party, target_id);

    /* Find and notify kicked player */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            g_sessions[i].party_id = 0;
            uint8_t notify[8] = {5, 0, 0, 0, 0, 0, 0, 0}; /* Kicked */
            send_response(&g_sessions[i], 124, notify, 8);
            break;
        }
    }

    /* Notify remaining members */
    uint8_t member_notify[12];
    memset(member_notify, 0, sizeof(member_notify));
    uint32_t action = 3; /* Member kicked */
    memcpy(member_notify, &action, 4);
    memcpy(member_notify + 4, &target_id, 4);
    party_notify_members(party, 125, member_notify, 12);

    printf("[PARTY] Kicked member %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 124, response, 8);
}

/* msg_no=125: PARTY_GET_INFO */
static int handle_party_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PARTY] PARTY_GET_INFO from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    PartyInfo* party = find_party(s->party_id);

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &s->party_id, 4);
    offset += 4;

    if (!party || !party->active) {
        /* No party or inactive */
        response[offset++] = 0; /* member_count = 0 */
        return send_response(s, 125, response, offset);
    }

    response[offset++] = (uint8_t)party->member_count;

    for (int i = 0; i < (int)party->member_count && i < MAX_PARTY_MEMBERS; i++) {
        uint32_t member_id = party->members[i];
        memcpy(response + offset, &member_id, 4);
        offset += 4;

        /* Find member info */
        char name[20] = "Unknown";
        uint8_t level = 1;
        uint8_t job = 0;

        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].character_id == member_id) {
                memcpy(name, g_sessions[j].character_name, 20);
                /* Find level/job from character data */
                for (int k = 0; k < g_sessions[j].character_count; k++) {
                    if (g_sessions[j].characters[k].charac_no == member_id) {
                        level = g_sessions[j].characters[k].level;
                        job = g_sessions[j].characters[k].job;
                        break;
                    }
                }
                break;
            }
        }

        memcpy(response + offset, name, 20);
        offset += 20;

        response[offset++] = level;
        response[offset++] = job;

        uint8_t is_leader = (member_id == party->leader_id) ? 1 : 0;
        response[offset++] = is_leader;
    }

    return send_response(s, 125, response, offset);
}

/* msg_no=126: PARTY_CHANGE_LEADER */
static int handle_party_change_leader(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_CHANGE_LEADER from fd=%d\n", s->fd);
    if (len < 4 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 126, error, 4);
    }

    PartyInfo* party = find_party(s->party_id);
    if (!party || party->leader_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not leader */
        return send_response(s, 126, error, 4);
    }

    uint32_t new_leader_id;
    memcpy(&new_leader_id, data, 4);

    /* Verify new leader is in party */
    int found = 0;
    for (int i = 0; i < (int)party->member_count; i++) {
        if (party->members[i] == new_leader_id) {
            found = 1;
            break;
        }
    }

    if (!found) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Not in party */
        return send_response(s, 126, error, 4);
    }

    party->leader_id = new_leader_id;
    printf("[PARTY] Changed leader to %u\n", new_leader_id);

    /* Notify all members */
    uint8_t notify[12];
    memset(notify, 0, sizeof(notify));
    uint32_t action = 4; /* Leader changed */
    memcpy(notify, &action, 4);
    memcpy(notify + 4, &new_leader_id, 4);
    party_notify_members(party, 125, notify, 12);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &new_leader_id, 4);
    return send_response(s, 126, response, 8);
}
