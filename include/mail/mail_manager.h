/**
 * @file mail_manager.h
 * @brief Mail Manager - Internal API
 *
 * Manages in-game mail system with attachments and notifications.
 *
 * Key Features:
 * - Player-to-player mail with gold cost (5000 gold/mail)
 * - System mail and broadcasts (free)
 * - Item and gold attachments (up to 5 per mail)
 * - Full item type support (avatar, seal, creature, normal items)
 * - Mail expiration and auto-cleanup (30 days)
 * - Inbox capacity management (100 mails per user)
 * - Soft delete to postal_del table
 *
 * @version 1.0
 * @date 2025-11-23
 */

#ifndef INCLUDE_MAIL_MAIL_MANAGER_H_
#define INCLUDE_MAIL_MAIL_MANAGER_H_

#include <common/defs.h>
#include <common/error_codes.h>
#include <time.h>

/*==============================================================================
 * Constants and Configuration
 *============================================================================*/

/* ✅ User confirmed configuration */
#define MAIL_MAX_PER_USER           100     /* ✅ 每用户最大邮件数：100封 */
#define MAIL_RETENTION_DAYS         30      /* ✅ 邮件保留天数：30天 */
#define MAIL_SENDING_COST           5000    /* ✅ 发送邮件费用：5000金币 */

/* ✅ From mail_interface.h */
#define MAIL_MAX_SUBJECT_LENGTH     128     /* ✅ 主题最大长度：128字符 */
#define MAIL_MAX_BODY_LENGTH        512     /* ✅ 正文最大长度：512字符 */
#define MAIL_MAX_ATTACHMENTS        5       /* ✅ 最多5个附件 */

/* ✅ From postal ORM table */
#define MAIL_SENDER_NAME_LENGTH     21      /* ✅ send_charac_name[21] */
#define MAIL_TIME_STRING_LENGTH     20      /* ✅ occ_time[20] */

/* Internal constants */
#define MAIL_INVALID_ID             -1      /* 无效邮件ID */
#define MAIL_SYSTEM_SENDER_ID       0       /* 系统邮件发送者ID */
#define MAIL_MAX_GLOBAL_MAILS       100000  /* 全局最大邮件数 */

/*==============================================================================
 * Data Types
 *============================================================================*/

/**
 * Mail types
 * ✅ 5种邮件类型
 */
typedef enum {
    MAIL_TYPE_PLAYER = 0,        /* 玩家邮件 */
    MAIL_TYPE_SYSTEM = 1,        /* 系统邮件 */
    MAIL_TYPE_NOTIFICATION = 2,  /* 通知邮件 */
    MAIL_TYPE_REWARD = 3,        /* 奖励邮件 */
    MAIL_TYPE_AUCTION = 4        /* 拍卖行邮件 */
} MailType;

/**
 * Mail states
 * ✅ 4种邮件状态
 */
typedef enum {
    MAIL_STATE_UNREAD = 0,       /* 未读 */
    MAIL_STATE_READ = 1,         /* 已读 */
    MAIL_STATE_DELETED = 2,      /* 已删除 */
    MAIL_STATE_CLAIMED = 3       /* 附件已领取 */
} MailState;

/**
 * Attachment types
 * ✅ 支持所有类型（用户确认）
 */
typedef enum {
    ATTACHMENT_TYPE_NONE = 0,
    ATTACHMENT_TYPE_ITEM = 1,            /* 普通物品 */
    ATTACHMENT_TYPE_GOLD = 2,            /* 金币 */
    ATTACHMENT_TYPE_PREMIUM_CURRENCY = 3, /* 点券 */
    ATTACHMENT_TYPE_AVATAR = 4,          /* 时装 (avata_flag) */
    ATTACHMENT_TYPE_SEAL = 5,            /* 封印物品 (seal_flag) */
    ATTACHMENT_TYPE_CREATURE = 6         /* 宠物 (creature_flag) */
} AttachmentType;

/**
 * Mail attachment
 * ✅ Based on postal ORM table item fields
 */
