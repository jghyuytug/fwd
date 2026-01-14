#ifndef GAME_DUNGEON_MONSTER_TYPES_H
#define GAME_DUNGEON_MONSTER_TYPES_H



// Monster/NPC on dungeon map
// Confidence: 85% - Complete class interface found
// Error count: 1,824
// Evidence: part13.c usage patterns, decl.h:9064-9103
class map_monster {
public:
    // Constructors/Destructors
    map_monster();
    map_monster(const map_monster& other);
    ~map_monster();

    // Operators
    map_monster& operator=(const map_monster& other);

    // Methods
    bool isNamedMonster() const;            // Check if boss/named monster

private:
    // Member layout (speculative based on usage patterns):
    int monsterId;          // Monster template ID
    int posX, posY;         // Position on map
    int currentHp;          // Current health
    int maxHp;              // Maximum health
    int aiState;            // AI behavior state
    int lootTableId;        // Loot table reference
    int spawnGroupId;       // Spawn group for respawn
    /* Full layout requires binary analysis */
};

#endif // GAME_DUNGEON_MONSTER_TYPES_H
