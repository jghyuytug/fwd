/**
 * Party Manager Module - Header
 * Purpose: Manage party lifecycle (create, join, leave, disband)
 *
 * Phase: 6.3 - Party System Implementation
 * Created: 2025-11-23
 */

#ifndef INCLUDE_PARTY_PARTY_MANAGER_H_
#define INCLUDE_PARTY_PARTY_MANAGER_H_

#include <common/defs.h>
#include "../../src/party/party_interface.h"
#include "../../src/database/headers/db_connection_manager.h"

/**
 * Initialize party manager
 * @param db_manager Database connection manager
 * @param max_parties Maximum concurrent parties
 * @return 0 on success
 */
int PartyManager_Initialize(DBConnectionManager* db_manager, int max_parties);

/**
 * Create a new party
 * @param leader_id Leader character ID
 * @param party_type Party type
 * @param party_name Party name (optional, can be NULL)
 * @param out_party Output party structure (optional)
 * @return Party ID on success, negative error code on failure
 */
int PartyManager_Create(int leader_id, PartyType party_type, const char* party_name, PartyInfo* out_party);

/**
 * Invite character to party
 * @param party_id Party ID
 * @param leader_id Leader character ID (must match party leader)
 * @param invitee_id Character ID to invite
 * @return 0 on success
 */
int PartyManager_Invite(int party_id, int leader_id, int invitee_id);

/**
 * Accept party invitation
 * @param party_id Party ID
 * @param character_id Character ID
 * @return 0 on success
 */
int PartyManager_AcceptInvite(int party_id, int character_id);

/**
 * Join party (for open parties)
 * @param party_id Party ID
 * @param character_id Character ID
 * @param password Party password (if required)
 * @return 0 on success
 */
int PartyManager_Join(int party_id, int character_id, const char* password);

/**
 * Leave party
 * @param party_id Party ID
 * @param character_id Character ID
 * @return 0 on success
 */
int PartyManager_Leave(int party_id, int character_id);

/**
 * Kick member from party
 * @param party_id Party ID
 * @param leader_id Leader character ID (must match party leader)
 * @param target_id Character ID to kick
 * @return 0 on success
 */
int PartyManager_Kick(int party_id, int leader_id, int target_id);

/**
 * Disband party
 * @param party_id Party ID
 * @param leader_id Leader character ID (must match party leader)
 * @return 0 on success
 */
int PartyManager_Disband(int party_id, int leader_id);

/**
 * Transfer leader role
 * @param party_id Party ID
 * @param current_leader_id Current leader character ID
 * @param new_leader_id New leader character ID
 * @return 0 on success
 */
int PartyManager_TransferLeader(int party_id, int current_leader_id, int new_leader_id);

/**
 * Get party information
 * @param party_id Party ID
 * @param out_party Output party structure
 * @return 0 on success
 */
int PartyManager_GetParty(int party_id, PartyInfo* out_party);

/**
 * Get character's party ID
 * @param character_id Character ID
 * @return Party ID, or 0 if not in party
 */
int PartyManager_GetCharacterParty(int character_id);

/**
 * Get all party members
 * @param party_id Party ID
 * @param members Output array of members
 * @param max_count Maximum members to retrieve
 * @return Number of members
 */
int PartyManager_GetMembers(int party_id, PartyMember* members, int max_count);

/**
 * Set party settings
 * @param party_id Party ID
 * @param leader_id Leader character ID (must match party leader)
 * @param settings Party settings
 * @return 0 on success
 */
int PartyManager_SetSettings(int party_id, int leader_id, const PartySettings* settings);

/**
 * Update member status (HP/MP)
 * @param character_id Character ID
 * @param current_hp Current HP
 * @param max_hp Maximum HP
 * @param current_mp Current MP
 * @param max_mp Maximum MP
 * @return 0 on success
 */
int PartyManager_UpdateMemberStatus(int character_id, int current_hp, int max_hp, int current_mp, int max_mp);

/**
 * Set database manager for party persistence
 * @param db_manager Database connection manager
 */
void PartyManager_SetDatabaseManager(DBConnectionManager* db_manager);

/**
 * Cleanup party manager
 */
void PartyManager_Cleanup(void);

#endif /* INCLUDE_PARTY_PARTY_MANAGER_H_ */
