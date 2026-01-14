#include "handler_dungeon.h"

#include <cstdio>
#include <cstring>
#include <ctime>

#include "db/db_game.h"
#include "db/db_inventory.h"
#include "db/db_social.h"
#include "db/db_types.h"
#include "handlers/gameplay_helpers.h"
#include "game_packet_utils.h"
#include "game_server_context.h"
#include "handlers/handler_login.h"

SharedDungeonInstance g_shared_dungeons[MAX_SHARED_DUNGEONS];
static uint32_t g_next_shared_dungeon_id = 1;

static SharedDungeonInstance* find_shared_dungeon_by_party(uint32_t party_id);
static SharedDungeonInstance* find_shared_dungeon(uint32_t shared_id);
static SharedDungeonInstance* create_shared_dungeon(uint32_t party_id, uint32_t dungeon_id, uint8_t difficulty);
static int shared_dungeon_add_member(SharedDungeonInstance* sd, int fd);
static int shared_dungeon_remove_member(SharedDungeonInstance* sd, int fd);
static void shared_dungeon_broadcast(SharedDungeonInstance* sd, uint8_t msg_no, const uint8_t* data, int len);
static void shared_dungeon_broadcast_except(SharedDungeonInstance* sd, int except_fd, uint8_t msg_no, const uint8_t* data, int len);
static void shared_dungeon_spawn_monsters(SharedDungeonInstance* sd, int room);
static void sync_dungeon_from_shared(DungeonInstance* local, SharedDungeonInstance* sd);
static void sync_shared_from_dungeon(SharedDungeonInstance* sd, DungeonInstance* local);
static void dungeon_spawn_monsters(DungeonInstance* d, int room);

int handle_dungeon_enter(ClientSession* s, const uint8_t* data, int len);
int handle_dungeon_exit(ClientSession* s, const uint8_t* data, int len);
int handle_dungeon_info(ClientSession* s, const uint8_t* data, int len);
int handle_dungeon_attack(ClientSession* s, const uint8_t* data, int len);
int handle_dungeon_next_room(ClientSession* s, const uint8_t* data, int len);
int handle_dungeon_clear(ClientSession* s, const uint8_t* data, int len);
int handle_dungeon_reward(ClientSession* s, const uint8_t* data, int len);
int handle_dungeon_monster_tick(ClientSession* s, const uint8_t* data, int len);
int handle_dungeon_player_hit(ClientSession* s, const uint8_t* data, int len);
int handle_select_dungeon_difficulty(ClientSession* s, const uint8_t* data, int len);
int handle_ranking_get_dungeon(ClientSession* s, const uint8_t* data, int len);

static SharedDungeonInstance* find_shared_dungeon_by_party(uint32_t party_id) {
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (g_shared_dungeons[i].active && g_shared_dungeons[i].party_id == party_id) {
            return &g_shared_dungeons[i];
        }
    }
    return NULL;
}

/*==============================================================================
 * Dungeon System Handlers
 *============================================================================*/

/*------------------------------------------------------------------------------
 * Shared Party Dungeon Helper Functions
 *----------------------------------------------------------------------------*/

/* Find shared dungeon by party_id */
static SharedDungeonInstance* find_shared_dungeon_by_party(uint32_t party_id) {
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (g_shared_dungeons[i].active && g_shared_dungeons[i].party_id == party_id) {
            return &g_shared_dungeons[i];
        }
    }
    return NULL;
}

/* Find shared dungeon by ID */
static SharedDungeonInstance* find_shared_dungeon(uint32_t shared_id) {
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (g_shared_dungeons[i].active && g_shared_dungeons[i].shared_id == shared_id) {
            return &g_shared_dungeons[i];
        }
    }
    return NULL;
}

/* Create a new shared dungeon instance for party */
static SharedDungeonInstance* create_shared_dungeon(uint32_t party_id, uint32_t dungeon_id, uint8_t difficulty) {
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (!g_shared_dungeons[i].active) {
            SharedDungeonInstance* sd = &g_shared_dungeons[i];
            memset(sd, 0, sizeof(SharedDungeonInstance));
            sd->shared_id = g_next_shared_dungeon_id++;
            sd->party_id = party_id;
            sd->dungeon_id = dungeon_id;
            sd->difficulty = difficulty;
            sd->current_room = 0;
            sd->total_rooms = 3 + difficulty;
            sd->active = 1;
            sd->start_time = (uint32_t)time(NULL);
            sd->time_limit = 600;  /* 10 minutes */
            sd->monster_count = 0;
            sd->monsters_killed = 0;
            sd->member_count = 0;
            printf("[PARTY_DUNGEON] Created shared dungeon %u for party %u (dungeon=%u diff=%d)\n",
                   sd->shared_id, party_id, dungeon_id, difficulty);
            return sd;
        }
    }
    printf("[PARTY_DUNGEON] ERROR: No free shared dungeon slots!\n");
    return NULL;
}

/* Add member to shared dungeon */
static int shared_dungeon_add_member(SharedDungeonInstance* sd, int fd) {
    if (!sd || sd->member_count >= MAX_PARTY_MEMBERS_DUNGEON) return -1;

    /* Check not already added */
    for (int i = 0; i < (int)sd->member_count; i++) {
        if (sd->member_fds[i] == (uint32_t)fd) return -1;
    }

    sd->member_fds[sd->member_count++] = (uint32_t)fd;
    printf("[PARTY_DUNGEON] Added member fd=%d to shared dungeon %u (members=%d)\n",
           fd, sd->shared_id, sd->member_count);
    return 0;
}

/* Remove member from shared dungeon */
static int shared_dungeon_remove_member(SharedDungeonInstance* sd, int fd) {
    if (!sd) return -1;

    int found = -1;
    for (int i = 0; i < (int)sd->member_count; i++) {
        if (sd->member_fds[i] == (uint32_t)fd) {
            found = i;
            break;
        }
    }

    if (found < 0) return -1;

    /* Shift remaining members */
    for (int i = found; i < (int)sd->member_count - 1; i++) {
        sd->member_fds[i] = sd->member_fds[i + 1];
    }
    sd->member_count--;

    printf("[PARTY_DUNGEON] Removed member fd=%d from shared dungeon %u (members=%d)\n",
           fd, sd->shared_id, sd->member_count);

    /* If no members left, deactivate shared dungeon */
    if (sd->member_count == 0) {
        printf("[PARTY_DUNGEON] Shared dungeon %u closed (no members)\n", sd->shared_id);
        sd->active = 0;
    }

    return 0;
}

