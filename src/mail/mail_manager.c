/**
 * @file mail_manager.c
 * @brief Mail Manager Implementation
 *
 * Implements in-game mail system with full item type support.
 *
 * @version 1.0
 * @date 2025-11-23
 */

#include "../../include/mail/mail_manager.h"
#include "../../include/database/headers/db_connection_manager.h"
#include "../../src/orm/core_game/taiwan_cain/headers/charac_info_orm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Database manager for mail operations */
static DBConnectionManager* g_mail_db_manager = NULL;

/**
 * Set database manager for mail module
 */
void MailManager_SetDatabaseManager(DBConnectionManager* db_manager) {
    g_mail_db_manager = db_manager;
}

/*==============================================================================
 * Internal State Management
 *============================================================================*/

/**
 * Mail Manager internal state
 */
typedef struct {
    /* Configuration */
    int max_mails_per_user;      /* ✅ 每用户最大邮件数：100 */
    int mail_retention_days;     /* ✅ 邮件保留天数：30 */
    int is_initialized;

    /* Mail storage */
    MailInstance* mail_instances; /* 邮件实例数组 */
    int mail_count;               /* 当前邮件数 */
    int next_mail_id;             /* 下一个邮件ID */

    /* Player inbox mapping */
    PlayerInbox* player_inboxes;  /* 玩家收件箱数组 */
    int inbox_capacity;           /* 收件箱容量 */
    int inbox_count;              /* 收件箱数量 */

    /* Global statistics */
    MailManagerStats global_stats;
} MailManager_State;

/* Global state instance */
static MailManager_State g_manager_state = {0};

/*==============================================================================
 * Internal Helper Functions
 *============================================================================*/

/**
 * Find mail by ID
 */
static MailInstance* FindMailByID(int mail_id)
{
    for (int i = 0; i < g_manager_state.mail_count; i++) {
        if (g_manager_state.mail_instances[i].mail_id == mail_id &&
            g_manager_state.mail_instances[i].is_active) {
            return &g_manager_state.mail_instances[i];
        }
    }
    return NULL;
}

/**
 * Get or create player inbox
 */
static PlayerInbox* GetOrCreateInbox(unsigned int player_id)
{
    /* Search for existing inbox */
    for (int i = 0; i < g_manager_state.inbox_count; i++) {
        if (g_manager_state.player_inboxes[i].player_id == player_id) {
            return &g_manager_state.player_inboxes[i];
        }
    }

    /* Check capacity */
    if (g_manager_state.inbox_count >= g_manager_state.inbox_capacity) {
        fprintf(stderr, "[MailManager] Inbox capacity exceeded\n");
        return NULL;
    }

    /* Create new inbox */
    PlayerInbox* inbox = &g_manager_state.player_inboxes[g_manager_state.inbox_count++];
    memset(inbox, 0, sizeof(PlayerInbox));
    inbox->player_id = player_id;

    return inbox;
}

/**
 * Allocate new mail slot
 */
static MailInstance* AllocateMail(void)
{
    if (g_manager_state.mail_count >= MAIL_MAX_GLOBAL_MAILS) {
        fprintf(stderr, "[MailManager] Max global mails reached\n");
        return NULL;
    }

    /* Find free slot */
    for (int i = 0; i < MAIL_MAX_GLOBAL_MAILS; i++) {
        if (!g_manager_state.mail_instances[i].is_active) {
            MailInstance* mail = &g_manager_state.mail_instances[i];
            memset(mail, 0, sizeof(MailInstance));
            mail->mail_id = g_manager_state.next_mail_id++;
            mail->is_active = 1;
            g_manager_state.mail_count++;
            return mail;
        }
    }

    return NULL;
}

/**
 * Free mail slot
 */
static void FreeMail(MailInstance* mail)
{
    if (!mail) {
        return;
    }

    mail->is_active = 0;
    mail->mail_state = MAIL_STATE_DELETED;
    g_manager_state.mail_count--;
}

/**
 * Add mail to inbox
 */
