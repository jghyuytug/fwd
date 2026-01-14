#ifndef GAME_SOCIAL_SOCIAL_TYPES_H
#define GAME_SOCIAL_SOCIAL_TYPES_H



// War room - Team PvP battle room
// Confidence: 30%
// Error count: 12
// Forward declaration - full interface in game/pvp/warroom.h
class WarRoom {
public:
    virtual ~WarRoom() {}
    virtual int CreateRoom(int host_id, int max_players) = 0;
    virtual int JoinRoom(int player_id) = 0;
    virtual void LeaveRoom(int player_id) = 0;
    virtual int StartMatch() = 0;
    /* Full implementation pending - see ENUM_CMDPACKET_MAKE_WARROOM_TEMP (92-96) */
};

// Trade space (auction house)
// Confidence: 35%
// Error count: 12
// Auction house trading system interface
class CTradeSpace {
public:
    virtual ~CTradeSpace() {}
    virtual int ListItem(int seller_id, int item_id, int price, int duration) = 0;
    virtual int BuyItem(int buyer_id, int listing_id) = 0;
    virtual int CancelListing(int seller_id, int listing_id) = 0;
    virtual int SearchItems(const char* keyword, int category, int* out_results) = 0;
    /* ORM: taiwan_cain_auction_gold, taiwan_cain_auction_cera tables */
};

#endif // GAME_SOCIAL_SOCIAL_TYPES_H