/* Broadcast dungeon update to all party members in dungeon */
static void shared_dungeon_broadcast(SharedDungeonInstance* sd, uint8_t msg_no,
                                      const uint8_t* data, int len) {
    if (!sd) return;

    for (int i = 0; i < (int)sd->member_count; i++) {
        int fd = (int)sd->member_fds[i];
        /* Find session by fd */
        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].fd == fd) {
                send_response(&g_sessions[j], msg_no, data, len);
                break;
            }
        }
    }
}

/* Broadcast dungeon update to all party members except sender */
static void shared_dungeon_broadcast_except(SharedDungeonInstance* sd, int except_fd,
                                             uint8_t msg_no, const uint8_t* data, int len) {
    if (!sd) return;

    for (int i = 0; i < (int)sd->member_count; i++) {
        int fd = (int)sd->member_fds[i];
        if (fd == except_fd) continue;  /* Skip sender */

        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].fd == fd) {
                send_response(&g_sessions[j], msg_no, data, len);
                break;
            }
        }
    }
}

/* Spawn monsters for shared dungeon */
static void shared_dungeon_spawn_monsters(SharedDungeonInstance* sd, int room);

/* Sync local dungeon state from shared instance */
static void sync_dungeon_from_shared(DungeonInstance* local, SharedDungeonInstance* sd) {
    if (!local || !sd) return;

    local->dungeon_id = sd->dungeon_id;
    local->difficulty = sd->difficulty;
    local->current_room = sd->current_room;
    local->total_rooms = sd->total_rooms;
    local->active = sd->active;
    local->start_time = sd->start_time;
    local->time_limit = sd->time_limit;
    local->monster_count = sd->monster_count;
    local->monsters_killed = sd->monsters_killed;

    /* Copy monster data */
    memcpy(local->monsters, sd->monsters, sizeof(MonsterInfo) * MAX_MONSTERS);
}

/* Sync shared instance from local dungeon state (after attack) */
static void sync_shared_from_dungeon(SharedDungeonInstance* sd, DungeonInstance* local) {
    if (!sd || !local) return;

    sd->current_room = local->current_room;
    sd->monster_count = local->monster_count;
    sd->monsters_killed = local->monsters_killed;

    /* Copy monster data */
    memcpy(sd->monsters, local->monsters, sizeof(MonsterInfo) * MAX_MONSTERS);
}

/* Helper: Spawn monsters for a dungeon room */
static void dungeon_spawn_monsters(DungeonInstance* d, int room) {
    d->monster_count = 0;

    /* Spawn 3-5 monsters per room based on difficulty */
    int base_count = 3 + d->difficulty;
    if (base_count > MAX_MONSTERS) base_count = MAX_MONSTERS;

    uint32_t current_time = (uint32_t)time(NULL);

    /* Determine if this room has a boss */
    int is_boss_room = (room == d->total_rooms - 1);
    /* Boss ID based on dungeon_id (1-3 for now) */
    uint32_t boss_id = (d->dungeon_id % 3) + 1;

    for (int i = 0; i < base_count; i++) {
        MonsterInfo* m = &d->monsters[i];
        memset(m, 0, sizeof(MonsterInfo));  /* Clear all fields including boss fields */

        m->monster_id = (room * 100) + i + 1;
        m->monster_type = 1000 + (d->dungeon_id % 10) * 100 + (i % 5);
        m->x = 100 + (i * 150) + (rand() % 50);
        m->y = 200 + (rand() % 100);
        m->max_hp = 1000 * (d->difficulty + 1) * (room + 1);
        m->hp = m->max_hp;
        m->alive = 1;

        /* Initialize AI fields */
        m->state = MONSTER_STATE_IDLE;
        m->spawn_x = m->x;
        m->spawn_y = m->y;
        m->target_x = m->x;
        m->target_y = m->y;
        m->target_player = 0;
        m->last_action_time = current_time;
        m->attack_cooldown = 0;
        m->hit_stun_frames = 0;

        /* Check if this is the boss (last monster in boss room) */
        int is_this_boss = (is_boss_room && i == base_count - 1);

        if (is_this_boss) {
            /* Initialize as boss using template system */
            boss_init_from_template(m, boss_id, d->difficulty);
            m->monster_id = (room * 100) + i + 1;  /* Preserve monster_id */
            m->x = 400;  /* Boss spawns in center */
            m->y = 250;
            m->spawn_x = m->x;
            m->spawn_y = m->y;
            m->move_speed = 3;
            m->aggro_range = 500;
            m->attack_range = 100;
        } else {
            /* Normal monster */
            m->ai_type = MONSTER_AI_NORMAL;
            m->is_boss = 0;
            m->move_speed = 5;
            m->aggro_range = 250;
            m->attack_range = 50;
            m->attack_damage = 100 * (d->difficulty + 1) * (room + 1);
            m->phys_def = 50 * (d->difficulty + 1);
            m->mag_def = 30 * (d->difficulty + 1);
        }

        d->monster_count++;
    }

    printf("[DUNGEON] Spawned %d monsters in room %d (boss_room=%d, boss_id=%u)\n",
           d->monster_count, room, is_boss_room, is_boss_room ? boss_id : 0);
}