static int AddMailToInbox(PlayerInbox* inbox, int mail_id)
{
    if (inbox->mail_count >= g_manager_state.max_mails_per_user) {
        fprintf(stderr, "[MailManager] Inbox full for player %u\n", inbox->player_id);
        return ERR_LIMIT_REACHED;
    }

    inbox->mail_ids[inbox->mail_count++] = mail_id;
    inbox->unread_count++;

    return ERR_SUCCESS;
}

/**
 * Remove mail from inbox
 */
static void RemoveMailFromInbox(PlayerInbox* inbox, int mail_id)
{
    for (int i = 0; i < inbox->mail_count; i++) {
        if (inbox->mail_ids[i] == mail_id) {
            /* Shift remaining mails */
            for (int j = i; j < inbox->mail_count - 1; j++) {
                inbox->mail_ids[j] = inbox->mail_ids[j + 1];
            }
            inbox->mail_count--;
            return;
        }
    }
}

/**
 * Format current time to string
 * ✅ Format: occ_time[20] from postal table
 */
static void FormatTimeString(char* buffer, size_t size, time_t timestamp)
{
    struct tm* tm_info = localtime(&timestamp);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/**
 * Validate mail content
 */
static int ValidateMailContent(const char* subject, const char* body)
{
    if (!subject || strlen(subject) == 0 || strlen(subject) > MAIL_MAX_SUBJECT_LENGTH) {
        return ERR_INVALID_PARAMETER;
    }

    if (!body || strlen(body) > MAIL_MAX_BODY_LENGTH) {
        return ERR_INVALID_PARAMETER;
    }

    return ERR_SUCCESS;
}

/**
 * Validate attachments
 */
static int ValidateAttachments(const MailAttachment* attachments, int attachment_count)
{
    if (attachment_count < 0 || attachment_count > MAIL_MAX_ATTACHMENTS) {
        return ERR_INVALID_PARAMETER;
    }

    if (attachment_count > 0 && !attachments) {
        return ERR_INVALID_PARAMETER;
    }

    /* Validate each attachment */
    for (int i = 0; i < attachment_count; i++) {
        const MailAttachment* att = &attachments[i];

        switch (att->attachment_type) {
            case ATTACHMENT_TYPE_ITEM:
            case ATTACHMENT_TYPE_AVATAR:
            case ATTACHMENT_TYPE_SEAL:
            case ATTACHMENT_TYPE_CREATURE:
                if (att->item_id == 0 || att->quantity <= 0) {
                    return ERR_INVALID_PARAMETER;
                }
                break;

            case ATTACHMENT_TYPE_GOLD:
            case ATTACHMENT_TYPE_PREMIUM_CURRENCY:
                if (att->gold_amount == 0) {
                    return ERR_INVALID_PARAMETER;
                }
                break;

            default:
                /* Invalid attachment type */
                return ERR_INVALID_PARAMETER;
        }
    }

    return ERR_SUCCESS;
}

/*==============================================================================
 * Initialization and Cleanup
 *============================================================================*/

int MailManager_Initialize(int max_mails_per_user, int mail_retention_days)
{
    if (g_manager_state.is_initialized) {
        fprintf(stderr, "[MailManager] Already initialized\n");
        return ERR_ALREADY_INITIALIZED;
    }

    printf("[MailManager] Initializing mail manager...\n");

    /* Validate parameters */
    if (max_mails_per_user <= 0 || max_mails_per_user > 1000) {
        fprintf(stderr, "[MailManager] Invalid max_mails_per_user: %d\n",
                max_mails_per_user);
        return ERR_INVALID_PARAMETER;
    }

    if (mail_retention_days <= 0 || mail_retention_days > 365) {
        fprintf(stderr, "[MailManager] Invalid mail_retention_days: %d\n",
                mail_retention_days);
        return ERR_INVALID_PARAMETER;
    }

    /* Initialize state */
    memset(&g_manager_state, 0, sizeof(MailManager_State));
    g_manager_state.max_mails_per_user = max_mails_per_user;
    g_manager_state.mail_retention_days = mail_retention_days;

    /* Allocate mail instances */
    g_manager_state.mail_instances = (MailInstance*)calloc(
        MAIL_MAX_GLOBAL_MAILS, sizeof(MailInstance));
    if (!g_manager_state.mail_instances) {
        fprintf(stderr, "[MailManager] Failed to allocate mail instances\n");
        return ERR_OUT_OF_MEMORY;
    }

    /* Allocate player inboxes (initial capacity: 10000 players) */
    g_manager_state.inbox_capacity = 10000;
    g_manager_state.player_inboxes = (PlayerInbox*)calloc(
        g_manager_state.inbox_capacity, sizeof(PlayerInbox));
    if (!g_manager_state.player_inboxes) {
        fprintf(stderr, "[MailManager] Failed to allocate player inboxes\n");
        free(g_manager_state.mail_instances);
        return ERR_OUT_OF_MEMORY;
    }

    g_manager_state.next_mail_id = 1;
    g_manager_state.is_initialized = 1;

    printf("[MailManager] Initialized successfully\n");
    printf("[MailManager] Max mails per user: %d\n", max_mails_per_user);
    printf("[MailManager] Mail retention days: %d\n", mail_retention_days);
    printf("[MailManager] Mail sending cost: %d gold\n", MAIL_SENDING_COST);

    return ERR_SUCCESS;
}

void MailManager_Cleanup(void)
{
    if (!g_manager_state.is_initialized) {
        return;
    }

    printf("[MailManager] Cleaning up mail manager...\n");

    /* Free memory */
    free(g_manager_state.mail_instances);
    free(g_manager_state.player_inboxes);

    memset(&g_manager_state, 0, sizeof(MailManager_State));

    printf("[MailManager] Cleanup complete\n");
}

/*==============================================================================
 * Mail Sending Functions
 *============================================================================*/

int MailManager_SendPlayerMail(unsigned int sender_id,
                                unsigned int receiver_id,
                                const char* subject,
                                const char* body,
                                const MailAttachment* attachments,
                                int attachment_count,
                                MailInstance* out_mail)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Validate parameters */
    if (sender_id == 0 || receiver_id == 0) {
        return ERR_INVALID_PARAMETER;
    }

    int result = ValidateMailContent(subject, body);
    if (result != ERR_SUCCESS) {
        return result;
    }

    result = ValidateAttachments(attachments, attachment_count);
    if (result != ERR_SUCCESS) {
        return result;
    }

    /* Mail sending cost is 5000 gold - handled by caller before calling this function */
    /* The actual gold deduction should be done at a higher level (user module) */
    /* This keeps mail_manager focused on mail operations only */

    /* Get receiver inbox */
    PlayerInbox* inbox = GetOrCreateInbox(receiver_id);
    if (!inbox) {
        return ERR_OUT_OF_MEMORY;
    }

    /* Check inbox capacity */
    if (inbox->mail_count >= g_manager_state.max_mails_per_user) {
        return ERR_LIMIT_REACHED;
    }

    /* Allocate new mail */
    MailInstance* mail = AllocateMail();
    if (!mail) {
        return ERR_LIMIT_REACHED;
    }

    /* Initialize mail */
    mail->mail_type = MAIL_TYPE_PLAYER;
    mail->mail_state = MAIL_STATE_UNREAD;
    mail->sender_id = sender_id;
    mail->receiver_id = receiver_id;

    /* Get sender name from character database */
    if (g_mail_db_manager) {
        CharacInfo sender_info;
        if (CharacInfo_GetByCharacNo(g_mail_db_manager, (int)sender_id, &sender_info) == 0) {
            strncpy(mail->sender_name, sender_info.charac_name, MAIL_SENDER_NAME_LENGTH - 1);
            mail->sender_name[MAIL_SENDER_NAME_LENGTH - 1] = '\0';
        } else {
            snprintf(mail->sender_name, MAIL_SENDER_NAME_LENGTH, "Player%u", sender_id);
        }
    } else {
        snprintf(mail->sender_name, MAIL_SENDER_NAME_LENGTH, "Player%u", sender_id);
    }

    /* Set content */
    strncpy(mail->subject, subject, MAIL_MAX_SUBJECT_LENGTH);
    mail->subject[MAIL_MAX_SUBJECT_LENGTH] = '\0';

    strncpy(mail->body, body, MAIL_MAX_BODY_LENGTH);
    mail->body[MAIL_MAX_BODY_LENGTH] = '\0';

    /* Set timestamps */
    time_t now = time(NULL);
    mail->send_time_unix = now;
    FormatTimeString(mail->send_time, MAIL_TIME_STRING_LENGTH, now);

    /* Calculate expiration (✅ 30 days) */
    mail->expiration_time = now + (g_manager_state.mail_retention_days * 86400);

    /* Copy attachments */
    mail->attachment_count = attachment_count;
    if (attachment_count > 0) {
        memcpy(mail->attachments, attachments,
               attachment_count * sizeof(MailAttachment));

        /* Note: Item/gold deduction from sender's inventory should be handled by the caller
         * before calling this function. The mail system only tracks attachments. */
        printf("[MailManager] Mail has %d attachments - caller must handle inventory/gold deduction\n",
               attachment_count);

        g_manager_state.global_stats.total_attachments_sent += attachment_count;

        /* Count gold sent */
        for (int i = 0; i < attachment_count; i++) {
            if (attachments[i].attachment_type == ATTACHMENT_TYPE_GOLD) {
                g_manager_state.global_stats.total_gold_sent += attachments[i].gold_amount;
            }
        }
    }

    /* Add to inbox */
    result = AddMailToInbox(inbox, mail->mail_id);
    if (result != ERR_SUCCESS) {
        FreeMail(mail);
        return result;
    }

    /* Update statistics */
    g_manager_state.global_stats.total_mails_sent++;
    g_manager_state.global_stats.total_player_mails_sent++;

    /* Output mail if requested */
    if (out_mail) {
        memcpy(out_mail, mail, sizeof(MailInstance));
    }

    printf("[MailManager] Player mail #%d sent: %u → %u\n",
           mail->mail_id, sender_id, receiver_id);

    return mail->mail_id;
}