typedef struct {
    AttachmentType attachment_type;   /* 附件类型 */

    /* ✅ From postal table */
    int item_id;                      /* ✅ item_id */
    int quantity;                     /* 数量（非堆叠物品为1） */
    unsigned int gold_amount;         /* ✅ gold */

    /* Item properties (✅ from postal table) */
    int add_info;                     /* ✅ add_info */
    short endurance;                  /* ✅ endurance */
    signed char upgrade;              /* ✅ upgrade */
    signed char amplify_option;       /* ✅ amplify_option */
    int amplify_value;                /* ✅ amplify_value */
    unsigned char random_option[14];  /* ✅ random_option[14] */
    signed char seperate_upgrade;     /* ✅ seperate_upgrade */

    /* Item flags (✅ from postal table) */
    signed char avata_flag;           /* ✅ avata_flag */
    signed char unlimit_flag;         /* ✅ unlimit_flag */
    signed char seal_flag;            /* ✅ seal_flag */
    signed char creature_flag;        /* ✅ creature_flag */

    /* Additional info */
    int extend_info;                  /* ✅ extend_info */
    unsigned char item_guid[10];      /* ✅ item_guid[10] */
} MailAttachment;

/**
 * Mail instance
 * ✅ Based on postal ORM table (36 fields)
 */
typedef struct {
    /* Mail identification (✅ from postal) */
    int mail_id;                      /* ✅ postal_id */
    MailType mail_type;               /* 邮件类型 */
    MailState mail_state;             /* 邮件状态 */

    /* Sender and receiver (✅ from postal) */
    unsigned int sender_id;           /* ✅ send_charac_no (0 for system) */
    char sender_name[MAIL_SENDER_NAME_LENGTH]; /* ✅ send_charac_name[21] */
    unsigned int receiver_id;         /* ✅ receive_charac_no */

    /* Content */
    char subject[MAIL_MAX_SUBJECT_LENGTH + 1]; /* 主题 */
    char body[MAIL_MAX_BODY_LENGTH + 1];       /* 正文 */
    int letter_id;                    /* ✅ letter_id (信件模板ID) */

    /* Attachments */
    int attachment_count;             /* 附件数量 */
    MailAttachment attachments[MAIL_MAX_ATTACHMENTS];

    /* Timestamps (✅ from postal) */
    char send_time[MAIL_TIME_STRING_LENGTH];    /* ✅ occ_time[20] */
    char read_time[MAIL_TIME_STRING_LENGTH];    /* ✅ receive_time[20] */
    time_t send_time_unix;            /* Unix timestamp for calculations */
    time_t read_time_unix;            /* Unix timestamp */
    time_t expiration_time;           /* send_time + 30 days */

    /* Flags (✅ from postal) */
    signed char delete_flag;          /* ✅ delete_flag */

    /* Auction integration (✅ from postal) */
    long long auction_id;             /* ✅ auction_id */

    /* Payment info (✅ from postal) */
    signed char ipg_db_id;            /* ✅ ipg_db_id */
    int ipg_transaction_id;           /* ✅ ipg_transaction_id */
    char ipg_nexon_id[33];            /* ✅ ipg_nexon_id[33] */

    /* Internal flags */
    int is_active;                    /* 是否活跃 */
} MailInstance;

/**
 * Player inbox
 * Manages all mails for a player
 */
typedef struct {
    unsigned int player_id;           /* 角色ID */
    int mail_ids[MAIL_MAX_PER_USER];  /* 邮件ID数组 */
    int mail_count;                   /* 邮件数量 */
    int unread_count;                 /* 未读邮件数 */
} PlayerInbox;

/**
 * Mail Manager global statistics
 */
typedef struct {
    unsigned int total_mails_sent;         /* 总发送邮件数 */
    unsigned int total_player_mails_sent;  /* 玩家邮件数 */
    unsigned int total_system_mails_sent;  /* 系统邮件数 */
    unsigned int total_mails_read;         /* 总已读邮件数 */
    unsigned int total_mails_deleted;      /* 总已删除邮件数 */
    unsigned int total_attachments_sent;   /* 总发送附件数 */
    unsigned int total_attachments_claimed;/* 总领取附件数 */
    unsigned int total_gold_sent;          /* 总发送金币数 */
    unsigned int total_gold_received;      /* 总接收金币数 */
    unsigned int total_mails_expired;      /* 总过期邮件数 */
} MailManagerStats;

/*==============================================================================
 * Mail Manager API
 *============================================================================*/