/* Spawn monsters for shared dungeon (uses same logic as dungeon_spawn_monsters) */
static void shared_dungeon_spawn_monsters(SharedDungeonInstance* sd, int room) {
    sd->monster_count = 0;

    int base_count = 3 + sd->difficulty;
    if (base_count > MAX_MONSTERS) base_count = MAX_MONSTERS;

    uint32_t current_time = (uint32_t)time(NULL);

    /* Determine if this room has a boss */
    int is_boss_room = (room == (int)sd->total_rooms - 1);
    /* Boss ID based on dungeon_id (1-3 for now) */
    uint32_t boss_id = (sd->dungeon_id % 3) + 1;
    /* Party scaling factor */
    float party_scale = 1.0f + (sd->member_count - 1) * 0.3f;

    for (int i = 0; i < base_count; i++) {
        MonsterInfo* m = &sd->monsters[i];
        memset(m, 0, sizeof(MonsterInfo));

        m->monster_id = (room * 100) + i + 1;
        m->monster_type = 1000 + (sd->dungeon_id % 10) * 100 + (i % 5);
        m->x = 100 + (i * 150) + (rand() % 50);
        m->y = 200 + (rand() % 100);
        /* Scale HP for party size */
        m->max_hp = (uint32_t)(1000 * (sd->difficulty + 1) * (room + 1) * party_scale);
        m->hp = m->max_hp;
        m->alive = 1;

        m->state = MONSTER_STATE_IDLE;
        m->spawn_x = m->x;
        m->spawn_y = m->y;
        m->target_x = m->x;
        m->target_y = m->y;
        m->target_player = 0;
        m->last_action_time = current_time;
        m->attack_cooldown = 0;
        m->hit_stun_frames = 0;

        /* Check if this is the boss (last monster in boss room) */
        int is_this_boss = (is_boss_room && i == base_count - 1);

        if (is_this_boss) {
            /* Initialize as boss using template system with party scaling */
            boss_init_from_template(m, boss_id, sd->difficulty);
            m->monster_id = (room * 100) + i + 1;
            m->x = 400;
            m->y = 250;
            m->spawn_x = m->x;
            m->spawn_y = m->y;
            m->move_speed = 3;
            m->aggro_range = 500;
            m->attack_range = 100;
            /* Apply party scaling to boss HP */
            m->max_hp = (uint32_t)(m->max_hp * party_scale);
            m->hp = m->max_hp;
        } else {
            /* Normal monster */
            m->ai_type = MONSTER_AI_NORMAL;
            m->is_boss = 0;
            m->move_speed = 5;
            m->aggro_range = 250;
            m->attack_range = 50;
            m->attack_damage = 100 * (sd->difficulty + 1) * (room + 1);
            m->phys_def = 50 * (sd->difficulty + 1);
            m->mag_def = 30 * (sd->difficulty + 1);
        }

        sd->monster_count++;
    }

    printf("[PARTY_DUNGEON] Shared dungeon %u: Spawned %d monsters in room %d (members=%d, boss=%u)\n",
           sd->shared_id, sd->monster_count, room, sd->member_count, is_boss_room ? boss_id : 0);
}

int handle_dungeon_enter(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DUNGEON_ENTER from fd=%d party_id=%u\n", s->fd, s->party_id);

    if (s->state < STATE_IN_GAME || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 60, error, 4);
    }

    uint32_t dungeon_id;
    uint8_t difficulty;
    memcpy(&dungeon_id, data, 4);
    difficulty = data[4];

    printf("[DUNGEON] Enter: dungeon_id=%u difficulty=%d\n", dungeon_id, difficulty);

    /* P1: Check and consume fatigue with DB persistence */
    int fatigue_cost = 8 + difficulty * 2;  /* Higher difficulty = more fatigue */
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            if (s->characters[i].fatigue <= 0) {
                printf("[DUNGEON] No fatigue remaining\n");
                uint8_t error[4] = {5, 0, 0, 0};  /* No fatigue */
                return send_response(s, 60, error, 4);
            }
            if (s->characters[i].fatigue < fatigue_cost) {
                printf("[DUNGEON] Not enough fatigue: have=%d need=%d\n",
                       s->characters[i].fatigue, fatigue_cost);
                uint8_t error[4] = {5, 0, 0, 0};  /* Not enough fatigue */
                return send_response(s, 60, error, 4);
            }
            /* Consume fatigue */
            s->characters[i].fatigue -= fatigue_cost;
            if (s->characters[i].fatigue < 0) {
                s->characters[i].fatigue = 0;
            }
            /* Persist fatigue to DB */
            db_update_fatigue(s->character_id, s->characters[i].fatigue);
            printf("[DUNGEON] Consumed %d fatigue, remaining: %d\n",
                   fatigue_cost, s->characters[i].fatigue);
            break;
        }
    }

    /* Initialize local dungeon instance */
    DungeonInstance* d = &s->dungeon;
    memset(d, 0, sizeof(DungeonInstance));

    SharedDungeonInstance* sd = NULL;

    /* Check if player is in a party */
    if (s->party_id > 0) {
        PartyInfo* party = find_party(s->party_id);
        if (party && party->active) {
            /* Check if party already has a shared dungeon */
            sd = find_shared_dungeon_by_party(s->party_id);

            if (sd == NULL) {
                /* Party leader creates the shared dungeon */
                if (party->leader_id == s->character_id) {
                    sd = create_shared_dungeon(s->party_id, dungeon_id, difficulty);
                    if (sd == NULL) {
                        printf("[PARTY_DUNGEON] Failed to create shared dungeon\n");
                        uint8_t error[4] = {6, 0, 0, 0};  /* Server error */
                        return send_response(s, 60, error, 4);
                    }
                    /* Spawn monsters for shared dungeon */
                    shared_dungeon_add_member(sd, s->fd);
                    shared_dungeon_spawn_monsters(sd, 0);

                    /* Mark as host */
                    d->is_party_host = 1;
                    printf("[PARTY_DUNGEON] Leader created shared dungeon %u\n", sd->shared_id);
                } else {
                    /* Non-leader trying to enter without leader */
                    printf("[PARTY_DUNGEON] ERROR: Leader must enter first!\n");
                    uint8_t error[4] = {7, 0, 0, 0};  /* Party leader must enter first */
                    return send_response(s, 60, error, 4);
                }
            } else {
                /* Join existing shared dungeon */
                if (sd->dungeon_id != dungeon_id) {
                    printf("[PARTY_DUNGEON] ERROR: Dungeon mismatch (requested %u, party in %u)\n",
                           dungeon_id, sd->dungeon_id);
                    uint8_t error[4] = {8, 0, 0, 0};  /* Wrong dungeon */
                    return send_response(s, 60, error, 4);
                }
                shared_dungeon_add_member(sd, s->fd);
                d->is_party_host = 0;
                printf("[PARTY_DUNGEON] Member joined shared dungeon %u\n", sd->shared_id);
            }

            /* Store shared dungeon reference */
            d->party_id = s->party_id;
            d->shared_dungeon_id = sd->shared_id;

            /* Sync local dungeon from shared instance */
            sync_dungeon_from_shared(d, sd);

            /* Notify other party members that someone joined */
            uint8_t join_notify[16];
            memset(join_notify, 0, sizeof(join_notify));
            uint32_t member_id = s->character_id;
            memcpy(join_notify, &member_id, 4);
            join_notify[4] = 1;  /* Join notification */
            shared_dungeon_broadcast_except(sd, s->fd, 68, join_notify, 8);  /* msg_no=68: PARTY_DUNGEON_MEMBER_UPDATE */

            printf("[PARTY_DUNGEON] Entered shared dungeon, members=%d\n", sd->member_count);
        } else {
            /* Party not found, fall back to solo */
            printf("[DUNGEON] Party not found, entering solo\n");
            s->party_id = 0;
        }
    }

    /* Solo dungeon (no party or party not found) */
    if (s->party_id == 0) {
        d->dungeon_id = dungeon_id;
        d->difficulty = difficulty;
        d->current_room = 0;
        d->total_rooms = 3 + difficulty;
        d->active = 1;
        d->start_time = (uint32_t)time(NULL);
        d->time_limit = 600;
        d->monsters_killed = 0;
        d->party_id = 0;
        d->shared_dungeon_id = 0;
        d->is_party_host = 0;

        dungeon_spawn_monsters(d, 0);
    }

    s->state = STATE_IN_DUNGEON;

    /* Build response */
    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &d->dungeon_id, 4);
    offset += 4;

    response[offset++] = d->difficulty;
    response[offset++] = d->total_rooms;
    response[offset++] = d->current_room;

    /* Party info */
    response[offset++] = (d->party_id > 0) ? 1 : 0;  /* is_party_dungeon */
    if (d->party_id > 0 && sd) {
        response[offset++] = sd->member_count;
    } else {
        response[offset++] = 1;  /* Solo = 1 member */
    }

    /* Monster count for this room */
    uint8_t mon_count = (uint8_t)d->monster_count;
    response[offset++] = mon_count;

    /* Monster list: id(4) + type(2) + x(2) + y(2) + hp(4) = 14 bytes each */
    for (int i = 0; i < d->monster_count && offset < 240; i++) {
        MonsterInfo* m = &d->monsters[i];
        memcpy(response + offset, &m->monster_id, 4);
        offset += 4;
        memcpy(response + offset, &m->monster_type, 2);
        offset += 2;
        memcpy(response + offset, &m->x, 2);
        offset += 2;
        memcpy(response + offset, &m->y, 2);
        offset += 2;
        memcpy(response + offset, &m->hp, 4);
        offset += 4;
    }

    printf("[DUNGEON] Entered dungeon %u room 0/%d with %d monsters (party=%u)\n",
           d->dungeon_id, d->total_rooms, d->monster_count, d->party_id);

    return send_response(s, 60, response, offset);
}