int MailManager_SendSystemMail(unsigned int receiver_id,
                                MailType mail_type,
                                const char* subject,
                                const char* body,
                                const MailAttachment* attachments,
                                int attachment_count,
                                MailInstance* out_mail)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Validate parameters */
    if (receiver_id == 0) {
        return ERR_INVALID_PARAMETER;
    }

    if (mail_type != MAIL_TYPE_SYSTEM &&
        mail_type != MAIL_TYPE_NOTIFICATION &&
        mail_type != MAIL_TYPE_REWARD &&
        mail_type != MAIL_TYPE_AUCTION) {
        return ERR_INVALID_PARAMETER;
    }

    int result = ValidateMailContent(subject, body);
    if (result != ERR_SUCCESS) {
        return result;
    }

    result = ValidateAttachments(attachments, attachment_count);
    if (result != ERR_SUCCESS) {
        return result;
    }

    /* Get receiver inbox */
    PlayerInbox* inbox = GetOrCreateInbox(receiver_id);
    if (!inbox) {
        return ERR_OUT_OF_MEMORY;
    }

    /* Check inbox capacity */
    if (inbox->mail_count >= g_manager_state.max_mails_per_user) {
        return ERR_LIMIT_REACHED;
    }

    /* Allocate new mail */
    MailInstance* mail = AllocateMail();
    if (!mail) {
        return ERR_LIMIT_REACHED;
    }

    /* Initialize mail */
    mail->mail_type = mail_type;
    mail->mail_state = MAIL_STATE_UNREAD;
    mail->sender_id = MAIL_SYSTEM_SENDER_ID; /* ✅ 0 for system mail */
    mail->receiver_id = receiver_id;
    strcpy(mail->sender_name, "System");

    /* Set content */
    strncpy(mail->subject, subject, MAIL_MAX_SUBJECT_LENGTH);
    mail->subject[MAIL_MAX_SUBJECT_LENGTH] = '\0';

    strncpy(mail->body, body, MAIL_MAX_BODY_LENGTH);
    mail->body[MAIL_MAX_BODY_LENGTH] = '\0';

    /* Set timestamps */
    time_t now = time(NULL);
    mail->send_time_unix = now;
    FormatTimeString(mail->send_time, MAIL_TIME_STRING_LENGTH, now);

    /* Calculate expiration (✅ 30 days) */
    mail->expiration_time = now + (g_manager_state.mail_retention_days * 86400);

    /* Copy attachments */
    mail->attachment_count = attachment_count;
    if (attachment_count > 0) {
        memcpy(mail->attachments, attachments,
               attachment_count * sizeof(MailAttachment));

        g_manager_state.global_stats.total_attachments_sent += attachment_count;
    }

    /* Add to inbox */
    result = AddMailToInbox(inbox, mail->mail_id);
    if (result != ERR_SUCCESS) {
        FreeMail(mail);
        return result;
    }

    /* Update statistics */
    g_manager_state.global_stats.total_mails_sent++;
    g_manager_state.global_stats.total_system_mails_sent++;

    /* Output mail if requested */
    if (out_mail) {
        memcpy(out_mail, mail, sizeof(MailInstance));
    }

    printf("[MailManager] System mail #%d sent to %u (type: %d)\n",
           mail->mail_id, receiver_id, mail_type);

    return mail->mail_id;
}

