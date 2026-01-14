#ifndef DATABASE_DB_ENUMS_H
#define DATABASE_DB_ENUMS_H



// Database connection handle indices
// Confidence: 85% - Clear usage in nested std::map
// Evidence: std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo>>
// Extended: Based on ORM layer analysis (22 production databases)
enum ENUM_DB_HANDLE_IDX {
    DB_HANDLE_ACCOUNT = 0,      // d_taiwan - Account database
    DB_HANDLE_CHARACTER = 1,    // taiwan_cain - Character database
    DB_HANDLE_GAME = 2,         // taiwan_cain - Game state database
    DB_HANDLE_LOG = 3,          // taiwan_cain_log - Logging database
    DB_HANDLE_CASH = 4,         // taiwan_billing - Cash shop database
    DB_HANDLE_GUILD = 5,        // d_guild - Guild database
    DB_HANDLE_LOGIN = 6,        // taiwan_login - Login/session database
    DB_HANDLE_SECURITY = 7,     // d_taiwan_secu - Security monitoring
    DB_HANDLE_EVENT = 8,        // taiwan_game_event - Event operations
    DB_HANDLE_AUCTION = 9,      // taiwan_cain_auction_* - Auction house
    DB_HANDLE_CHANNEL = 10,     // d_channel - Channel system
    DB_HANDLE_WEB = 11,         // taiwan_main_web - Web community
    /* Additional handles allocated dynamically for extended databases */
};

// Server group enumeration
// Confidence: 85% - Used as map key for DB connections
// Taiwan server region configuration
enum ENUM_SERVER_GROUP {
    SERVER_GROUP_CAIN = 0,      // Main server (taiwan_cain)
    SERVER_GROUP_DIREGIE = 1,   // Secondary server
    SERVER_GROUP_SIROCO = 2,    // Third server
    SERVER_GROUP_PREY = 3,      // Fourth server
    SERVER_GROUP_CASILLAS = 4,  // Fifth server
    SERVER_GROUP_MAX = 5        // Total server count
};

#endif // DATABASE_DB_ENUMS_H
