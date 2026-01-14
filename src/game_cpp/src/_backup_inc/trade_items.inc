        /* Transfer items from self to partner */
        for (int i = 0; i < g_trades[idx].item_count && i < 10; i++) {
            uint32_t item_uid = g_trades[idx].items[i];
            if (item_uid > 0) {
                int new_slot = db_transfer_item(item_uid, s->character_id, partner->character_id);
                if (new_slot >= 0) {
                    printf("[TRADE] Item %u transferred from %s to %s (slot=%d)\n",
                           item_uid, s->character_name, partner->character_name, new_slot);
                } else {
                    printf("[TRADE] Item %u transfer FAILED from %s to %s\n",
                           item_uid, s->character_name, partner->character_name);
                }
            }
        }

        /* Transfer items from partner to self */
        for (int i = 0; i < g_trades[partner_idx].item_count && i < 10; i++) {
            uint32_t item_uid = g_trades[partner_idx].items[i];
            if (item_uid > 0) {
                int new_slot = db_transfer_item(item_uid, partner->character_id, s->character_id);
                if (new_slot >= 0) {
                    printf("[TRADE] Item %u transferred from %s to %s (slot=%d)\n",
                           item_uid, partner->character_name, s->character_name, new_slot);
                } else {
                    printf("[TRADE] Item %u transfer FAILED from %s to %s\n",
                           item_uid, partner->character_name, s->character_name);
                }
            }
        }