/**
 * Initialize Mail Manager
 *
 * Sets up mail management system with user-confirmed parameters.
 *
 * Parameters:
 *   max_mails_per_user: Maximum mails per user (✅ default: 100)
 *   mail_retention_days: Days to keep mail (✅ default: 30)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int MailManager_Initialize(int max_mails_per_user, int mail_retention_days);

/**
 * Cleanup Mail Manager
 *
 * Releases all resources.
 */
void MailManager_Cleanup(void);

/*==============================================================================
 * Mail Sending Functions
 *============================================================================*/

/**
 * Send Player Mail
 *
 * Player sends mail to another player.
 *
 * Requirements:
 * - Sender must have 5000 gold (✅ user confirmed cost)
 * - Receiver inbox must have space (< 100 mails)
 * - Subject and body must be valid
 * - Attachments must be valid (if any)
 *
 * Actions:
 * - Deduct 5000 gold from sender
 * - Create new mail in receiver's inbox
 * - Remove attached items from sender's inventory
 * - Deduct attached gold from sender
 *
 * Parameters:
 *   sender_id: Sender character ID
 *   receiver_id: Receiver character ID
 *   subject: Mail subject (max 127 chars)
 *   body: Mail body (max 511 chars)
 *   attachments: Array of attachments (can be NULL)
 *   attachment_count: Number of attachments (0-5)
 *   out_mail: Optional output mail instance
 *
 * Returns: Mail ID on success, negative error code on failure
 */
int MailManager_SendPlayerMail(unsigned int sender_id,
                                unsigned int receiver_id,
                                const char* subject,
                                const char* body,
                                const MailAttachment* attachments,
                                int attachment_count,
                                MailInstance* out_mail);

/**
 * Send System Mail
 *
 * System sends mail to a player (free, no gold cost).
 *
 * Parameters:
 *   receiver_id: Receiver character ID
 *   mail_type: MAIL_TYPE_SYSTEM, NOTIFICATION, REWARD, or AUCTION
 *   subject: Mail subject
 *   body: Mail body
 *   attachments: Array of attachments (can be NULL)
 *   attachment_count: Number of attachments (0-5)
 *   out_mail: Optional output mail instance
 *
 * Returns: Mail ID on success, negative error code on failure
 */
int MailManager_SendSystemMail(unsigned int receiver_id,
                                MailType mail_type,
                                const char* subject,
                                const char* body,
                                const MailAttachment* attachments,
                                int attachment_count,
                                MailInstance* out_mail);

/**
 * Send Broadcast Mail
 *
 * Send system mail to all online players.
 *
 * Parameters:
 *   subject: Mail subject
 *   body: Mail body
 *   attachments: Array of attachments (can be NULL)
 *   attachment_count: Number of attachments (0-5)
 *
 * Returns: Number of mails sent
 */
int MailManager_SendBroadcast(const char* subject,
                               const char* body,
                               const MailAttachment* attachments,
                               int attachment_count);

/*==============================================================================
 * Mail Reading Functions
 *============================================================================*/

/**
 * Get Inbox
 *
 * Retrieves player's inbox (all mails).
 *
 * Parameters:
 *   user_id: Character ID
 *   out_mails: Output array of mail instances
 *   max_mails: Maximum mails to retrieve
 *
 * Returns: Number of mails in inbox
 */
int MailManager_GetInbox(unsigned int user_id,
                          MailInstance* out_mails,
                          int max_mails);

/**
 * Read Mail
 *
 * Mark mail as read.
 *
 * State transition: UNREAD → READ
 *
 * Parameters:
 *   mail_id: Mail ID
 *   user_id: Character ID (for verification)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int MailManager_ReadMail(int mail_id, unsigned int user_id);

/**
 * Get Mail Info
 *
 * Retrieves mail information.
 *
 * Parameters:
 *   mail_id: Mail ID
 *   out_mail: Output mail instance
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int MailManager_GetMail(int mail_id, MailInstance* out_mail);

/**
 * Get Unread Count
 *
 * Gets number of unread mails for a player.
 *
 * Parameters:
 *   user_id: Character ID
 *
 * Returns: Number of unread mails
 */
int MailManager_GetUnreadCount(unsigned int user_id);

/*==============================================================================
 * Mail Attachment Functions
 *============================================================================*/

