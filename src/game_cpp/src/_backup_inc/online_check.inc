/* Check if a character is currently online */
static int is_character_online(uint32_t charac_no) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active &&
            g_sessions[i].state == STATE_INGAME &&
            g_sessions[i].character_id == charac_no) {
            return 1;
        }
    }
    return 0;
}

/* Get online status for multiple characters */
static void update_online_status(uint32_t* charac_nos, uint8_t* online_status, int count) {
    for (int i = 0; i < count; i++) {
        online_status[i] = (uint8_t)is_character_online(charac_nos[i]);
    }
}