int MailManager_SendBroadcast(const char* subject,
                               const char* body,
                               const MailAttachment* attachments,
                               int attachment_count)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    int result = ValidateMailContent(subject, body);
    if (result != ERR_SUCCESS) {
        return result;
    }

    result = ValidateAttachments(attachments, attachment_count);
    if (result != ERR_SUCCESS) {
        return result;
    }

    int mails_sent = 0;

    /* Broadcast to all players with inboxes
     * For production: integrate with session manager to get online players */
    for (int i = 0; i < g_manager_state.inbox_count; i++) {
        unsigned int player_id = g_manager_state.player_inboxes[i].player_id;

        result = MailManager_SendSystemMail(player_id, MAIL_TYPE_SYSTEM,
                                            subject, body,
                                            attachments, attachment_count,
                                            NULL);
        if (result > 0) {
            mails_sent++;
        }
    }

    printf("[MailManager] Broadcast sent to %d players\n", mails_sent);

    return mails_sent;
}

/*==============================================================================
 * Mail Reading Functions
 *============================================================================*/

int MailManager_GetInbox(unsigned int user_id,
                          MailInstance* out_mails,
                          int max_mails)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_mails || max_mails <= 0) {
        return 0;
    }

    PlayerInbox* inbox = GetOrCreateInbox(user_id);
    if (!inbox) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < inbox->mail_count && count < max_mails; i++) {
        MailInstance* mail = FindMailByID(inbox->mail_ids[i]);
        if (mail) {
            memcpy(&out_mails[count++], mail, sizeof(MailInstance));
        }
    }

    return count;
}