/* msg_no=61: DUNGEON_EXIT */
int handle_dungeon_exit(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_EXIT from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 61, error, 4);
    }

    DungeonInstance* d = &s->dungeon;

    /* Handle party dungeon exit */
    if (d->party_id > 0 && d->shared_dungeon_id > 0) {
        SharedDungeonInstance* sd = find_shared_dungeon(d->shared_dungeon_id);
        if (sd) {
            /* Notify other party members */
            uint8_t exit_notify[16];
            memset(exit_notify, 0, sizeof(exit_notify));
            uint32_t member_id = s->character_id;
            memcpy(exit_notify, &member_id, 4);
            exit_notify[4] = 0;  /* Leave notification */
            shared_dungeon_broadcast_except(sd, s->fd, 68, exit_notify, 8);

            /* Remove from shared dungeon */
            shared_dungeon_remove_member(sd, s->fd);

            printf("[PARTY_DUNGEON] Member fd=%d left shared dungeon %u (remaining=%d)\n",
                   s->fd, sd->shared_id, sd->member_count);
        }
    }

    /* Reset dungeon state */
    d->active = 0;
    d->party_id = 0;
    d->shared_dungeon_id = 0;
    s->state = STATE_IN_GAME;

    printf("[DUNGEON] Exited dungeon (killed %d monsters)\n", d->monsters_killed);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &d->monsters_killed, 4);

    return send_response(s, 61, response, 8);
}

/* msg_no=62: DUNGEON_INFO (get current dungeon state) */
int handle_dungeon_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_INFO from fd=%d\n", s->fd);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    if (s->state != STATE_IN_DUNGEON || !s->dungeon.active) {
        uint32_t result = 1;  /* Not in dungeon */
        memcpy(response, &result, 4);
        return send_response(s, 62, response, 4);
    }

    DungeonInstance* d = &s->dungeon;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &d->dungeon_id, 4);
    offset += 4;

    response[offset++] = d->difficulty;
    response[offset++] = d->current_room;
    response[offset++] = d->total_rooms;

    /* Time remaining */
    uint32_t elapsed = (uint32_t)time(NULL) - d->start_time;
    uint32_t remaining = (elapsed < d->time_limit) ? (d->time_limit - elapsed) : 0;
    memcpy(response + offset, &remaining, 4);
    offset += 4;

    /* Monsters alive in current room */
    uint8_t alive_count = 0;
    for (int i = 0; i < d->monster_count; i++) {
        if (d->monsters[i].alive) alive_count++;
    }
    response[offset++] = alive_count;

    memcpy(response + offset, &d->monsters_killed, 4);
    offset += 4;

    return send_response(s, 62, response, offset);
}

