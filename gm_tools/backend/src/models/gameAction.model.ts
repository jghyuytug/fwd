/**
 * 游戏操作模型
 * 执行游戏内的实际操作（物品赠送、封禁、数据修改等）
 */

import { cainPool, loginPool } from '../config/database';
import { ResultSetHeader, RowDataPacket } from 'mysql2';

/**
 * 物品赠送参数
 */
export interface GrantItemParams {
  char_no: number;
  item_id: number;
  amount: number;
  expire_days?: number; // 0表示永久
}

/**
 * 物品赠送结果
 */
export interface GrantItemResult {
  success: boolean;
  slot_no?: number;
  error?: string;
}

/**
 * 账号封禁参数
 */
export interface BanAccountParams {
  m_id: string;
  ban_type: number; // 1-封禁，2-禁言，3-警告
  ban_duration: number; // 秒，0表示永久
  reason: string;
  evidence?: string;
}

/**
 * 等级修改参数
 */
export interface ModifyLevelParams {
  char_no: number;
  new_level: number;
}

/**
 * 金币修改参数
 */
export interface ModifyMoneyParams {
  char_no: number;
  gold?: bigint;
  silver?: bigint;
}

/**
 * 赠送物品到角色背包
 */
export async function grantItemToCharacter(
  params: GrantItemParams
): Promise<GrantItemResult> {
  const { char_no, item_id, amount, expire_days = 0 } = params;

  try {
    // 1. 查找空闲背包槽位
    const findSlotSql = `
      SELECT slot_no
      FROM character_inventory
      WHERE char_no = ? AND (item_id = 0 OR item_id IS NULL)
      ORDER BY slot_no
      LIMIT 1
    `;
    const [slotRows] = await cainPool.query<RowDataPacket[]>(findSlotSql, [char_no]);

    if (slotRows.length === 0) {
      return {
        success: false,
        error: '背包已满，无可用槽位'
      };
    }

    const slotNo = slotRows[0].slot_no;

    // 2. 计算过期时间
    let expireTime = null;
    if (expire_days > 0) {
      const expireDate = new Date();
      expireDate.setDate(expireDate.getDate() + expire_days);
      expireTime = expireDate;
    }

    // 3. 插入物品到背包
    const insertSql = `
      UPDATE character_inventory
      SET
        item_id = ?,
        amount = ?,
        upgrade_level = 0,
        enchant_id = 0,
        is_bound = 0,
        expire_time = ?,
        created_at = NOW()
      WHERE char_no = ? AND slot_no = ?
    `;

    await cainPool.query(insertSql, [item_id, amount, expireTime, char_no, slotNo]);

    return {
      success: true,
      slot_no: slotNo
    };
  } catch (error: any) {
    return {
      success: false,
      error: error.message
    };
  }
}

/**
 * 发送邮件到角色邮箱
 */
export async function sendMailToCharacter(params: {
  char_no: number;
  sender_name: string;
  title: string;
  content: string;
  item_id?: number;
  item_amount?: number;
  gold?: bigint;
  expire_days?: number;
}): Promise<{ success: boolean; mail_id?: number; error?: string }> {
  const {
    char_no,
    sender_name,
    title,
    content,
    item_id = 0,
    item_amount = 0,
    gold = BigInt(0),
    expire_days = 30
  } = params;

  try {
    // 计算过期时间
    const expireDate = new Date();
    expireDate.setDate(expireDate.getDate() + expire_days);

    const sql = `
      INSERT INTO character_mail (
        char_no, sender_name, title, content,
        item_id, item_amount, gold,
        is_read, is_received, expire_time, created_at
      ) VALUES (?, ?, ?, ?, ?, ?, ?, 0, 0, ?, NOW())
    `;

    const [result] = await cainPool.query<ResultSetHeader>(sql, [
      char_no,
      sender_name,
      title,
      content,
      item_id,
      item_amount,
      gold.toString(),
      expireDate
    ]);

    return {
      success: true,
      mail_id: result.insertId
    };
  } catch (error: any) {
    return {
      success: false,
      error: error.message
    };
  }
}

/**
 * 封禁账号
 */