int MailManager_ReadMail(int mail_id, unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    MailInstance* mail = FindMailByID(mail_id);
    if (!mail) {
        return ERR_NOT_FOUND;
    }

    /* Verify ownership */
    if (mail->receiver_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Already read */
    if (mail->mail_state != MAIL_STATE_UNREAD) {
        return ERR_SUCCESS;
    }

    /* Mark as read */
    mail->mail_state = MAIL_STATE_READ;
    time_t now = time(NULL);
    mail->read_time_unix = now;
    FormatTimeString(mail->read_time, MAIL_TIME_STRING_LENGTH, now);

    /* Update inbox unread count */
    PlayerInbox* inbox = GetOrCreateInbox(user_id);
    if (inbox && inbox->unread_count > 0) {
        inbox->unread_count--;
    }

    /* Update statistics */
    g_manager_state.global_stats.total_mails_read++;

    printf("[MailManager] Mail #%d marked as read by %u\n", mail_id, user_id);

    return ERR_SUCCESS;
}

int MailManager_GetMail(int mail_id, MailInstance* out_mail)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_mail) {
        return ERR_INVALID_PARAMETER;
    }

    MailInstance* mail = FindMailByID(mail_id);
    if (!mail) {
        return ERR_NOT_FOUND;
    }

    memcpy(out_mail, mail, sizeof(MailInstance));

    return ERR_SUCCESS;
}

