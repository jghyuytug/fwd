#ifndef COMMON_WONGWORK_NAMESPACE_H
#define COMMON_WONGWORK_NAMESPACE_H

// WongWork namespace - Anti-cheat and security system
// Based on error analysis and ORM table structures
// Top members: HackIPCounter_t (1,320), HackFullIPCounter_t (1,320), Avatar_Item (1,044)
// Updated: 2025-11-25 - Fields derived from ORM tables and decompiled code patterns

namespace WongWork {

    // ========================================
    // Hack Detection Types
    // ========================================

    // IP-based hack counter
    // Error count: 1,320 errors
    // Source: hack_ip table structure
    struct HackIPCounter_t {
        unsigned int ip_hash;           // Hashed IP address
        unsigned int hack_count;        // Number of hack attempts
        unsigned int first_detected;    // First detection timestamp
        unsigned int last_detected;     // Last detection timestamp
        unsigned int ban_until;         // Ban expiration time (0 = permanent)
    };

    // Full IP hack counter
    // Error count: 1,320 errors
    // Source: hack_full_ip table structure
    struct HackFullIPCounter_t {
        char ip_address[16];            // Full IP address string
        unsigned int hack_count;        // Number of hack attempts
        unsigned int hack_type_mask;    // Bitmask of detected hack types
        unsigned int first_detected;    // First detection timestamp
        unsigned int last_detected;     // Last detection timestamp
        int severity_level;             // 0=warning, 1=temporary, 2=permanent
    };

    // Hack type information
    // Error count: 840 errors
    struct stHacktypeExceptionInfo_t {
        int hack_type;                  // ENUM_HACKTYPE value
        int exception_code;             // Exception reason code
        char description[128];          // Human-readable description
        int auto_ban_enabled;           // Whether to auto-ban this type
    };

    // Hack information structure
    // Error count: 504 errors
    // Source: hack_info table structure
    struct stHackInfo_t {
        int charac_no;                  // Character ID
        int hack_type;                  // ENUM_HACKTYPE value
        unsigned int detect_time;       // Detection timestamp
        char ip_address[16];            // IP at time of detection
        char hack_data[256];            // Additional hack details
    };

    // Hack type enumeration
    // Error count: 816 errors
    // Derived from auto_punish system and hack_info patterns
    enum ENUM_HACKTYPE {
        HACKTYPE_UNKNOWN = 0,
        HACKTYPE_SPEED = 1,             // Speed hack
        HACKTYPE_DAMAGE = 2,            // Damage modification
        HACKTYPE_TELEPORT = 3,          // Position teleport
        HACKTYPE_ITEM_DUP = 4,          // Item duplication
        HACKTYPE_GOLD = 5,              // Gold manipulation
        HACKTYPE_PACKET = 6,            // Invalid packet data
        HACKTYPE_MEMORY = 7,            // Memory modification
        HACKTYPE_CLIENT = 8,            // Modified client
        HACKTYPE_BOT = 9,               // Bot/automation
        HACKTYPE_EXPLOIT = 10           // Game exploit
    };

    // Black IP monitor entry
    // Error count: 48 errors
    struct stBlackIPMonitorEntry_t {
        char ip_address[16];            // IP address to monitor
        int monitor_level;              // 0=watch, 1=restrict, 2=block
        unsigned int added_time;        // When added to monitor list
        int report_count;               // Number of reports against this IP
    };

    // Hack type info
    // Error count: 72 errors
    struct HackTypeInfo_t {
        int hack_type_id;               // ENUM_HACKTYPE value
        char type_name[32];             // Human-readable name
        int default_ban_days;           // Default ban duration
        int severity;                   // 1-10 severity scale
    };

    // ========================================
    // Avatar and Items
    // ========================================

    // Avatar item tracking
    // Error count: 1,044 errors
    // Source: Character avatar/costume system
    struct Avatar_Item {
        int item_id;                    // Item template ID
        int slot;                       // Avatar slot index
        int expire_time;                // Expiration timestamp (0 = permanent)
        int enchant_level;              // Enhancement level
        int color_index;                // Dye/color selection
    };

    // ========================================
    // IP Geolocation
    // ========================================

    // IP geolocation group
    // Error count: 756 errors
    // Wraps GeoIP library functionality
    class IPG {
    public:
        static int GetCountryCode(const char* ip_address);
        static int GetRegionCode(const char* ip_address);
        static bool IsAllowedRegion(int region_code);
        /* Internal: Uses libGeoIP for IP-to-location mapping */
    };

    // ========================================
    // GM Accounts
    // ========================================

    // GM account management
    // Error count: 708 errors
    // Source: GM permission and account handling
    class CGMAccounts {
    public:
        static bool IsGMAccount(int account_id);
        static int GetGMLevel(int account_id);
        static bool HasPermission(int account_id, int permission_flag);
        /* GM levels: 0=player, 1=helper, 2=GM, 3=senior GM, 4=admin */
    };

    // ========================================
    // Mailbox
    // ========================================

    // Mailbox system
    // Error count: 72 errors
    class CMailBox {
    public:
        static int SendMail(int sender_id, int receiver_id, const char* subject,
                           const char* body, int attached_gold, int* attached_items, int item_count);
        static int GetUnreadCount(int character_id);
        /* Mail storage: Uses postal table via ORM */
    };

    // ========================================
    // Auto-Punish System
    // ========================================

    // Auto-punish rule for black IP management
    // Error count: 360 errors
    // Source: Automated ban/punishment system
    class CAutoPunishRuleBlackIPMgr {
    public:
        static bool CheckIP(const char* ip_address);
        static int AddBlackIP(const char* ip_address, int ban_duration, const char* reason);
        static int RemoveBlackIP(const char* ip_address);
        /* Database: hack_ip, hack_full_ip tables via ORM */
    };

    // ========================================
    // Hack Analyzer
    // ========================================

    // Main hack analysis class (defined in security/hack_analyzer.h)
    class CHackAnalyzer;

} // namespace WongWork

#endif // COMMON_WONGWORK_NAMESPACE_H
