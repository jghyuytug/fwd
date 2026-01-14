#ifndef GAME_PVP_PVP_ASSAULT_H
#define GAME_PVP_PVP_ASSAULT_H



// PVP Assault Mode System Namespace
// Confidence: 75% - Partial structure found
// Error count: 996
// Evidence: part01.c:72064-72067, part06.c:67743-68118
namespace pvp_assault {
    // Constants
    extern int ASSAULT_RECT_PLAYER_POINT[16];  // Player spawn positions (x,y pairs)
    extern int ASSAULT_RECT_ICON_POINT;        // Icon position (130)

    // Static spawn disposition
    class CStaticDisposition {
    public:
        int SetVirtualMap(int mapId);
    };

    // Dynamic spawn disposition
    class CDynamicDisposition {
    public:
        int SetVirtualMap(int mapId, int flags);
    };

    // Main assault manager
    class CAssaultMgr {
    public:
        int GetRequestAssaultNeedMoney(int placeId, int level);
    private:
        static char _GetNextAssaultPlaceId_count[];
    };

    // Assault place factory
    // Creates assault battle arenas
    class CAssaultPlaceFactory {
    public:
        void* CreatePlace(int placeType, int mapId);    // Create arena instance
        void DestroyPlace(void* place);                  // Cleanup arena
        int GetPlaceCount();                             // Active arenas
        void UpdateAll(float deltaTime);                 // Tick all arenas
        /* Factory pattern for assault mode arena management */
    };

    // Global access functions
    CAssaultMgr* GetInstanceAssaultMgr();
    CAssaultPlaceFactory* GetInstanceAssaultPlaceFactory();
    int GetDirectionByTeam(int team);

} // namespace pvp_assault

#endif // GAME_PVP_PVP_ASSAULT_H