export async function banAccount(params: BanAccountParams): Promise<{
  success: boolean;
  ban_until?: Date;
  error?: string;
}> {
  const { m_id, ban_type, ban_duration, reason, evidence } = params;

  try {
    // 计算封禁过期时间
    let banUntil: Date | null = null;
    if (ban_duration > 0) {
      banUntil = new Date();
      banUntil.setSeconds(banUntil.getSeconds() + ban_duration);
    }

    // 更新账号封禁状态
    const updateSql = `
      UPDATE accounts
      SET
        is_banned = 1,
        ban_type = ?,
        ban_expire_time = ?,
        ban_reason = ?,
        ban_evidence = ?,
        ban_time = NOW()
      WHERE m_id = ?
    `;

    await loginPool.query(updateSql, [ban_type, banUntil, reason, evidence || null, m_id]);

    // 记录封禁历史
    const historySql = `
      INSERT INTO ban_history (
        m_id, ban_type, ban_duration, ban_until,
        reason, evidence, created_at
      ) VALUES (?, ?, ?, ?, ?, ?, NOW())
    `;

    await loginPool.query(historySql, [m_id, ban_type, ban_duration, banUntil, reason, evidence || null]);

    return {
      success: true,
      ban_until: banUntil || undefined
    };
  } catch (error: any) {
    return {
      success: false,
      error: error.message
    };
  }
}

/**
 * 解封账号
 */
export async function unbanAccount(mId: string): Promise<{
  success: boolean;
  error?: string;
}> {
  try {
    const sql = `
      UPDATE accounts
      SET
        is_banned = 0,
        ban_type = NULL,
        ban_expire_time = NULL,
        ban_reason = NULL,
        ban_evidence = NULL
      WHERE m_id = ?
    `;

    await loginPool.query(sql, [mId]);

    return {
      success: true
    };
  } catch (error: any) {
    return {
      success: false,
      error: error.message
    };
  }
}

/**
 * 修改角色等级
 */
export async function modifyCharacterLevel(
  params: ModifyLevelParams
): Promise<{ success: boolean; old_level?: number; error?: string }> {
  const { char_no, new_level } = params;

  try {
    // 查询当前等级
    const querySql = 'SELECT lev FROM charac_info WHERE char_no = ?';
    const [rows] = await cainPool.query<RowDataPacket[]>(querySql, [char_no]);

    if (rows.length === 0) {
      return {
        success: false,
        error: '角色不存在'
      };
    }

    const oldLevel = rows[0].lev;

    // 更新等级
    const updateSql = `
      UPDATE charac_info
      SET lev = ?
      WHERE char_no = ?
    `;

    await cainPool.query(updateSql, [new_level, char_no]);

    return {
      success: true,
      old_level: oldLevel
    };
  } catch (error: any) {
    return {
      success: false,
      error: error.message
    };
  }
}

/**
 * 修改角色金币
 */
export async function modifyCharacterMoney(
  params: ModifyMoneyParams
): Promise<{ success: boolean; old_money?: { gold: bigint; silver: bigint }; error?: string }> {
  const { char_no, gold, silver } = params;

  try {
    // 查询当前金币
    const querySql = 'SELECT money, silver FROM charac_info WHERE char_no = ?';
    const [rows] = await cainPool.query<RowDataPacket[]>(querySql, [char_no]);

    if (rows.length === 0) {
      return {
        success: false,
        error: '角色不存在'
      };
    }

    const oldMoney = {
      gold: BigInt(rows[0].money || 0),
      silver: BigInt(rows[0].silver || 0)
    };

    // 构建更新SQL
    const updates: string[] = [];
    const values: any[] = [];

    if (gold !== undefined) {
      updates.push('money = ?');
      values.push(gold.toString());
    }

    if (silver !== undefined) {
      updates.push('silver = ?');
      values.push(silver.toString());
    }

    if (updates.length === 0) {
      return {
        success: false,
        error: '没有需要修改的金币类型'
      };
    }

    values.push(char_no);

    const updateSql = `
      UPDATE charac_info
      SET ${updates.join(', ')}
      WHERE char_no = ?
    `;

    await cainPool.query(updateSql, values);

    return {
      success: true,
      old_money: oldMoney
    };
  } catch (error: any) {
    return {
      success: false,
      error: error.message
    };
  }
}

/**
 * 检查物品ID是否有效
 */
export async function checkItemExists(itemId: number): Promise<boolean> {
  const sql = 'SELECT 1 FROM item_info WHERE item_id = ?';
  const [rows] = await cainPool.query<RowDataPacket[]>(sql, [itemId]);
  return rows.length > 0;
}

/**
 * 获取物品名称
 */
export async function getItemName(itemId: number): Promise<string | null> {
  const sql = 'SELECT item_name FROM item_info WHERE item_id = ?';
  const [rows] = await cainPool.query<RowDataPacket[]>(sql, [itemId]);
  return rows.length > 0 ? rows[0].item_name : null;
}