/* msg_no=65: DUNGEON_ATTACK (attack a monster with skill damage calculation) */
int handle_dungeon_attack(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DUNGEON_ATTACK from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 65, error, 4);
    }

    uint32_t monster_id;
    uint16_t skill_id = 100;  /* Default: normal attack */
    uint8_t skill_level = 1;
    memcpy(&monster_id, data, 4);

    /* Parse skill_id if provided (offset 4-5) */
    if (len >= 6) {
        memcpy(&skill_id, data + 4, 2);
    }
    /* Parse skill_level if provided (offset 6) */
    if (len >= 7) {
        skill_level = data[6];
    }

    DungeonInstance* d = &s->dungeon;
    SharedDungeonInstance* sd = NULL;

    /* For party dungeons, operate on shared instance */
    if (d->party_id > 0 && d->shared_dungeon_id > 0) {
        sd = find_shared_dungeon(d->shared_dungeon_id);
        if (sd) {
            /* Sync local from shared before finding monster */
            sync_dungeon_from_shared(d, sd);
        }
    }

    /* Find monster */
    MonsterInfo* target = NULL;
    for (int i = 0; i < d->monster_count; i++) {
        if (d->monsters[i].monster_id == monster_id && d->monsters[i].alive) {
            target = &d->monsters[i];
            break;
        }
    }

    if (!target) {
        printf("[DUNGEON] Monster %u not found or dead\n", monster_id);
        uint8_t error[4] = {2, 0, 0, 0};  /* Monster not found */
        return send_response(s, 65, error, 4);
    }

    /* Get character info for skill calculation */
    uint8_t job = 0;
    uint8_t char_level = 1;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            job = s->characters[i].job;
            char_level = s->characters[i].level;
            break;
        }
    }

    /* Calculate monster effective level based on difficulty and room */
    uint8_t monster_level = (uint8_t)(10 + d->difficulty * 15 + d->current_room * 3);
    if (monster_level > 99) monster_level = 99;

    /* Monster defense scales with difficulty and room */
    uint32_t monster_def = 50 + d->difficulty * 30 + d->current_room * 10;

    /* Use v3 damage calculation with full modifiers (Buff/Penetration/Element) */
    uint8_t is_crit = 0;
    uint8_t is_miss = 0;
    uint32_t damage = calculate_skill_damage_v3(s, skill_id, skill_level,
                                                 target->monster_type, monster_level,
                                                 monster_def, &is_crit, &is_miss);

    /* Handle miss */
    if (is_miss) {
        printf("[DUNGEON] Skill %u MISSED! (hit_rate too low vs monster lv%u)\n",
               skill_id, monster_level);
        uint8_t miss_resp[8] = {0, 0, 0, 0, 0, 0, 0, 1};  /* damage=0, miss=1 */
        memcpy(miss_resp, &monster_id, 4);
        return send_response(s, 65, miss_resp, 8);
    }

    /* Get hit count for multi-hit skills */
    uint8_t hit_count = get_skill_hit_count(skill_id);
    uint32_t total_damage = damage * hit_count;

    /* Apply lifesteal from Buff */
    int32_t lifesteal_pct = buff_get_stat_modifier(s, BUFF_LIFESTEAL);
    if (lifesteal_pct > 0 && total_damage > 0) {
        uint32_t heal = total_damage * lifesteal_pct / 100;
        if (heal > 0) {
            s->current_hp += heal;
            if (s->current_hp > s->combat_stats.max_hp) {
                s->current_hp = s->combat_stats.max_hp;
            }
            printf("[DUNGEON] Lifesteal: +%u HP (now %u/%u)\n",
                   heal, s->current_hp, s->combat_stats.max_hp);
        }
    }

    printf("[DUNGEON] Skill %u (lv%d) -> Monster %u (type=%u, lv=%u): %u dmg x %d hits = %u%s\n",
           skill_id, skill_level, monster_id, target->monster_type, monster_level,
           damage, hit_count, total_damage, is_crit ? " CRIT!" : "");

    /* Apply damage with hit stun */
    uint8_t killed = 0;
    if (total_damage >= target->hp) {
        target->hp = 0;
        target->alive = 0;
        target->state = MONSTER_STATE_DEAD;
        killed = 1;
        d->monsters_killed++;
        printf("[DUNGEON] Monster %u killed! Total kills: %d\n", monster_id, d->monsters_killed);
    } else {
        target->hp -= total_damage;
        /* Apply hit stun based on damage dealt */
        uint8_t stun_frames = (uint8_t)((total_damage / 100) + 3);
        if (stun_frames > 30) stun_frames = 30;
        monster_apply_hit_stun(target, stun_frames);
        printf("[DUNGEON] Monster %u took %u damage, HP: %u/%u, stun: %d frames\n",
               monster_id, total_damage, target->hp, target->max_hp, stun_frames);
    }

    /* Check if room is cleared */
    uint8_t room_cleared = dungeon_room_cleared(d) ? 1 : 0;

    /* Build response */
    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &monster_id, 4);
    offset += 4;

    memcpy(response + offset, &total_damage, 4);
    offset += 4;

    memcpy(response + offset, &target->hp, 4);
    offset += 4;

    response[offset++] = killed;
    response[offset++] = is_crit;
    response[offset++] = hit_count;
    response[offset++] = room_cleared;

    /* Add attacker ID for party sync */
    memcpy(response + offset, &s->character_id, 4);
    offset += 4;

    /* For party dungeons, sync state and broadcast to other members */
    if (sd) {
        /* Sync shared from local (after damage applied) */
        sync_shared_from_dungeon(sd, d);

        /* Broadcast damage update to other party members (msg_no=69: PARTY_DUNGEON_ATTACK_SYNC) */
        shared_dungeon_broadcast_except(sd, s->fd, 69, response, offset);

        /* Sync updated monster state to all party sessions */
        for (int i = 0; i < (int)sd->member_count; i++) {
            int member_fd = (int)sd->member_fds[i];
            if (member_fd == s->fd) continue;  /* Skip attacker */

            for (int j = 0; j < MAX_SESSIONS; j++) {
                if (g_sessions[j].active && g_sessions[j].fd == member_fd) {
                    /* Sync their local dungeon from shared */
                    sync_dungeon_from_shared(&g_sessions[j].dungeon, sd);
                    break;
                }
            }
        }

        printf("[PARTY_DUNGEON] Synced attack to %d other members\n", sd->member_count - 1);
    }

    return send_response(s, 65, response, offset);
}

/* msg_no=66: DUNGEON_NEXT_ROOM (move to next room) */
int handle_dungeon_next_room(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_NEXT_ROOM from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 66, error, 4);
    }

    DungeonInstance* d = &s->dungeon;
    SharedDungeonInstance* sd = NULL;

    /* For party dungeons, sync from shared instance first */
    if (d->party_id > 0 && d->shared_dungeon_id > 0) {
        sd = find_shared_dungeon(d->shared_dungeon_id);
        if (sd) {
            sync_dungeon_from_shared(d, sd);
        }
    }

    /* Check if current room is cleared */
    if (!dungeon_room_cleared(d)) {
        printf("[DUNGEON] Room not cleared yet\n");
        uint8_t error[4] = {2, 0, 0, 0};  /* Room not cleared */
        return send_response(s, 66, error, 4);
    }

    /* Check if this was the last room */
    if (d->current_room >= d->total_rooms - 1) {
        printf("[DUNGEON] Already at final room\n");
        uint8_t error[4] = {3, 0, 0, 0};  /* Already at final room */
        return send_response(s, 66, error, 4);
    }

    /* For party dungeons, only leader can advance room */
    if (sd && !d->is_party_host) {
        printf("[PARTY_DUNGEON] Only party leader can advance room\n");
        uint8_t error[4] = {4, 0, 0, 0};  /* Not party leader */
        return send_response(s, 66, error, 4);
    }

    /* Move to next room */
    d->current_room++;

    /* For party dungeons, spawn on shared instance and sync */
    if (sd) {
        sd->current_room = d->current_room;
        shared_dungeon_spawn_monsters(sd, sd->current_room);
        sync_dungeon_from_shared(d, sd);

        printf("[PARTY_DUNGEON] Advanced to room %d/%d with %d monsters\n",
               sd->current_room, sd->total_rooms, sd->monster_count);
    } else {
        dungeon_spawn_monsters(d, d->current_room);
        printf("[DUNGEON] Moved to room %d/%d with %d monsters\n",
               d->current_room, d->total_rooms, d->monster_count);
    }

    /* Build response */
    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = d->current_room;
    response[offset++] = d->total_rooms;

    uint8_t mon_count = (uint8_t)d->monster_count;
    response[offset++] = mon_count;

    /* Monster list */
    for (int i = 0; i < d->monster_count && offset < 240; i++) {
        MonsterInfo* m = &d->monsters[i];
        memcpy(response + offset, &m->monster_id, 4);
        offset += 4;
        memcpy(response + offset, &m->monster_type, 2);
        offset += 2;
        memcpy(response + offset, &m->x, 2);
        offset += 2;
        memcpy(response + offset, &m->y, 2);
        offset += 2;
        memcpy(response + offset, &m->hp, 4);
        offset += 4;
    }

    /* For party dungeons, broadcast room change to all members and sync their state */
    if (sd) {
        /* Broadcast to other members (msg_no=70 but with result=100 to indicate room change) */
        uint8_t room_notify[8];
        memset(room_notify, 0, sizeof(room_notify));
        room_notify[0] = d->current_room;
        room_notify[1] = d->total_rooms;
        room_notify[2] = mon_count;
        shared_dungeon_broadcast_except(sd, s->fd, 67, room_notify, 8);  /* msg_no=67: PARTY_DUNGEON_ROOM_CHANGE */

        /* Sync all party members */
        for (int i = 0; i < (int)sd->member_count; i++) {
            int member_fd = (int)sd->member_fds[i];
            if (member_fd == s->fd) continue;

            for (int j = 0; j < MAX_SESSIONS; j++) {
                if (g_sessions[j].active && g_sessions[j].fd == member_fd) {
                    sync_dungeon_from_shared(&g_sessions[j].dungeon, sd);
                    /* Send same response to party members */
                    send_response(&g_sessions[j], 66, response, offset);
                    break;
                }
            }
        }
    }

    return send_response(s, 66, response, offset);
}

