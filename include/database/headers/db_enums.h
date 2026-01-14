#ifndef DATABASE_DB_ENUMS_H
#define DATABASE_DB_ENUMS_H



// Database connection handle indices
// Confidence: 85% - Clear usage in nested std::map
// Evidence: std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo>>
enum ENUM_DB_HANDLE_IDX {
    DB_HANDLE_ACCOUNT = 0,      // Account database (d_taiwan)
    DB_HANDLE_CHARACTER = 1,    // Character database (taiwan_cain)
    DB_HANDLE_GAME = 2,         // Game state database (taiwan_cain)
    DB_HANDLE_LOG = 3,          // Logging database (taiwan_cain_log)
    DB_HANDLE_CASH = 4,         // Cash shop database (taiwan_billing)
    DB_HANDLE_GUILD = 5,        // Guild database (d_guild)
    DB_HANDLE_EVENT = 6,        // Event database (taiwan_game_event)
    // Matches DB_TYPE_MAX_INDEX = 15 in DBConnectionManager
};

// Server group enumeration
// Confidence: 85% - Used as map key for DB connections
enum ENUM_SERVER_GROUP {
    SERVER_GROUP_CAIN = 0,      // Cain server group
    SERVER_GROUP_DIREGIE = 1,   // Diregie server group
    SERVER_GROUP_SIROCO = 2,    // Siroco server group
    SERVER_GROUP_PREY = 3,      // Prey server group
    // Taiwan region typically has 4 server groups
};

#endif // DATABASE_DB_ENUMS_H
