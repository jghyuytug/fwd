#include "handlers/handler_dispatch.h"

#include "handlers/handler_login.h"
#include "handlers/handler_inventory.h"
#include "handlers/handler_prototypes.h"

extern int handle_default(ClientSession* s, uint8_t msg_no, const uint8_t* data, int len);

int dispatch_message(ClientSession* s, uint16_t msg_no, const uint8_t* data, int len) {
    const int data_len = len;
    switch (msg_no) {

        /* ========== Core Connection (0-8) ========== */
        case 0:  return handle_check_connection(s, data, data_len);  /* CHECK_CONNECTION */
        case 1:  return handle_login(s, data, data_len);             /* LOGIN */
        case 2:  return handle_set_udp(s, data, data_len);           /* SET_UDP_IP_PORT */
        case 3:  return handle_exit(s, data, data_len);              /* EXIT */
        case 4:  return handle_select_character(s, data, data_len);  /* SELECT_CHARACTER */
        case 5:  return handle_create_character(s, data, data_len);  /* CREATE_CHARACTER */
        case 6:  return handle_delete_character(s, data, data_len);  /* DELETE_CHARACTER */
        case 7:  return handle_return_select(s, data, data_len);     /* RETURN_SELECT_CHARACTER */
        case 8:  return handle_get_userinfo(s, data, data_len);      /* GET_USERINFO */
        /* 9: RECOVER_STAMINA - not implemented */

        /* ========== P2P & Party (10-14) ========== */
        /* 10: REQUEST_PEER - P2P related */
        /* 11: RESPONSE_PEER - P2P related */
        case 12: return handle_party_create(s, data, data_len);      /* SET_PARTY_INFO - reuse as create */
        case 13: return handle_party_leave(s, data, data_len);       /* LEAVE_PARTY */
        case 14: return handle_party_kick(s, data, data_len);        /* WALKOUT_PARTY_MEMBER */

        /* ========== Game Start & Dungeon (15-16) ========== */
        case 15: return handle_start_game(s, data, data_len);        /* START_GAME */
        case 16: return handle_dungeon_enter(s, data, data_len);     /* SELECT_DUNGEON - enter dungeon */

        /* ========== Chat (17) ========== */
        case 17: return handle_chat(s, data, data_len);              /* SEND_MESSAGE */

        /* ========== Item Operations (18-29) ========== */
        case 18: return handle_delete_item(s, data, data_len);       /* DELETE_ITEM */
        case 19: return handle_move_item(s, data, data_len);         /* MOVE_ITEMSPACE */
        case 20: return handle_sort_item(s, data, data_len);         /* SORT_ITEM */
        case 21: return handle_npc_shop_buy(s, data, data_len);      /* BUY_ITEM (NPC shop) */
        /* 22: RENT_AVATAR - not implemented */
        /* 23: EXTEND_AVATAR - not implemented */
        case 24: return handle_npc_shop_sell(s, data, data_len);     /* SELL_ITEM */
        case 25: return handle_repair_equipment(s, data, data_len);  /* REPAIR_EQUIPMENT */
        case 26: return handle_trade_request(s, data, data_len);     /* SET_ITEMTRADE_STATE */
        case 27: return handle_compound_item(s, data, data_len);     /* COMPOUND_ITEM */
        case 28: return handle_disjoint_item(s, data, data_len);     /* DISJOINT_ITEM */
        /* 29: USE_LOTTERY_ITEM - not implemented */

        /* ========== Skills & Stats (30-36) ========== */
        case 30: return handle_change_skillslot(s, data, data_len);  /* CHANGE_SKILLSLOT */
        case 31: return handle_buy_skill(s, data, data_len);         /* BUY_SKILL */
        case 32: return handle_increase_status(s, data, data_len);   /* INCREASE_STATUS */
        case 33: return handle_accept_quest(s, data, data_len);      /* ACCEPT_QUEST */
        case 34: return handle_giveup_quest(s, data, data_len);      /* GIVEUP_QUEST */
        case 35: return handle_set_quest_trigger(s, data, data_len); /* SET_QUEST_TRIGGER */
        case 36: return handle_complete_quest(s, data, data_len);    /* FINISH_QUEST */

        /* ========== Movement & Area (37-40) ========== */
        case 37: return handle_move(s, data, data_len);              /* SET_USER_POSITION */
        case 38: return handle_move_area(s, data, data_len);         /* SET_USER_AREA */
        /* 39: FISHING - not implemented */
        case 40: return handle_finish_loading(s, data, data_len);    /* FINISH_LOADING */

        /* ========== Combat & Items (41-51) ========== */
        case 41: return handle_use_skill(s, data, data_len);         /* USE_SKILL */
        case 42: return handle_die_monster(s, data, data_len);       /* DIE_MONSTER */
        case 43: return handle_die_character(s, data, data_len);     /* DIE_CHARACTER */
        case 44: return handle_use_coin(s, data, data_len);          /* USE_COIN (resurrection) */
        case 45: return handle_giveup_game(s, data, data_len);       /* GIVEUP_GAME */
        case 46: return handle_get_item(s, data, data_len);          /* GET_ITEM (pickup) */
        case 47: return handle_use_item(s, data, data_len);          /* USE_STACKABLE */
        case 48: return handle_dungeon_next_room(s, data, data_len); /* MOVE_MAP (next room) */
        case 49: return handle_dungeon_clear(s, data, data_len);     /* SET_PLAY_RESULT */
        case 50: return handle_drop_item(s, data, data_len);         /* DROP_ITEM */
        /* 51: DECREASE_DURABILITY - handled server-side */

        /* ========== PVP (53-62) ========== */
        case 53: return handle_pvp_create_room(s, data, data_len);   /* MAKE_PVP_ROOM */
        case 54: return handle_pvp_enter_room(s, data, data_len);    /* ENTER_PVP_ROOM */
        case 55: return handle_pvp_set_seat(s, data, data_len);      /* SET_PVP_SEAT_STATE */
        case 56: return handle_pvp_ready(s, data, data_len);         /* SET_PVP_READY_STATE */
        /* 57-62: Additional PVP - not yet implemented */

        /* ========== Social - Friends (63-64) ========== */
        case 63: return handle_friend_add(s, data, data_len);        /* ADD_FRIEND */
        case 64: return handle_friend_remove(s, data, data_len);     /* REMOVE_FRIEND */

        /* ========== Debug & CERA (65-69) ========== */
        case 65: return handle_debug_command(s, data, data_len);     /* DEBUG_COMMAND */
        /* 66-69: CERA shop - not implemented */

        /* ========== Guild (70-82) ========== */
        case 70: return handle_guild_get_members(s, data, data_len); /* GUILD_MEMER_LIST */
        /* 71-76: Guild management - partial */
        case 77: return handle_guild_get_info(s, data, data_len);    /* GUILD_INFO */
        case 78: return handle_guild_join(s, data, data_len);        /* REQUEST_GUILD_ENTER */
        /* 79-82: Guild member management */
        case 81: return handle_guild_leave(s, data, data_len);       /* MEMBER_SECEDE */

        /* ========== Item Enhancement (83-91) ========== */
        case 83: return handle_upgrade_item(s, data, data_len);      /* UPGRADE_ITEM */
        /* 84-91: Private store, etc */

        /* ========== Mail (97-99) ========== */
        case 97: return handle_mail_send(s, data, data_len);         /* MAILBOX_SEND */
        case 98: return handle_mail_take_attachment(s, data, data_len); /* MAILBOX_EXTRACT_ITEM */
        case 99: return handle_mail_get_list(s, data, data_len);     /* MAILBOX_OPEN */

        /* ========== Storage/Cargo (100-101) ========== */
        case 100: return handle_storage_deposit(s, data, data_len);  /* DEPOSIT_MONEY */
        case 101: return handle_storage_withdraw(s, data, data_len); /* WITHDRAW_MONEY */

        /* ========== Extended protocols - keep old numbers for Python test compatibility ========== */
        /* Inventory (get) - custom extension */
        case 200: return handle_get_inventory(s, data, data_len);    /* Custom: GET_INVENTORY */
        case 201: return handle_get_skill_list(s, data, data_len);   /* Custom: GET_SKILL_LIST */
        case 202: return handle_get_equipment(s, data, data_len);    /* Custom: GET_EQUIPMENT */
        case 203: return handle_equip_item(s, data, data_len);       /* Custom: EQUIP_ITEM */
        case 204: return handle_unequip_item(s, data, data_len);     /* Custom: UNEQUIP_ITEM */
        case 205: return handle_get_character_stats(s, data, data_len); /* Custom: GET_STATS */

        /* Dungeon extensions */
        case 210: return handle_dungeon_info(s, data, data_len);     /* Custom: DUNGEON_INFO */
        case 211: return handle_dungeon_exit(s, data, data_len);     /* Custom: DUNGEON_EXIT */
        case 212: return handle_dungeon_attack(s, data, data_len);   /* Custom: DUNGEON_ATTACK */
        case 213: return handle_dungeon_reward(s, data, data_len);   /* Custom: DUNGEON_REWARD */
        case 214: return handle_dungeon_monster_tick(s, data, data_len); /* Custom: MONSTER_TICK */
        case 215: return handle_dungeon_player_hit(s, data, data_len);   /* Custom: PLAYER_HIT */

        /* Party extensions */
        case 220: return handle_party_invite(s, data, data_len);     /* Custom: PARTY_INVITE */
        case 221: return handle_party_join(s, data, data_len);       /* Custom: PARTY_JOIN */
        case 222: return handle_party_get_info(s, data, data_len);   /* Custom: PARTY_GET_INFO */
        case 223: return handle_party_change_leader(s, data, data_len); /* Custom: PARTY_CHANGE_LEADER */
        case 224: return handle_party_set_loot_mode(s, data, data_len); /* Custom: PARTY_SET_LOOT */
        case 225: return handle_party_chat(s, data, data_len);       /* Custom: PARTY_CHAT */

        /* Friend extensions */
        case 230: return handle_friend_get_list(s, data, data_len);  /* Custom: FRIEND_GET_LIST */
        case 231: return handle_friend_accept(s, data, data_len);    /* Custom: FRIEND_ACCEPT */
        case 232: return handle_friend_decline(s, data, data_len);   /* Custom: FRIEND_DECLINE */
        case 233: return handle_friend_block(s, data, data_len);     /* Custom: FRIEND_BLOCK */
        case 234: return handle_friend_unblock(s, data, data_len);   /* Custom: FRIEND_UNBLOCK */
        case 235: return handle_friend_get_blocked_list(s, data, data_len); /* Custom: FRIEND_BLOCKED_LIST */

        /* Guild extensions */
        case 240: return handle_guild_create(s, data, data_len);     /* Custom: GUILD_CREATE */
        case 241: return handle_guild_disband(s, data, data_len);    /* Custom: GUILD_DISBAND */
        case 242: return handle_guild_invite(s, data, data_len);     /* Custom: GUILD_INVITE */
        case 243: return handle_guild_kick(s, data, data_len);       /* Custom: GUILD_KICK */
        case 244: return handle_guild_promote(s, data, data_len);    /* Custom: GUILD_PROMOTE */
        case 245: return handle_guild_demote(s, data, data_len);     /* Custom: GUILD_DEMOTE */
        case 246: return handle_guild_set_notice(s, data, data_len); /* Custom: GUILD_SET_NOTICE */
        case 247: return handle_guild_get_applications(s, data, data_len); /* Custom: GUILD_GET_APPS */
        case 248: return handle_guild_respond_application(s, data, data_len); /* Custom: GUILD_RESPOND_APP */

        /* Mail extensions */
        case 250: return handle_mail_read(s, data, data_len);        /* Custom: MAIL_READ */
        case 251: return handle_mail_delete(s, data, data_len);      /* Custom: MAIL_DELETE */
        case 252: return handle_mail_get_unread_count(s, data, data_len); /* Custom: MAIL_UNREAD */
        case 253: return handle_mail_return(s, data, data_len);      /* Custom: MAIL_RETURN */

        /* Trade extensions */
        case 260: return handle_trade_accept(s, data, data_len);     /* Custom: TRADE_ACCEPT */
        case 261: return handle_trade_decline(s, data, data_len);    /* Custom: TRADE_DECLINE */
        case 262: return handle_trade_add_item(s, data, data_len);   /* Custom: TRADE_ADD_ITEM */
        case 263: return handle_trade_remove_item(s, data, data_len);/* Custom: TRADE_REMOVE_ITEM */
        case 264: return handle_trade_set_gold(s, data, data_len);   /* Custom: TRADE_SET_GOLD */
        case 265: return handle_trade_lock(s, data, data_len);       /* Custom: TRADE_LOCK */
        case 266: return handle_trade_confirm(s, data, data_len);    /* Custom: TRADE_CONFIRM */
        case 267: return handle_trade_cancel(s, data, data_len);     /* Custom: TRADE_CANCEL */

        /* PVP extensions */
        case 270: return handle_pvp_get_info(s, data, data_len);     /* Custom: PVP_GET_INFO */
        case 271: return handle_pvp_queue(s, data, data_len);        /* Custom: PVP_QUEUE */
        case 272: return handle_pvp_cancel_queue(s, data, data_len); /* Custom: PVP_CANCEL_QUEUE */
        case 273: return handle_pvp_accept_match(s, data, data_len); /* Custom: PVP_ACCEPT_MATCH */
        case 274: return handle_pvp_decline_match(s, data, data_len);/* Custom: PVP_DECLINE_MATCH */
        case 275: return handle_pvp_attack(s, data, data_len);       /* Custom: PVP_ATTACK */
        case 276: return handle_pvp_leave(s, data, data_len);        /* Custom: PVP_LEAVE */
        case 277: return handle_pvp_get_ranking(s, data, data_len);  /* Custom: PVP_GET_RANKING */

        /* Ranking extensions */
        case 280: return handle_ranking_get_level(s, data, data_len);
        case 281: return handle_ranking_get_wealth(s, data, data_len);
        case 282: return handle_ranking_get_dungeon(s, data, data_len);
        case 283: return handle_ranking_get_my_rank(s, data, data_len);
        case 284: return handle_ranking_get_guild(s, data, data_len);
        case 285: return handle_ranking_get_achievement(s, data, data_len);

        /* Achievement extensions */
        case 290: return handle_achievement_get_list(s, data, data_len);
        case 291: return handle_achievement_get_detail(s, data, data_len);
        case 292: return handle_achievement_claim_reward(s, data, data_len);
        case 293: return handle_achievement_get_progress(s, data, data_len);
        case 294: return handle_achievement_get_categories(s, data, data_len);
        case 295: return handle_achievement_get_by_category(s, data, data_len);
        case 296: return handle_achievement_get_recent(s, data, data_len);

        /* Title protocols */
        case 160: return handle_title_get_list(s, data, data_len);
        case 161: return handle_title_equip(s, data, data_len);
        case 162: return handle_title_unequip(s, data, data_len);
        case 163: return handle_title_get_detail(s, data, data_len);
        case 164: return handle_title_get_current(s, data, data_len);
        case 165: return handle_title_get_unlockable(s, data, data_len);

        /* Auction protocols */
        case 170: return handle_auction_search(s, data, data_len);
        case 171: return handle_auction_list_item(s, data, data_len);
        case 172: return handle_auction_cancel(s, data, data_len);
        case 173: return handle_auction_buy(s, data, data_len);
        case 174: return handle_auction_get_my_listings(s, data, data_len);
        case 175: return handle_auction_get_history(s, data, data_len);
        case 176: return handle_auction_bid(s, data, data_len);
        case 177: return handle_auction_get_price_history(s, data, data_len);
        case 178: return handle_auction_collect_gold(s, data, data_len);

        /* Storage protocols */
        case 180: return handle_storage_get_list(s, data, data_len);
        case 181: return handle_storage_deposit_item(s, data, data_len);
        case 182: return handle_storage_withdraw_item(s, data, data_len);
        case 183: return handle_storage_deposit_gold(s, data, data_len);
        case 184: return handle_storage_withdraw_gold(s, data, data_len);
        case 185: return handle_storage_expand(s, data, data_len);

        /* Pet protocols */
        case 190: return handle_pet_get_list(s, data, data_len);
        case 191: return handle_pet_summon(s, data, data_len);
        case 192: return handle_pet_unsummon(s, data, data_len);
        case 193: return handle_pet_feed(s, data, data_len);
        case 194: return handle_pet_rename(s, data, data_len);
        case 195: return handle_pet_get_info(s, data, data_len);
        case 196: return handle_pet_release(s, data, data_len);
        case 197: return handle_pet_equip_item(s, data, data_len);
        case 198: return handle_pet_unequip_item(s, data, data_len);

        /* Phase 7 protocols (300-319) */
        case 300: return handle_enhance_equipment(s, data, data_len);   /* P1: 装备强化 */
        case 301: return handle_learn_skill(s, data, data_len);         /* P4: 技能学习 */
        case 302: return handle_reset_skills(s, data, data_len);        /* P4: 技能重置 */
        case 303: return handle_split_item_stack(s, data, data_len);    /* P6: 物品拆分 */
        case 304: return handle_select_dungeon_difficulty(s, data, data_len); /* P7: 副本难度 */
        case 305: return handle_guild_donate(s, data, data_len);        /* P9: 公会捐献 */
        case 306: return handle_validate_equip(s, data, data_len);      /* P5: 装备验证 */

        /* ========== Phase 12/14: Abyss System (350-355) ========== */
        case 350: return handle_abyss_enter(s, data, data_len);        /* 进入深渊 */
        case 351: return handle_abyss_clear(s, data, data_len);        /* 深渊通关 */
        case 352: return handle_abyss_exit(s, data, data_len);         /* 退出深渊 */
        case 353: return handle_abyss_get_info(s, data, data_len);     /* 深渊信息 */
        case 354: return handle_abyss_room_clear(s, data, data_len);   /* 房间清理 */
        case 355: return handle_abyss_get_ranking(s, data, data_len);  /* 深渊排行 */

        /* ========== Phase 14: Daily/Weekly & Events (380-399) ========== */
        case 380: return handle_daily_quest_get(s, data, data_len);   /* 获取日常任务 */
        case 381: return handle_daily_quest_claim(s, data, data_len); /* 领取日常奖励 */
        case 382: return handle_weekly_quest_get(s, data, data_len);  /* 获取周常任务 */
        case 383: return handle_weekly_quest_claim(s, data, data_len);/* 领取周常奖励 */
        case 534: return handle_sign_in_info(s, data, data_len);    /* 签到信息 */
        case 535: return handle_sign_in_do(s, data, data_len);      /* 执行签到 */
        case 536: return handle_sign_in_makeup(s, data, data_len);  /* 补签 */
        case 390: return handle_event_get_list(s, data, data_len);    /* 获取活动列表 */
        case 391: return handle_event_claim(s, data, data_len);       /* 领取活动奖励 */

        /* ========== Phase 13: Advanced Systems (410-459) ========== */
        /* Tower Challenge System */
        case 410: return handle_tower_enter(s, data, data_len);         /* 进入死亡之塔 */
        case 411: return handle_tower_clear_floor(s, data, data_len);   /* 通关楼层 */
        case 412: return handle_tower_get_ranking(s, data, data_len);   /* 获取排行榜 */

        /* Set Bonus System */
        case 420: return handle_set_get_info(s, data, data_len);        /* 获取套装信息 */

        /* Season System */
        case 430: return handle_season_get_info(s, data, data_len);     /* 获取赛季信息 */

        /* Equipment Growth System */
        case 440: return handle_growth_get_info(s, data, data_len);     /* 获取成长信息 */
        case 441: return handle_growth_upgrade(s, data, data_len);      /* 成长升级 */

        /* Mystery Shop System */
        case 450: return handle_mystery_shop_get(s, data, data_len);    /* 获取神秘商店 */
        case 451: return handle_mystery_shop_buy(s, data, data_len);    /* 购买商品 */
        case 452: return handle_mystery_shop_refresh(s, data, data_len);/* 刷新商店 */

        /* Phase 14: Skill Combo System */
        case 460: return handle_combo_skill(s, data, data_len);        /* 连招技能 */
        case 461: return handle_skill_cancel(s, data, data_len);       /* 技能取消 */

        /* Phase 14: Item Usage System */
        case 462: return handle_use_consumable(s, data, data_len);     /* 使用消耗品 */

        /* Phase 14: Map Teleport System */
        case 470: return handle_teleport(s, data, data_len);           /* 传送 */
        case 471: return handle_move_map(s, data, data_len);           /* 切换地图 */

        /* Phase 14: Quest System Enhancement */
        case 472: return handle_quest_trigger(s, data, data_len);      /* 任务触发 */
        case 473: return handle_quest_track(s, data, data_len);        /* 任务追踪 */

        /* Phase 14: Dungeon Matching System */
        case 480: return handle_quick_party_queue(s, data, data_len);  /* 快速组队排队 */
        case 481: return handle_quick_party_cancel(s, data, data_len); /* 取消排队 */
        case 482: return handle_match_status(s, data, data_len);       /* 匹配状态 */


        /* Phase 14: Cash Shop System */
        case 510: return handle_cash_shop_get_list(s, data, data_len);   /* 商城列表 */
        case 511: return handle_cash_shop_buy(s, data, data_len);        /* 购买商品 */
        case 512: return handle_cash_shop_get_balance(s, data, data_len);/* 获取余额 */
        case 513: return handle_cash_shop_recharge(s, data, data_len);   /* 充值 */
        case 514: return handle_cash_shop_gift(s, data, data_len);       /* 赠送 */

        /* Phase 14: Cross-Server Matching System */
        case 550: return handle_cross_match_queue(s, data, data_len);    /* 跨服排队 */
        case 551: return handle_cross_match_cancel(s, data, data_len);   /* 取消排队 */
        case 552: return handle_cross_match_status(s, data, data_len);   /* 匹配状态 */
        case 553: return handle_cross_match_accept(s, data, data_len);   /* 接受匹配 */
        case 554: return handle_cross_match_decline(s, data, data_len);  /* 拒绝匹配 */
        case 555: return handle_cross_match_result(s, data, data_len);   /* 匹配结果 */
        default: return handle_default(s, msg_no, data, data_len);
    }
}