/**
 * Claim Attachments
 *
 * Claim all attachments from a mail.
 *
 * Requirements:
 * - Player must have inventory space
 * - Mail must have attachments
 * - Mail must not be already claimed
 *
 * Actions:
 * - Add items to player's inventory
 * - Add gold to player's wallet
 * - Mark mail as claimed
 * - State → CLAIMED
 *
 * Parameters:
 *   mail_id: Mail ID
 *   user_id: Character ID (for verification)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int MailManager_ClaimAttachments(int mail_id, unsigned int user_id);

/**
 * Check if Mail has Attachments
 *
 * Parameters:
 *   mail_id: Mail ID
 *
 * Returns: 1 if has attachments, 0 otherwise
 */
int MailManager_HasAttachments(int mail_id);

/**
 * Check if Attachments are Claimed
 *
 * Parameters:
 *   mail_id: Mail ID
 *
 * Returns: 1 if claimed, 0 otherwise
 */
int MailManager_AreAttachmentsClaimed(int mail_id);

/*==============================================================================
 * Mail Deletion Functions
 *============================================================================*/

/**
 * Delete Mail
 *
 * Soft delete mail from inbox (moves to postal_del table).
 *
 * Requirements:
 * - Attachments must be claimed or mail has no attachments
 *
 * State transition: ANY → DELETED
 *
 * Parameters:
 *   mail_id: Mail ID
 *   user_id: Character ID (for verification)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int MailManager_DeleteMail(int mail_id, unsigned int user_id);

/**
 * Delete All Read Mails
 *
 * Deletes all read mails without attachments for a player.
 *
 * Parameters:
 *   user_id: Character ID
 *
 * Returns: Number of mails deleted
 */
int MailManager_DeleteAllRead(unsigned int user_id);

/*==============================================================================
 * Mail Expiration Functions
 *============================================================================*/

/**
 * Cleanup Expired Mails
 *
 * Removes all mails older than 30 days (✅ user confirmed).
 *
 * Actions:
 * - Find all mails with (send_time + 30 days) < current_time
 * - Soft delete to postal_del table
 * - Update statistics
 *
 * Returns: Number of mails expired and deleted
 */
int MailManager_CleanupExpired(void);

/**
 * Check if Mail is Expired
 *
 * Parameters:
 *   mail_id: Mail ID
 *   current_time: Current server time
 *
 * Returns: 1 if expired, 0 otherwise
 */
int MailManager_IsMailExpired(int mail_id, time_t current_time);

/*==============================================================================
 * Query Functions
 *============================================================================*/

/**
 * Get Mail Statistics
 *
 * Retrieves global mail statistics.
 *
 * Parameters:
 *   out_stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int MailManager_GetStatistics(MailManagerStats* out_stats);

/**
 * Get Player Inbox Statistics
 *
 * Gets inbox statistics for a player.
 *
 * Parameters:
 *   user_id: Character ID
 *   out_total: Total mails in inbox
 *   out_unread: Unread mails count
 *   out_with_attachments: Mails with unclaimed attachments
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int MailManager_GetPlayerInboxStats(unsigned int user_id,
                                     int* out_total,
                                     int* out_unread,
                                     int* out_with_attachments);

/*==============================================================================
 * System Functions
 *============================================================================*/

/**
 * Update Mail Manager
 *
 * Processes mail expiration and cleanup.
 * Should be called periodically (e.g., every hour).
 *
 * Actions:
 * - Check for expired mails (> 30 days)
 * - Cleanup expired mails
 *
 * Parameters:
 *   current_time: Current server time
 *
 * Returns: Number of mails processed
 */
int MailManager_Update(time_t current_time);

/**
 * Validate Mail Sending
 *
 * Checks if player can send mail.
 *
 * Validations:
 * - Sender has 5000 gold (for player mail)
 * - Receiver inbox has space (< 100 mails)
 * - Receiver exists and is valid
 *
 * Parameters:
 *   sender_id: Sender character ID
 *   receiver_id: Receiver character ID
 *   is_system_mail: 1 if system mail (no gold cost), 0 otherwise
 *
 * Returns: ERR_SUCCESS if can send, error code otherwise
 */
int MailManager_ValidateSending(unsigned int sender_id,
                                 unsigned int receiver_id,
                                 int is_system_mail);

#endif // INCLUDE_MAIL_MAIL_MANAGER_H_
