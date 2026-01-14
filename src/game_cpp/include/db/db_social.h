#ifndef DB_SOCIAL_H
#define DB_SOCIAL_H

#include <stdint.h>

typedef struct {
    uint32_t postal_id;
    uint32_t send_charac_no;
    char     send_name[20];
    uint32_t item_id;
    uint32_t gold;
    uint32_t occ_time;
    uint8_t  read_flag;
} DBMailInfo;

typedef struct {
    uint32_t postal_id;
    uint32_t item_id;
    uint32_t gold;
    uint8_t  claimed;
} DBMailAttachment;

typedef struct {
    uint32_t friend_charac_no;
    char     friend_name[20];
    uint8_t  online;
} DBFriendInfo;

typedef struct {
    uint32_t guild_id;
    char     guild_name[40];
    char     master_name[20];
    uint8_t  level;
    uint32_t member_count;
    uint32_t guild_point;
} DBGuildInfo;

typedef struct {
    uint32_t charac_no;
    char     charac_name[20];
    uint8_t  grade;
    uint8_t  job;
    uint8_t  level;
    uint8_t  online;
} DBGuildMemberInfo;

#ifdef __cplusplus
extern "C" {
#endif

int db_get_mail_list(uint32_t charac_no, DBMailInfo* mails, int max_count);
int db_get_unread_mail_count(uint32_t charac_no);
int db_get_mail_attachment(uint32_t postal_id, uint32_t charac_no, DBMailAttachment* att);
int db_claim_mail_attachment(uint32_t postal_id);
int db_get_mail_by_id(uint32_t postal_id, uint32_t charac_no, DBMailInfo* mail);
int db_delete_mail(uint32_t postal_id, uint32_t charac_no);
int db_send_mail(uint32_t send_charac_no, const char* send_name,
                 uint32_t recv_charac_no, uint32_t item_id, uint32_t gold);

int db_add_friend(uint32_t charac_no, uint32_t friend_no);
int db_remove_friend(uint32_t charac_no, uint32_t friend_no);
int db_get_friend_list(uint32_t charac_no, DBFriendInfo* friends, int max_count);

int db_create_guild(const char* guild_name, uint32_t master_charac_no, const char* master_name);
int db_add_guild_member(uint32_t guild_id, uint32_t charac_no, const char* charac_name, uint8_t grade);
int db_update_guild_member_count(uint32_t guild_id, int delta);
int db_update_character_guild(uint32_t charac_no, uint32_t guild_id);
int db_remove_guild_member(uint32_t guild_id, uint32_t charac_no);
int db_disband_guild(uint32_t guild_id);
int db_get_guild_by_charac(uint32_t charac_no, DBGuildInfo* guild);
int db_get_guild_members(uint32_t guild_id, DBGuildMemberInfo* members, int max_count);

#ifdef __cplusplus
}
#endif

#endif /* DB_SOCIAL_H */