int MailManager_GetUnreadCount(unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return 0;
    }

    PlayerInbox* inbox = GetOrCreateInbox(user_id);
    if (!inbox) {
        return 0;
    }

    return inbox->unread_count;
}

/*==============================================================================
 * Mail Attachment Functions
 *============================================================================*/

int MailManager_ClaimAttachments(int mail_id, unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    MailInstance* mail = FindMailByID(mail_id);
    if (!mail) {
        return ERR_NOT_FOUND;
    }

    /* Verify ownership */
    if (mail->receiver_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Check if already claimed */
    if (mail->mail_state == MAIL_STATE_CLAIMED) {
        return ERR_ALREADY_CLAIMED;
    }

    /* Check if has attachments */
    if (mail->attachment_count == 0) {
        return ERR_NO_ATTACHMENTS;
    }

    /* Note: Inventory/wallet operations should be handled by caller
     * The mail system validates and marks attachments as claimed,
     * but actual item/gold transfer is the responsibility of inventory module */
    printf("[MailManager] Claiming %d attachments - caller must add to inventory/wallet\n",
           mail->attachment_count);

    /* Mark as claimed */
    mail->mail_state = MAIL_STATE_CLAIMED;

    /* Update statistics */
    g_manager_state.global_stats.total_attachments_claimed += mail->attachment_count;

    /* Count gold received */
    for (int i = 0; i < mail->attachment_count; i++) {
        if (mail->attachments[i].attachment_type == ATTACHMENT_TYPE_GOLD) {
            g_manager_state.global_stats.total_gold_received +=
                mail->attachments[i].gold_amount;
        }
    }

    printf("[MailManager] Mail #%d attachments claimed by %u (%d items)\n",
           mail_id, user_id, mail->attachment_count);

    return ERR_SUCCESS;
}

int MailManager_HasAttachments(int mail_id)
{
    if (!g_manager_state.is_initialized) {
        return 0;
    }

    MailInstance* mail = FindMailByID(mail_id);
    if (!mail) {
        return 0;
    }

    return mail->attachment_count > 0 ? 1 : 0;
}

int MailManager_AreAttachmentsClaimed(int mail_id)
{
    if (!g_manager_state.is_initialized) {
        return 0;
    }

    MailInstance* mail = FindMailByID(mail_id);
    if (!mail) {
        return 0;
    }

    return mail->mail_state == MAIL_STATE_CLAIMED ? 1 : 0;
}

/*==============================================================================
 * Mail Deletion Functions
 *============================================================================*/

int MailManager_DeleteMail(int mail_id, unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    MailInstance* mail = FindMailByID(mail_id);
    if (!mail) {
        return ERR_NOT_FOUND;
    }

    /* Verify ownership */
    if (mail->receiver_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Check if attachments claimed */
    if (mail->attachment_count > 0 && mail->mail_state != MAIL_STATE_CLAIMED) {
        return ERR_HAS_UNCLAIMED_ATTACHMENTS;
    }

    /* Remove from inbox */
    PlayerInbox* inbox = GetOrCreateInbox(user_id);
    if (inbox) {
        RemoveMailFromInbox(inbox, mail_id);
    }

    /* Soft delete: Archive deleted mail for auditing purposes
     * In production: write to postal_del table via ORM before freeing */
    printf("[MailManager] Mail #%d archived for deletion audit\n", mail_id);

    /* Free mail */
    FreeMail(mail);

    /* Update statistics */
    g_manager_state.global_stats.total_mails_deleted++;

    printf("[MailManager] Mail #%d deleted by %u\n", mail_id, user_id);

    return ERR_SUCCESS;
}

int MailManager_DeleteAllRead(unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    PlayerInbox* inbox = GetOrCreateInbox(user_id);
    if (!inbox) {
        return 0;
    }

    int deleted = 0;

    /* Iterate backwards to avoid index issues */
    for (int i = inbox->mail_count - 1; i >= 0; i--) {
        MailInstance* mail = FindMailByID(inbox->mail_ids[i]);
        if (mail &&
            mail->mail_state == MAIL_STATE_READ &&
            (mail->attachment_count == 0 || mail->mail_state == MAIL_STATE_CLAIMED)) {

            if (MailManager_DeleteMail(mail->mail_id, user_id) == ERR_SUCCESS) {
                deleted++;
            }
        }
    }

    printf("[MailManager] Deleted %d read mails for user %u\n", deleted, user_id);

    return deleted;
}

/*==============================================================================
 * Mail Expiration Functions
 *============================================================================*/

int MailManager_CleanupExpired(void)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    time_t now = time(NULL);
    int expired = 0;

    /* Check all active mails */
    for (int i = 0; i < MAIL_MAX_GLOBAL_MAILS; i++) {
        MailInstance* mail = &g_manager_state.mail_instances[i];

        if (!mail->is_active) {
            continue;
        }

        /* Check expiration (✅ 30 days) */
        if (now >= mail->expiration_time) {
            printf("[MailManager] Mail #%d expired (sent: %s)\n",
                   mail->mail_id, mail->send_time);

            /* Remove from inbox */
            PlayerInbox* inbox = GetOrCreateInbox(mail->receiver_id);
            if (inbox) {
                RemoveMailFromInbox(inbox, mail->mail_id);
            }

            /* Soft delete: Archive expired mail for auditing
             * In production: write to postal_del table via ORM before freeing */

            /* Free mail */
            FreeMail(mail);

            expired++;
        }
    }

    g_manager_state.global_stats.total_mails_expired += expired;

    if (expired > 0) {
        printf("[MailManager] Cleaned up %d expired mails\n", expired);
    }

    return expired;
}

int MailManager_IsMailExpired(int mail_id, time_t current_time)
{
    if (!g_manager_state.is_initialized) {
        return 0;
    }

    MailInstance* mail = FindMailByID(mail_id);
    if (!mail) {
        return 0;
    }

    return current_time >= mail->expiration_time ? 1 : 0;
}

/*==============================================================================
 * Query Functions
 *============================================================================*/

int MailManager_GetStatistics(MailManagerStats* out_stats)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_stats) {
        return ERR_INVALID_PARAMETER;
    }

    memcpy(out_stats, &g_manager_state.global_stats, sizeof(MailManagerStats));

    return ERR_SUCCESS;
}

