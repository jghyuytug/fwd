    /* Add item to player inventory if present */
    if (att.item_id > 0) {
        int free_slot = db_find_free_slot(s->character_id, 11);  /* Bag slots start at 11 */
        if (free_slot >= 0 && free_slot <= 200) {
            int new_ui_id = db_add_item(s->character_id, att.item_id, free_slot, 0);
            if (new_ui_id > 0) {
                printf("[MAIL] Added item_id=%u to slot=%d (new ui_id=%d) from mail %u\n",
                       att.item_id, free_slot, new_ui_id, mail_id);
            } else {
                printf("[MAIL] Failed to add item_id=%u from mail %u\n", att.item_id, mail_id);
            }
        } else {
            printf("[MAIL] No free inventory slot for item from mail %u\n", mail_id);
        }
    }
