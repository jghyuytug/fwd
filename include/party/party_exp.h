/**
 * Party Experience Distribution System - Header
 * Purpose: Calculate and distribute experience among party members
 *
 * Phase: 6.3 - Party System Implementation
 * Created: 2025-11-23
 */

#ifndef INCLUDE_PARTY_PARTY_EXP_H_
#define INCLUDE_PARTY_PARTY_EXP_H_

#include <common/defs.h>

/**
 * Experience distribution mode
 */
typedef enum {
    EXP_DISTRIBUTE_AVERAGE = 0,      // Equal distribution among all members
    EXP_DISTRIBUTE_DAMAGE = 1,       // Distribute based on damage contribution
    EXP_DISTRIBUTE_LEVEL = 2         // Distribute with level difference adjustment
} ExpDistributeType;

/**
 * Member experience share
 */
typedef struct {
    int character_id;
    int exp_amount;
    float share_percentage;
} MemberExpShare;

/**
 * Initialize party experience system
 * @return 0 on success
 */
int PartyExp_Initialize(void);

/**
 * Calculate experience distribution for party members
 * @param party_id Party ID
 * @param base_exp Base experience to distribute
 * @param distribute_type Distribution type
 * @param out_shares Output array of member shares
 * @param max_shares Maximum number of shares
 * @return Number of shares calculated
 */
int PartyExp_Calculate(int party_id, int base_exp, ExpDistributeType distribute_type,
                       MemberExpShare* out_shares, int max_shares);

/**
 * Distribute experience to party members
 * @param party_id Party ID
 * @param base_exp Base experience to distribute
 * @param distribute_type Distribution type
 * @return Total experience distributed
 */
int PartyExp_Distribute(int party_id, int base_exp, ExpDistributeType distribute_type);

/**
 * Calculate party experience bonus based on member count
 * @param member_count Number of party members
 * @return Bonus percentage (0.0 to 1.0)
 */
float PartyExp_GetPartyBonus(int member_count);

/**
 * Calculate level difference penalty
 * @param member_level Member level
 * @param monster_level Monster/content level
 * @return Penalty multiplier (0.0 to 1.0)
 */
float PartyExp_GetLevelPenalty(int member_level, int monster_level);

/**
 * Set member damage contribution (for damage-based distribution)
 * @param character_id Character ID
 * @param damage Damage dealt
 * @return 0 on success
 */
int PartyExp_SetMemberDamage(int character_id, int damage);

/**
 * Clear damage contributions (call before new combat)
 * @return 0 on success
 */
int PartyExp_ClearDamageContributions(void);

/**
 * Cleanup party experience system
 */
void PartyExp_Cleanup(void);

#endif /* INCLUDE_PARTY_PARTY_EXP_H_ */
