/**
 * 装备数据模型
 * 查询角色装备、背包、仓库等物品信息
 */

import { cainPool } from '../config/database';
import { RowDataPacket } from 'mysql2';

/**
 * 装备槽位枚举
 */
export enum EquipSlot {
  WEAPON = 0,           // 武器
  TITLE = 1,            // 称号
  JACKET = 2,           // 上衣
  PANTS = 3,            // 下装
  SHOULDER = 4,         // 护肩
  BELT = 5,             // 腰带
  SHOES = 6,            // 鞋子
  BRACELET = 7,         // 手镯
  NECKLACE = 8,         // 项链
  RING = 9,             // 戒指
  SUPPORT = 10,         // 辅助装备
  MAGIC_STONE = 11,     // 魔法石
  EARRING = 12,         // 耳环
  AURA = 13,            // 光环
  CREATURE = 14,        // 宠物
  PET_EQUIP = 15        // 宠物装备
}

/**
 * 装备槽位名称
 */
const SLOT_NAMES: Record<number, string> = {
  [EquipSlot.WEAPON]: '武器',
  [EquipSlot.TITLE]: '称号',
  [EquipSlot.JACKET]: '上衣',
  [EquipSlot.PANTS]: '下装',
  [EquipSlot.SHOULDER]: '护肩',
  [EquipSlot.BELT]: '腰带',
  [EquipSlot.SHOES]: '鞋子',
  [EquipSlot.BRACELET]: '手镯',
  [EquipSlot.NECKLACE]: '项链',
  [EquipSlot.RING]: '戒指',
  [EquipSlot.SUPPORT]: '辅助装备',
  [EquipSlot.MAGIC_STONE]: '魔法石',
  [EquipSlot.EARRING]: '耳环',
  [EquipSlot.AURA]: '光环',
  [EquipSlot.CREATURE]: '宠物',
  [EquipSlot.PET_EQUIP]: '宠物装备'
};

/**
 * 装备信息
 */
export interface Equipment {
  equip_slot: number;
  slot_name: string;
  item_id: number;
  item_name: string;
  upgrade_level: number;
  enchant_id: number;
  enchant_name: string | null;
  amplify_option: number;
  strength_bonus: number;
  intelligence_bonus: number;
  vitality_bonus: number;
  spirit_bonus: number;
  physical_attack: number;
  magical_attack: number;
  independent_attack: number;
  physical_defense: number;
  magical_defense: number;
}

/**
 * 物品信息（背包、仓库）
 */
export interface Item {
  slot_no: number;
  item_id: number;
  item_name: string;
  amount: number;
  upgrade_level: number;
  enchant_id: number;
  is_bound: boolean;
  expire_time: Date | null;
  created_at: Date;
}

/**
 * 背包/仓库查询结果
 */
export interface InventoryResult {
  items: Item[];
  total: number;
  page: number;
  page_size: number;
}

/**
 * 获取槽位名称
 */
export function getSlotName(slot: number): string {
  return SLOT_NAMES[slot] || `未知槽位(${slot})`;
}

/**
 * 查询角色装备
 */
export async function findCharacterEquipment(charNo: number): Promise<Equipment[]> {
  const sql = `
    SELECT
      e.equip_slot,
      e.item_id,
      e.upgrade_level,
      e.enchant_id,
      e.amplify_option,
      e.strength_bonus,
      e.intelligence_bonus,
      e.vitality_bonus,
      e.spirit_bonus,
      e.physical_attack,
      e.magical_attack,
      e.independent_attack,
      e.physical_defense,
      e.magical_defense,
      i.item_name
    FROM character_equipment e
    LEFT JOIN item_info i ON e.item_id = i.item_id
    WHERE e.char_no = ?
    ORDER BY e.equip_slot
  `;

  const [rows] = await cainPool.query<RowDataPacket[]>(sql, [charNo]);

  return rows.map(row => ({
    ...row,
    slot_name: getSlotName(row.equip_slot),
    enchant_name: null, // TODO: 从enchant表查询
    item_name: row.item_name || `未知物品(${row.item_id})`
  }));
}

/**
 * 查询角色背包
 */
export async function findCharacterInventory(
  charNo: number,
  page: number = 1,
  pageSize: number = 50
): Promise<InventoryResult> {
  // 查询总数
  const countSql = `
    SELECT COUNT(*) as total
    FROM character_inventory
    WHERE char_no = ? AND item_id > 0
  `;
  const [countRows] = await cainPool.query<RowDataPacket[]>(countSql, [charNo]);
  const total = countRows[0].total;

  // 查询数据
  const offset = (page - 1) * pageSize;
  const sql = `
    SELECT
      inv.slot_no,
      inv.item_id,
      inv.amount,
      inv.upgrade_level,
      inv.enchant_id,
      inv.is_bound,
      inv.expire_time,
      inv.created_at,
      i.item_name
    FROM character_inventory inv
    LEFT JOIN item_info i ON inv.item_id = i.item_id
    WHERE inv.char_no = ? AND inv.item_id > 0
    ORDER BY inv.slot_no
    LIMIT ? OFFSET ?
  `;

  const [rows] = await cainPool.query<RowDataPacket[]>(sql, [charNo, pageSize, offset]);

  const items: Item[] = rows.map(row => ({
    ...row,
    item_name: row.item_name || `未知物品(${row.item_id})`
  }));

  return {
    items,
    total,
    page,
    page_size: pageSize
  };
}

/**
 * 查询角色仓库
 */
export async function findCharacterWarehouse(
  charNo: number,
  page: number = 1,
  pageSize: number = 50
): Promise<InventoryResult> {
  // 查询总数
  const countSql = `
    SELECT COUNT(*) as total
    FROM character_warehouse
    WHERE char_no = ? AND item_id > 0
  `;
  const [countRows] = await cainPool.query<RowDataPacket[]>(countSql, [charNo]);
  const total = countRows[0].total;

  // 查询数据
  const offset = (page - 1) * pageSize;
  const sql = `
    SELECT
      w.slot_no,
      w.item_id,
      w.amount,
      w.upgrade_level,
      w.enchant_id,
      w.is_bound,
      w.expire_time,
      w.created_at,
      i.item_name
    FROM character_warehouse w
    LEFT JOIN item_info i ON w.item_id = i.item_id
    WHERE w.char_no = ? AND w.item_id > 0
    ORDER BY w.slot_no
    LIMIT ? OFFSET ?
  `;

  const [rows] = await cainPool.query<RowDataPacket[]>(sql, [charNo, pageSize, offset]);

  const items: Item[] = rows.map(row => ({
    ...row,
    item_name: row.item_name || `未知物品(${row.item_id})`
  }));

  return {
    items,
    total,
    page,
    page_size: pageSize
  };
}

/**
 * 查询角色金币信息
 */
export async function findCharacterMoney(charNo: number): Promise<{
  gold: bigint;
  silver: bigint;
}> {
  const sql = `
    SELECT
      COALESCE(money, 0) as gold,
      COALESCE(silver, 0) as silver
    FROM charac_info
    WHERE char_no = ?
  `;

  const [rows] = await cainPool.query<RowDataPacket[]>(sql, [charNo]);

  if (rows.length === 0) {
    return {
      gold: BigInt(0),
      silver: BigInt(0)
    };
  }

  return {
    gold: BigInt(rows[0].gold || 0),
    silver: BigInt(rows[0].silver || 0)
  };
}