/* msg_no=70: DUNGEON_CLEAR (clear final room) */
int handle_dungeon_clear(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_CLEAR from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 70, error, 4);
    }

    DungeonInstance* d = &s->dungeon;

    /* Check if final room and cleared */
    if (d->current_room < d->total_rooms - 1) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Not at final room */
        return send_response(s, 70, error, 4);
    }

    if (!dungeon_room_cleared(d)) {
        uint8_t error[4] = {3, 0, 0, 0};  /* Room not cleared */
        return send_response(s, 70, error, 4);
    }

    /* Calculate clear time */
    uint32_t clear_time = (uint32_t)time(NULL) - d->start_time;

    /* Calculate rank (S, A, B, C, D based on time and difficulty) */
    uint8_t rank;
    uint32_t base_time = 180 * (d->difficulty + 1);  /* Base time for S rank */
    if (clear_time <= base_time) {
        rank = 0;  /* S */
    } else if (clear_time <= base_time * 1.5) {
        rank = 1;  /* A */
    } else if (clear_time <= base_time * 2) {
        rank = 2;  /* B */
    } else if (clear_time <= base_time * 3) {
        rank = 3;  /* C */
    } else {
        rank = 4;  /* D */
    }

    printf("[DUNGEON] Cleared! Time: %u sec, Rank: %c, Kills: %d\n",
           clear_time, "SABCD"[rank], d->monsters_killed);

    /* Build response */
    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &clear_time, 4);
    offset += 4;

    response[offset++] = rank;

    memcpy(response + offset, &d->monsters_killed, 4);
    offset += 4;

    /* Base rewards (scaled by rank and difficulty) */
    uint32_t exp_reward = 10000 * (d->difficulty + 1) * (5 - rank);
    uint32_t gold_reward = 5000 * (d->difficulty + 1) * (5 - rank);

    memcpy(response + offset, &exp_reward, 4);
    offset += 4;

    memcpy(response + offset, &gold_reward, 4);
    offset += 4;

    /* Persist rewards to database */
    printf("[DUNGEON] Persisting rewards: EXP=%u, Gold=%u\n", exp_reward, gold_reward);

    /* Add EXP to database */
    if (db_add_exp(s->character_id, exp_reward) >= 0) {
        printf("[DUNGEON] EXP persisted to DB\n");
        /* Update in-memory too */
        for (int i = 0; i < s->character_count; i++) {
            if (s->characters[i].charac_no == s->character_id) {
                s->characters[i].exp += exp_reward;
                break;
            }
        }
    }

    /* P2: Check for level up after gaining exp */
    int levels_gained = check_and_level_up(s);
    if (levels_gained > 0) {
        printf("[DUNGEON] Gained %d levels from dungeon clear!\n", levels_gained);
    }

    /* Add gold to database */
    if (db_add_gold(s->character_id, gold_reward) >= 0) {
        printf("[DUNGEON] Gold persisted to DB\n");
        /* Update in-memory too */
        s->inventory.money += gold_reward;
    }

    /* P3: Decrease equipment durability after dungeon */
    int durability_loss = 2 + d->difficulty;  /* Higher difficulty = more wear */
    decrease_equipment_durability(s, durability_loss);
    save_equipment_durability(s);
    printf("[DUNGEON] Equipment durability decreased by %d\n", durability_loss);

    /* Phase 9: Add guild experience on dungeon clear */
    if (s->guild_id > 0) {
        /* Guild exp = base * difficulty * rank bonus */
        /* Base: 100 exp per dungeon, +50 per difficulty level */
        /* Rank bonus: S=2.0x, A=1.5x, B=1.2x, C=1.0x, D=0.8x */
        uint32_t guild_exp_base = 100 + d->difficulty * 50;
        float rank_mult[5] = {2.0f, 1.5f, 1.2f, 1.0f, 0.8f};
        uint32_t guild_exp = (uint32_t)(guild_exp_base * rank_mult[rank]);

        uint8_t new_level = 0, leveled_up = 0;
        if (guild_ipc_add_exp(s->guild_id, guild_exp, &new_level, &leveled_up) == 0) {
            printf("[DUNGEON] Guild %u gained %u exp (level=%u, leveled_up=%u)\n",
                   s->guild_id, guild_exp, new_level, leveled_up);
        }
    }

    /* Phase 12: Log boss kill if boss was defeated */
    uint32_t boss_id = (d->dungeon_id % 3) + 1;  /* Same logic as spawning */
    uint32_t kill_count = 0, best_time = 0;
    db_get_boss_kill_stats(s->character_id, boss_id, &kill_count, &best_time);
    uint8_t is_first_kill = (kill_count == 0) ? 1 : 0;
    db_log_boss_kill(s->character_id, boss_id, d->dungeon_id, d->difficulty,
                     clear_time, is_first_kill);
    if (is_first_kill) {
        printf("[BOSS] FIRST KILL! Boss %u defeated by charac=%u\n", boss_id, s->character_id);
    }

    /* Phase 12: Achievement triggers on dungeon clear */
    {
        int ach_completed = 0;
        /* Monster kill achievement */
        ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_KILL_MONSTER,
                                                     d->monsters_killed);
        /* Dungeon clear achievement - need to track cumulative */
        /* For now, count from db */
        char ach_query[256];
        snprintf(ach_query, sizeof(ach_query),
                 "SELECT COUNT(*) FROM boss_kill_log WHERE charac_no=%u", s->character_id);
        if (g_db_charac && mysql_query(g_db_charac, ach_query) == 0) {
            MYSQL_RES* ach_result = mysql_store_result(g_db_charac);
            if (ach_result) {
                MYSQL_ROW ach_row = mysql_fetch_row(ach_result);
                if (ach_row) {
                    uint32_t dungeon_clears = (uint32_t)atoi(ach_row[0]);
                    ach_completed += check_achievement_progress(s->character_id,
                                                                 ACH_TYPE_CLEAR_DUNGEON,
                                                                 dungeon_clears);
                }
                mysql_free_result(ach_result);
            }
        }
        /* Boss kill achievement */
        ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_KILL_BOSS,
                                                     kill_count + 1);
        /* First boss kill achievement */
        if (is_first_kill) {
            ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_FIRST_BOSS,
                                                         boss_id);
        }
        /* Speed clear achievement (target = seconds, lower is better) */
        if (clear_time <= 180) {
            ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_SPEED_CLEAR,
                                                         clear_time);
        }
        /* No death clear - if player never died in this dungeon */
        if (d->death_count == 0) {
            /* Count no-death clears from session stat */
            s->stat_no_death_clears++;
            ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_NO_DEATH,
                                                         s->stat_no_death_clears);
        }
        /* Party clear achievement */
        if (s->party_id > 0) {
            s->stat_party_clears++;
            ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_PARTY_CLEAR,
                                                         s->stat_party_clears);
        }
        if (ach_completed > 0) {
            printf("[ACH] Dungeon clear triggered %d achievement completions\n", ach_completed);
        }
    }

    /* Drop a random item as reward (simple: item_id based on dungeon_id + difficulty) */
    uint32_t drop_item_id = 60000 + d->dungeon_id * 100 + d->difficulty * 10 + (rank < 3 ? 1 : 0);
    int drop_slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
    if (drop_slot > 0 && drop_slot < 200) {
        int new_id = db_add_item(s->character_id, drop_item_id, drop_slot, 0);
        if (new_id > 0) {
            printf("[DUNGEON] Item drop persisted: item_id=%u, slot=%d\n", drop_item_id, drop_slot);
        }
    }

    /* Mark dungeon as complete */
    d->active = 0;
    s->state = STATE_IN_GAME;

    return send_response(s, 70, response, offset);
}


