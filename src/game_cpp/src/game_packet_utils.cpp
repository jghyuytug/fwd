#include <string.h>

#include "game_packet_utils.h"
#include "game_server_types.h"

/*==============================================================================
 * Packet Building Helpers
 *============================================================================*/

/* Build CHARACTER_STATUS response */
int build_character_status(uint8_t* buffer, int max_len, const DBCharacterInfo* ch) {
    if (!buffer || !ch || max_len < 128) return -1;

    int offset = 0;

    /* Result code (4 bytes for consistency with test client) */
    uint32_t result = 0;
    memcpy(buffer + offset, &result, 4);
    offset += 4;

    memcpy(buffer + offset, &ch->charac_no, 4);
    offset += 4;
    memset(buffer + offset, 0, MAX_CHARAC_NAME);
    strncpy((char*)(buffer + offset), ch->charac_name, MAX_CHARAC_NAME - 1);
    offset += MAX_CHARAC_NAME;
    buffer[offset++] = ch->job;
    buffer[offset++] = ch->grow_type;
    buffer[offset++] = ch->level;
    memcpy(buffer + offset, &ch->exp, 4);
    offset += 4;
    memcpy(buffer + offset, &ch->hp, 2);
    offset += 2;
    memcpy(buffer + offset, &ch->max_hp, 2);
    offset += 2;
    uint16_t mp = 30000;  /* Default MP */
    memcpy(buffer + offset, &mp, 2);
    offset += 2;
    memcpy(buffer + offset, &ch->max_mp, 2);
    offset += 2;
    uint32_t gold = 999999999;
    memcpy(buffer + offset, &gold, 4);
    offset += 4;
    memcpy(buffer + offset, &ch->village, 4);
    offset += 4;
    int32_t fatigue32 = ch->fatigue;
    memcpy(buffer + offset, &fatigue32, 4);
    offset += 4;

    return offset;
}