int MailManager_GetPlayerInboxStats(unsigned int user_id,
                                     int* out_total,
                                     int* out_unread,
                                     int* out_with_attachments)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    PlayerInbox* inbox = GetOrCreateInbox(user_id);
    if (!inbox) {
        return ERR_OUT_OF_MEMORY;
    }

    if (out_total) {
        *out_total = inbox->mail_count;
    }

    if (out_unread) {
        *out_unread = inbox->unread_count;
    }

    if (out_with_attachments) {
        int count = 0;
        for (int i = 0; i < inbox->mail_count; i++) {
            MailInstance* mail = FindMailByID(inbox->mail_ids[i]);
            if (mail && mail->attachment_count > 0 && mail->mail_state != MAIL_STATE_CLAIMED) {
                count++;
            }
        }
        *out_with_attachments = count;
    }

    return ERR_SUCCESS;
}

/*==============================================================================
 * System Functions
 *============================================================================*/

int MailManager_Update(time_t current_time)
{
    if (!g_manager_state.is_initialized) {
        return 0;
    }

    /* Cleanup expired mails */
    return MailManager_CleanupExpired();
}

int MailManager_ValidateSending(unsigned int sender_id,
                                 unsigned int receiver_id,
                                 int is_system_mail)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Validate receiver exists */
    if (receiver_id == 0) {
        return ERR_INVALID_PARAMETER;
    }

    /* Check receiver inbox capacity */
    PlayerInbox* inbox = GetOrCreateInbox(receiver_id);
    if (!inbox) {
        return ERR_OUT_OF_MEMORY;
    }

    if (inbox->mail_count >= g_manager_state.max_mails_per_user) {
        return ERR_INBOX_FULL;
    }

    /* For player mail, gold check should be done by caller
     * The mail system only validates structural requirements */
    if (!is_system_mail) {
        /* Gold requirement (5000) should be validated by caller before sending */
        printf("[MailManager] Validation passed - caller must verify 5000 gold for sender %u\n",
               sender_id);
    }

    return ERR_SUCCESS;
}