/* msg_no=71: DUNGEON_REWARD (claim additional rewards with random attributes) */
int handle_dungeon_reward(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DUNGEON_REWARD from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 71, error, 4);
    }

    uint32_t reward_type;
    memcpy(&reward_type, data, 4);

    printf("[DUNGEON] Claiming reward type: %u\n", reward_type);

    /* Determine item rarity based on reward_type and RNG */
    uint8_t item_rarity = RARITY_NORMAL;
    uint32_t rarity_roll = (uint32_t)(rand() % 10000);
    if (rarity_roll < 100) {
        item_rarity = RARITY_LEGENDARY;  /* 1% */
    } else if (rarity_roll < 500) {
        item_rarity = RARITY_EPIC;       /* 4% */
    } else if (rarity_roll < 1500) {
        item_rarity = RARITY_RARE;       /* 10% */
    } else if (rarity_roll < 4000) {
        item_rarity = RARITY_MAGIC;      /* 25% */
    }

    /* Boost rarity for higher reward types */
    if (reward_type >= 5 && item_rarity < RARITY_EPIC) {
        item_rarity++;  /* Better rewards for harder content */
    }

    /* Generate item ID: base equipment (2000-2100) based on reward type */
    uint32_t item_id = 2000 + (reward_type % 20) * 5;
    uint16_t item_count = 1;

    /* Item level based on dungeon difficulty (stored in session) */
    uint8_t item_level = s->dungeon.difficulty * 10 + 15;
    if (item_level > 85) item_level = 85;

    /* Find free slot */
    int slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
    if (slot < 0 || slot >= 200) {
        uint8_t error[4] = {4, 0, 0, 0};  /* Inventory full */
        return send_response(s, 71, error, 4);
    }

    /* Add item with random attributes */
    int new_ui_id = db_add_item_with_attrs(s->character_id, item_id, slot, item_rarity, item_level);
    if (new_ui_id <= 0) {
        /* Fallback to basic item without attributes */
        new_ui_id = db_add_item(s->character_id, item_id, slot, item_rarity);
        if (new_ui_id <= 0) {
            uint8_t error[4] = {5, 0, 0, 0};  /* DB error */
            return send_response(s, 71, error, 4);
        }
    }

    printf("[DUNGEON] Reward item: item_id=%u, rarity=%u, level=%u, slot=%d, ui_id=%d\n",
           item_id, item_rarity, item_level, slot, new_ui_id);

    /* Build response with rarity info */
    uint8_t response[40];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &item_id, 4);
    offset += 4;
    memcpy(response + offset, &item_count, 2);
    offset += 2;

    uint16_t slot16 = (uint16_t)slot;
    memcpy(response + offset, &slot16, 2);
    offset += 2;

    /* New: include rarity and level for client display */
    response[offset++] = item_rarity;
    response[offset++] = item_level;

    return send_response(s, 71, response, offset);
}

