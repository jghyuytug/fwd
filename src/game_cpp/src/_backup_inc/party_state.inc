
/* Party member info */
#define MAX_PARTY_MEMBERS 4
typedef struct {
    uint32_t party_id;                           /* Unique party ID */
    uint32_t leader_id;                          /* Leader character ID */
    uint32_t members[MAX_PARTY_MEMBERS];         /* Member character IDs */
    uint8_t  member_count;
    uint8_t  loot_mode;                          /* 0=free, 1=round-robin, 2=leader */
    uint8_t  active;
} PartyInfo;

#define MAX_PARTIES 256
static PartyInfo g_parties[MAX_PARTIES];
static uint32_t g_next_party_id = 1;

/* Find party by ID */
static PartyInfo* find_party(uint32_t party_id) {
    for (int i = 0; i < MAX_PARTIES; i++) {
        if (g_parties[i].active && g_parties[i].party_id == party_id) {
            return &g_parties[i];
        }
    }
    return NULL;
}

/* Create a new party */
static PartyInfo* create_party(uint32_t leader_id) {
    for (int i = 0; i < MAX_PARTIES; i++) {
        if (!g_parties[i].active) {
            memset(&g_parties[i], 0, sizeof(PartyInfo));
            g_parties[i].party_id = g_next_party_id++;
            g_parties[i].leader_id = leader_id;
            g_parties[i].members[0] = leader_id;
            g_parties[i].member_count = 1;
            g_parties[i].loot_mode = 0;
            g_parties[i].active = 1;
            printf("[PARTY] Created party %u with leader %u\n",
                   g_parties[i].party_id, leader_id);
            return &g_parties[i];
        }
    }
    return NULL;  /* No free slots */
}

/* Add member to party */
static int party_add_member(PartyInfo* party, uint32_t member_id) {
    if (!party || party->member_count >= MAX_PARTY_MEMBERS) return -1;

    /* Check not already in party */
    for (int i = 0; i < (int)party->member_count; i++) {
        if (party->members[i] == member_id) return -1;
    }

    party->members[party->member_count++] = member_id;
    printf("[PARTY] Added member %u to party %u (count=%u)\n",
           member_id, party->party_id, party->member_count);
    return 0;
}

/* Remove member from party */
static int party_remove_member(PartyInfo* party, uint32_t member_id) {
    if (!party) return -1;

    int found = -1;
    for (int i = 0; i < (int)party->member_count; i++) {
        if (party->members[i] == member_id) {
            found = i;
            break;
        }
    }

    if (found < 0) return -1;

    /* Shift remaining members */
    for (int i = found; i < (int)party->member_count - 1; i++) {
        party->members[i] = party->members[i + 1];
    }
    party->member_count--;

    printf("[PARTY] Removed member %u from party %u (count=%u)\n",
           member_id, party->party_id, party->member_count);

    /* If party empty or leader left, dissolve party */
    if (party->member_count == 0 || member_id == party->leader_id) {
        if (party->member_count > 0) {
            /* Transfer leadership to first remaining member */
            party->leader_id = party->members[0];
            printf("[PARTY] New leader: %u\n", party->leader_id);
        } else {
            party->active = 0;
            printf("[PARTY] Party %u dissolved\n", party->party_id);
        }
    }

    return 0;
}

/* Notify all party members */
static void party_notify_members(PartyInfo* party, uint8_t msg_no,
                                  const uint8_t* data, int len) {
    if (!party) return;

    for (int i = 0; i < (int)party->member_count; i++) {
        uint32_t member_id = party->members[i];
        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].character_id == member_id) {
                send_response(&g_sessions[j], msg_no, data, len);
                break;
            }
        }
    }
}

