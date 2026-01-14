/* Master include file for generated headers */
/* Only includes actually existing files */

#ifndef ALL_GENERATED_H
#define ALL_GENERATED_H

/* ========================================
 * Character System
 * ======================================== */
#include "character/character_enums.h"
#include "character/character_rewards.h"
#include "character/character_types.h"

/* ========================================
 * Common Utilities
 * ======================================== */
#include "common/constants.h"
#include "common/constants_improved.h"
#include "common/containers.h"
#include "common/error_codes.h"
#include "common/memory_pools.h"
#include "common/utilities.h"
#include "common/forward_declarations.h"
#include "common/missing_enums.h"

/* ========================================
 * Core Namespaces
 * ======================================== */
#include "common/arad_namespace.h"
#include "common/wongwork_namespace.h"
#include "common/quickparty_namespace.h"
#include "common/nexon_namespace.h"

/* ========================================
 * Data Structures & Parsing
 * ======================================== */
#include "data/dnflex_stream.h"
#include "data/flex_tables.h"
#include "data/globals.h"
#include "data/packset_stub.h"
#include "data/rdar_string_manager_stub.h"
#include "data/scanner.h"
#include "data/stream_loaders.h"
#include "data/stream_manager.h"
#include "data/stream_utils.h"

/* ========================================
 * Database System (existing files only)
 * ======================================== */
#include "database/db_enums.h"
#include "database/db_types.h"

/* ========================================
 * Effects System
 * ======================================== */
#include "effects/effect_types.h"

/* ========================================
 * Events System
 * ======================================== */
#include "events/event_base.h"
#include "events/event_scripts.h"
#include "events/event_types.h"

/* ========================================
 * Game Systems
 * ======================================== */
// Altar
#include "game/altar/advancealtar.h"
#include "game/altar/altar_types.h"

// Dimension
#include "game/dimension/dimension_types.h"

// Dungeon
#include "game/dungeon/dungeon_types.h"
#include "game/dungeon/monster_types.h"

// PVP
#include "game/pvp/pvp_assault.h"

// Social
#include "game/social/social_types.h"

// Upgrade
#include "game/upgrade/upgrade_types.h"

/* ========================================
 * Inventory System
 * ======================================== */
#include "inventory/inventory_enums.h"
#include "inventory/inventory_types.h"

/* ========================================
 * Network System
 * ======================================== */
#include "network/packet_enums.h"
#include "network/packet_types.h"

/* ========================================
 * Rewards System
 * ======================================== */
#include "rewards/ap_system.h"
#include "rewards/reward_types.h"

/* ========================================
 * Shop System
 * ======================================== */
#include "shop/cerashop.h"
#include "shop/nexon_cash.h"
#include "shop/taiwan_cash.h"

/* ========================================
 * UI System
 * ======================================== */
#include "ui/advertisement.h"
#include "ui/visual_types.h"

#endif // ALL_GENERATED_H

/* Phase 5.3: Type completion headers */
#include "item/CItem.h"
#include "network/PacketDispatcher.h"
#include "network/InterDispatcher.h"
#include "timer/TimerQueue.h"
#include "timer/TimerDispatcher.h"
#include "game/TOD_Layer.h"
#include "game/EpollHandler.h"