/* msg_no=220: DUNGEON_MONSTER_TICK - Client sends player position, server updates monsters */
int handle_dungeon_monster_tick(ClientSession* s, const uint8_t* data, int len) {
    if (s->state != STATE_IN_DUNGEON || len < 4) {
        return 0;  /* Silent fail for tick messages */
    }

    /* Parse player position from client */
    uint16_t player_x, player_y;
    memcpy(&player_x, data, 2);
    memcpy(&player_y, data + 2, 2);

    DungeonInstance* d = &s->dungeon;
    if (!d->active) return 0;

    /* P3: Process buff updates every tick */
    static uint32_t last_buff_tick = 0;
    uint32_t now = (uint32_t)time(NULL) * 1000;
    uint32_t delta_ms = (last_buff_tick > 0) ? (now - last_buff_tick) : 100;
    if (delta_ms > 1000) delta_ms = 1000;  /* Cap at 1 second */
    last_buff_tick = now;

    /* Update and expire buffs */
    buff_update(s);

    /* Process DOT/HOT effects (poison damage, regen healing, etc.) */
    buff_process_dots_and_hots(s, delta_ms);

    /* Recalculate buff stat modifiers */
    buff_apply_stat_modifiers(s);

    /* Use enhanced AI v2 with skill selection and debuffs */
    DungeonTickResult tick = dungeon_tick_monsters_v2(d, s, player_x, player_y);

    /* Apply total damage to player if attacked */
    uint8_t player_died = 0;
    uint32_t actual_damage = 0;
    if (tick.total_damage > 0) {
        actual_damage = (uint32_t)player_take_damage(s, d, tick.total_damage, &player_died);
        printf("[DUNGEON] Monster attacks hit player! attacks=%d raw=%u actual=%u hp=%u\n",
               tick.attacks_count, tick.total_damage, actual_damage, s->current_hp);
    }

    /* Check if player died from DOT effects */
    if (!player_died && s->current_hp == 0) {
        player_die(s);
        player_died = 1;
    }

    /* Build response: monster positions + player status */
    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result code: 0=OK, 1=player died */
    uint32_t result = player_died ? 1 : 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Player HP (4) + debuff_type (2) + debuff_duration (2) = 8 bytes */
    memcpy(response + offset, &s->current_hp, 4);
    offset += 4;
    memcpy(response + offset, &tick.debuff_type, 2);
    offset += 2;
    memcpy(response + offset, &tick.debuff_duration, 2);
    offset += 2;

    /* Monster count */
    response[offset++] = (uint8_t)d->monster_count;

    /* Monster positions: id(4) + x(2) + y(2) + state(1) + hp(4) = 13 bytes */
    for (int i = 0; i < d->monster_count && offset < 220; i++) {
        MonsterInfo* m = &d->monsters[i];
        memcpy(response + offset, &m->monster_id, 4);
        offset += 4;
        memcpy(response + offset, &m->x, 2);
        offset += 2;
        memcpy(response + offset, &m->y, 2);
        offset += 2;
        response[offset++] = m->state;
        memcpy(response + offset, &m->hp, 4);
        offset += 4;
    }

    /* Attack summary: count(1) + total_damage(4) = 5 bytes */
    response[offset++] = (uint8_t)tick.attacks_count;
    memcpy(response + offset, &actual_damage, 4);
    offset += 4;

    /* P5: Party dungeon sync - broadcast to all party members */
    if (s->party_id > 0) {
        SharedDungeonInstance* sd = find_shared_dungeon_by_party(s->party_id);
        if (sd && sd->member_count > 1) {
            /* Build party sync packet with sender's info */
            uint8_t sync_pkt[280];
            memset(sync_pkt, 0, sizeof(sync_pkt));
            int sync_offset = 0;

            /* Add sender character ID and position */
            memcpy(sync_pkt + sync_offset, &s->character_id, 4);
            sync_offset += 4;
            memcpy(sync_pkt + sync_offset, &player_x, 2);
            sync_offset += 2;
            memcpy(sync_pkt + sync_offset, &player_y, 2);
            sync_offset += 2;
            memcpy(sync_pkt + sync_offset, &s->current_hp, 4);
            sync_offset += 4;

            /* Copy monster data from response */
            memcpy(sync_pkt + sync_offset, response, offset);
            sync_offset += offset;

            /* Broadcast to all party members except sender */
            for (int i = 0; i < (int)sd->member_count; i++) {
                int fd = (int)sd->member_fds[i];
                if (fd == s->fd) continue;  /* Skip sender */
                for (int j = 0; j < MAX_SESSIONS; j++) {
                    if (g_sessions[j].active && g_sessions[j].fd == fd) {
                        send_response(&g_sessions[j], 215, sync_pkt, sync_offset);  /* PARTY_DUNGEON_SYNC */
                        break;
                    }
                }
            }
        }
    }

    return send_response(s, 214, response, offset);
}

/* msg_no=221: DUNGEON_PLAYER_HIT - Acknowledge player hit by monster */
int handle_dungeon_player_hit(ClientSession* s, const uint8_t* data, int len) {
    if (s->state != STATE_IN_DUNGEON || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 215, error, 4);
    }
    
    uint32_t monster_id;
    uint8_t monster_idx;
    memcpy(&monster_id, data, 4);
    monster_idx = data[4];
    
    DungeonInstance* d = &s->dungeon;
    if (!d->active || monster_idx >= d->monster_count) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 215, error, 4);
    }
    
    MonsterInfo* m = &d->monsters[monster_idx];
    if (!m->alive || m->monster_id != monster_id) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 215, error, 4);
    }
    

    /* Calculate actual damage with defense reduction */
    uint32_t raw_damage = m->attack_damage;
    uint32_t player_def = s->combat_stats.phys_def;

    /* Defense formula: damage = raw * (100 / (100 + def)) */
    uint32_t actual_damage = raw_damage * 100 / (100 + player_def);
    if (actual_damage < 1) actual_damage = 1;

    /* Check for damage reduction buffs */
    int32_t def_mod = buff_get_stat_modifier(s, BUFF_DEFENSE_UP);
    if (def_mod > 0) {
        actual_damage = actual_damage * 100 / (100 + def_mod);
    }

    /* Check for invincibility buff */
    if (buff_has(s, BUFF_INVINCIBLE)) {
        actual_damage = 0;
        printf("[DUNGEON] Player is invincible, damage blocked!\n");
    }

    /* Apply damage to player HP */
    uint32_t old_hp = s->current_hp;
    uint8_t player_died = 0;

    if (actual_damage >= s->current_hp) {
        s->current_hp = 0;
        player_died = 1;
        printf("[DUNGEON] Player KILLED by monster %u! Damage: %u, Old HP: %u\n",
               monster_id, actual_damage, old_hp);
    } else {
        s->current_hp -= actual_damage;
        printf("[DUNGEON] Player hit by monster %u for %u damage (raw: %u), HP: %u -> %u\n",
               monster_id, actual_damage, raw_damage, old_hp, s->current_hp);
    }

    /* Build response */
    uint8_t response[20];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &monster_id, 4);
    offset += 4;
    memcpy(response + offset, &actual_damage, 4);
    offset += 4;
    memcpy(response + offset, &s->current_hp, 4);
    offset += 4;
    response[offset++] = player_died;

    return send_response(s, 215, response, offset);
}

