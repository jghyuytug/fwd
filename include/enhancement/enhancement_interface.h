/**
 * Enhancement Module - Public Interface
 * Purpose: Equipment enhancement, amplification, and enchantment system
 *
 * Components:
 * - Equipment strengthening (+1 to +12+)
 * - Amplification system
 * - Enchantment cards
 * - Refinement and upgrade
 * - Success/failure mechanics
 * - Protection items and safe enhancement
 */

#ifndef INCLUDE_ENHANCEMENT_ENHANCEMENT_INTERFACE_H_
#define INCLUDE_ENHANCEMENT_ENHANCEMENT_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Enhancement types
 */
typedef enum {
    ENHANCE_TYPE_STRENGTHEN = 0,    // Basic strengthening (+1, +2, etc.)
    ENHANCE_TYPE_AMPLIFY = 1,       // Amplification (random stats)
    ENHANCE_TYPE_ENCHANT = 2,       // Enchantment cards
    ENHANCE_TYPE_REFINE = 3         // Refinement
} EnhancementType;

/**
 * Enhancement results
 */
typedef enum {
    ENHANCE_RESULT_SUCCESS = 0,      // Enhancement succeeded
    ENHANCE_RESULT_FAILURE = 1,      // Enhancement failed (item degraded)
    ENHANCE_RESULT_DESTROYED = 2,    // Item destroyed
    ENHANCE_RESULT_PROTECTED = 3,    // Failed but protected from destruction
    ENHANCE_RESULT_GREAT_SUCCESS = 4 // Great success (bonus upgrade)
} EnhancementResult;

/**
 * Amplification option
 */
typedef struct {
    int ability_type;           // Stat type (STR, INT, HP, etc.)
    int ability_value;          // Stat value
} AmplifyOption;

/**
 * Enhancement information
 */
typedef struct {
    unsigned int item_id;
    unsigned int user_id;
    EnhancementType enhance_type;
    int current_level;          // Current +level (0-12+)
    int target_level;           // Target +level
    int amplify_level;          // Amplification level
    AmplifyOption amplify_options[4]; // Up to 4 amplify stats
    int has_enchantment;
    unsigned int enchant_card_id;
    unsigned int last_enhance_time;
} EnhancementInfo;

/**
 * Enhancement statistics
 */
typedef struct {
    unsigned int total_attempts;
    unsigned int total_successes;
    unsigned int total_failures;
    unsigned int items_destroyed;
    unsigned int great_successes;
    unsigned int total_gold_spent;
    unsigned int highest_enhancement_level;
    float success_rate;
} EnhancementStats;

/**
 * Initialize Enhancement Module
 *
 * Sets up enhancement system
 *
 * Parameters:
 *   max_concurrent_enhancements: Maximum concurrent enhancement operations
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Enhancement_Initialize(int max_concurrent_enhancements);

/**
 * Cleanup Enhancement Module
 *
 * Releases enhancement system resources
 */
void Enhancement_Cleanup();

/**
 * Strengthen Equipment
 *
 * Attempts to strengthen equipment (+1, +2, etc.)
 *
 * Parameters:
 *   user_id: User ID
 *   item_id: Item ID to strengthen
 *   use_protection: Whether to use protection item
 *
 * Returns: Enhancement result (ENHANCE_RESULT_*)
 */
int Enhancement_Strengthen(unsigned int user_id, unsigned int item_id,
                           int use_protection);

/**
 * Amplify Equipment
 *
 * Attempts to amplify equipment (add random stats)
 *
 * Parameters:
 *   user_id: User ID
 *   item_id: Item ID to amplify
 *   amplify_level: Target amplification level
 *
 * Returns: Enhancement result (ENHANCE_RESULT_*)
 */
int Enhancement_Amplify(unsigned int user_id, unsigned int item_id,
                        int amplify_level);

/**
 * Apply Enchantment
 *
 * Applies enchantment card to equipment
 *
 * Parameters:
 *   user_id: User ID
 *   item_id: Item ID to enchant
 *   enchant_card_id: Enchantment card ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Enhancement_ApplyEnchant(unsigned int user_id, unsigned int item_id,
                             unsigned int enchant_card_id);

/**
 * Remove Enchantment
 *
 * Removes enchantment from equipment
 *
 * Parameters:
 *   user_id: User ID
 *   item_id: Item ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Enhancement_RemoveEnchant(unsigned int user_id, unsigned int item_id);

/**
 * Get Enhancement Info
 *
 * Retrieves enhancement information for an item
 *
 * Parameters:
 *   item_id: Item ID
 *   enhance_info: Output enhancement structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Enhancement_GetInfo(unsigned int item_id, EnhancementInfo* enhance_info);

/**
 * Calculate Success Rate
 *
 * Calculates success rate for enhancement
 *
 * Parameters:
 *   item_id: Item ID
 *   enhance_type: Enhancement type
 *   target_level: Target enhancement level
 *
 * Returns: Success rate (0-100)
 */
int Enhancement_CalculateSuccessRate(unsigned int item_id,
                                      EnhancementType enhance_type,
                                      int target_level);

/**
 * Calculate Enhancement Cost
 *
 * Calculates gold cost for enhancement
 *
 * Parameters:
 *   item_id: Item ID
 *   enhance_type: Enhancement type
 *   target_level: Target enhancement level
 *
 * Returns: Gold cost
 */
unsigned int Enhancement_CalculateCost(unsigned int item_id,
                                        EnhancementType enhance_type,
                                        int target_level);

/**
 * Get Enhancement Statistics
 *
 * Retrieves enhancement statistics for a player
 *
 * Parameters:
 *   user_id: User ID
 *   stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Enhancement_GetStatistics(unsigned int user_id, EnhancementStats* stats);

/**
 * Get Amplify Options
 *
 * Gets amplification options for an item
 *
 * Parameters:
 *   item_id: Item ID
 *   options: Output array of amplify options
 *   max_options: Maximum options to retrieve
 *
 * Returns: Number of amplify options
 */
int Enhancement_GetAmplifyOptions(unsigned int item_id,
                                   AmplifyOption* options,
                                   int max_options);

/**
 * Reset Enhancement
 *
 * Resets item enhancement to base state
 *
 * Parameters:
 *   user_id: User ID
 *   item_id: Item ID to reset
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Enhancement_Reset(unsigned int user_id, unsigned int item_id);

/**
 * Update Enhancement State
 *
 * Processes enhancement tick
 *
 * Parameters:
 *   delta_time_ms: Time elapsed since last update (milliseconds)
 *
 * Returns: Number of enhancements processed
 */
int Enhancement_Update(int delta_time_ms);

#endif // INCLUDE_ENHANCEMENT_ENHANCEMENT_INTERFACE_H_
