/**
 * 玩家相关类型定义
 */

// 玩家搜索参数
export interface PlayerSearchParams {
  keyword: string;
  search_type: 'account' | 'character' | 'character_id';
  fuzzy?: boolean;
}

// 玩家搜索结果
export interface PlayerSearchResult {
  character_id: number;
  character_name: string;
  account_id: number;
  account_name: string;
  level: number;
  job: string;
  job_name: string;
  grow_type?: string;
  last_login?: string;
  status: 'active' | 'banned' | 'suspended';
}

// 玩家账号信息
export interface PlayerAccount {
  account_id: number;
  account_name: string;
  status: 'active' | 'banned' | 'suspended';
  ban_reason?: string;
  ban_until?: string;
  created_at: string;
  last_login?: string;
  cera_point?: number; // 点券
  mileage?: number; // 里程
}

// 角色信息
export interface Character {
  character_id: number;
  character_name: string;
  level: number;
  job: string;
  job_name: string;
  grow_type?: string;
  grow_type_name?: string;
  gold: number;
  exp: number;
  max_exp: number;
  hp: number;
  max_hp: number;
  mp: number;
  max_mp: number;
  created_at: string;
  last_login?: string;
  playtime?: number; // 游戏时长（秒）
}

// 装备槽位类型
export type EquipmentSlot =
  | 'weapon' // 武器
  | 'top' // 上衣
  | 'bottom' // 下装
  | 'head' // 头肩
  | 'shoes' // 鞋子
  | 'belt' // 腰带
  | 'necklace' // 项链
  | 'bracelet' // 手镯
  | 'ring' // 戒指
  | 'earring' // 耳环
  | 'support' // 辅助装备
  | 'magic_stone'; // 魔法石

// 装备信息
export interface Equipment {
  slot: EquipmentSlot;
  slot_name: string;
  item_id: number;
  item_name: string;
  item_rarity: 'common' | 'uncommon' | 'rare' | 'unique' | 'epic' | 'legendary';
  upgrade_level: number; // 强化等级
  amplify_level?: number; // 增幅等级
  enchant_cards?: {
    // 附魔卡片
    position: 'top' | 'bottom' | 'left' | 'right';
    card_id: number;
    card_name: string;
  }[];
  seal_flag?: boolean; // 是否封装
}

// 仓库物品
export interface WarehouseItem {
  slot: number;
  item_id: number;
  item_name: string;
  item_rarity: 'common' | 'uncommon' | 'rare' | 'unique' | 'epic' | 'legendary';
  quantity: number;
  upgrade_level?: number;
  expire_time?: string;
}

// 任务状态
export type QuestStatus = 'in_progress' | 'completed' | 'failed' | 'available';

// 任务信息
export interface Quest {
  quest_id: number;
  quest_name: string;
  quest_type: 'main' | 'side' | 'daily' | 'achievement';
  status: QuestStatus;
  progress: number;
  max_progress: number;
  reward_exp?: number;
  reward_gold?: number;
  reward_items?: {
    item_id: number;
    item_name: string;
    quantity: number;
  }[];
}

// 玩家详情
export interface PlayerDetail {
  account: PlayerAccount;
  character: Character;
  equipment: Equipment[];
  warehouse: WarehouseItem[];
  quests: Quest[];
}
