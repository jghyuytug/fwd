/**
 * Mail Module - Public Interface
 * Purpose: In-game mail system with attachments and notifications
 *
 * Components:
 * - Mail sending and receiving
 * - Mail with item and gold attachments
 * - Mail inbox management
 * - Mail expiration and cleanup
 * - System mail and broadcasts
 * - Mail notifications
 */

#ifndef INCLUDE_MAIL_MAIL_INTERFACE_H_
#define INCLUDE_MAIL_MAIL_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Mail types
 */
typedef enum {
    MAIL_TYPE_PLAYER = 0,        // Player-to-player mail
    MAIL_TYPE_SYSTEM = 1,        // System mail
    MAIL_TYPE_NOTIFICATION = 2,  // Notification mail
    MAIL_TYPE_REWARD = 3,        // Reward mail (events, achievements)
    MAIL_TYPE_AUCTION = 4        // Auction house mail
} MailType;

/**
 * Mail states
 */
typedef enum {
    MAIL_STATE_UNREAD = 0,       // Mail not yet read
    MAIL_STATE_READ = 1,         // Mail has been read
    MAIL_STATE_DELETED = 2,      // Mail marked for deletion
    MAIL_STATE_CLAIMED = 3       // Attachments claimed
} MailState;

/**
 * Attachment types
 */
typedef enum {
    ATTACHMENT_TYPE_NONE = 0,
    ATTACHMENT_TYPE_ITEM = 1,
    ATTACHMENT_TYPE_GOLD = 2,
    ATTACHMENT_TYPE_PREMIUM_CURRENCY = 3
} AttachmentType;

/**
 * Mail attachment
 */
typedef struct {
    AttachmentType attachment_type;
    unsigned int item_id;        // 0 if not item
    int quantity;
    unsigned int gold_amount;    // 0 if not gold
} MailAttachment;

/**
 * Mail information
 */
typedef struct {
    unsigned int mail_id;
    MailType mail_type;
    MailState mail_state;
    unsigned int sender_id;      // 0 for system mail
    unsigned int receiver_id;
    char subject[128];
    char body[512];

    // Attachments
    int attachment_count;
    MailAttachment attachments[5];  // Up to 5 attachments per mail

    // Timestamps
    unsigned int send_time;
    unsigned int read_time;
    unsigned int claim_time;
    unsigned int expiration_time;
} MailInfo;

/**
 * Mail statistics
 */
typedef struct {
    unsigned int total_mails_sent;
    unsigned int total_mails_received;
    unsigned int total_mails_read;
    unsigned int total_mails_deleted;
    unsigned int total_attachments_sent;
    unsigned int total_attachments_claimed;
    unsigned int total_gold_sent;
    unsigned int total_gold_received;
} MailStats;

/**
 * Initialize Mail Module
 *
 * Sets up mail system
 *
 * Parameters:
 *   max_mails_per_user: Maximum mails per user inbox
 *   mail_retention_days: Days to keep mail before auto-delete
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Mail_Initialize(int max_mails_per_user, int mail_retention_days);

/**
 * Cleanup Mail Module
 *
 * Releases mail system resources
 */
void Mail_Cleanup();

/**
 * Send Mail
 *
 * Player sends mail to another player
 *
 * Parameters:
 *   sender_id: Sender user ID
 *   receiver_id: Receiver user ID
 *   subject: Mail subject (max 127 chars)
 *   body: Mail body (max 511 chars)
 *   attachments: Array of attachments (can be NULL)
 *   attachment_count: Number of attachments (0-5)
 *
 * Returns: Mail ID on success, negative error code on failure
 */
int Mail_Send(unsigned int sender_id, unsigned int receiver_id,
              const char* subject, const char* body,
              const MailAttachment* attachments, int attachment_count);

/**
 * Send System Mail
 *
 * System sends mail to a player
 *
 * Parameters:
 *   receiver_id: Receiver user ID
 *   subject: Mail subject
 *   body: Mail body
 *   attachments: Array of attachments (can be NULL)
 *   attachment_count: Number of attachments (0-5)
 *
 * Returns: Mail ID on success, negative error code on failure
 */
int Mail_SendSystem(unsigned int receiver_id,
                    const char* subject, const char* body,
                    const MailAttachment* attachments, int attachment_count);

/**
 * Send Broadcast Mail
 *
 * Send mail to all players
 *
 * Parameters:
 *   subject: Mail subject
 *   body: Mail body
 *   attachments: Array of attachments (can be NULL)
 *   attachment_count: Number of attachments (0-5)
 *
 * Returns: Number of mails sent
 */
int Mail_SendBroadcast(const char* subject, const char* body,
                       const MailAttachment* attachments, int attachment_count);

/**
 * Get Inbox
 *
 * Retrieves player's inbox
 *
 * Parameters:
 *   user_id: User ID
 *   mails: Output array of mail info
 *   max_mails: Maximum mails to retrieve
 *
 * Returns: Number of mails in inbox
 */
int Mail_GetInbox(unsigned int user_id, MailInfo* mails, int max_mails);

/**
 * Read Mail
 *
 * Mark mail as read
 *
 * Parameters:
 *   mail_id: Mail ID
 *   user_id: User ID (for verification)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Mail_Read(unsigned int mail_id, unsigned int user_id);

/**
 * Delete Mail
 *
 * Delete mail from inbox
 *
 * Parameters:
 *   mail_id: Mail ID
 *   user_id: User ID (for verification)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Mail_Delete(unsigned int mail_id, unsigned int user_id);

/**
 * Claim Attachments
 *
 * Claim mail attachments (items/gold)
 *
 * Parameters:
 *   mail_id: Mail ID
 *   user_id: User ID (for verification)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Mail_ClaimAttachments(unsigned int mail_id, unsigned int user_id);

/**
 * Get Mail Info
 *
 * Retrieves mail information
 *
 * Parameters:
 *   mail_id: Mail ID
 *   mail_info: Output mail structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Mail_GetInfo(unsigned int mail_id, MailInfo* mail_info);

/**
 * Get Unread Count
 *
 * Gets number of unread mails for a player
 *
 * Parameters:
 *   user_id: User ID
 *
 * Returns: Number of unread mails
 */
int Mail_GetUnreadCount(unsigned int user_id);

/**
 * Get Mail Statistics
 *
 * Retrieves mail statistics for a player
 *
 * Parameters:
 *   user_id: User ID
 *   stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Mail_GetStatistics(unsigned int user_id, MailStats* stats);

/**
 * Cleanup Expired Mails
 *
 * Removes expired mails from system
 *
 * Returns: Number of mails deleted
 */
int Mail_CleanupExpired();

/**
 * Update Mail State
 *
 * Processes mail tick, checks expiration
 *
 * Parameters:
 *   delta_time_ms: Time elapsed since last update (milliseconds)
 *
 * Returns: Number of mails processed
 */
int Mail_Update(int delta_time_ms);

#endif // INCLUDE_MAIL_MAIL_INTERFACE_H_
