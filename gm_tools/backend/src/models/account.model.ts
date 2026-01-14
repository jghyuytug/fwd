/**
 * 账号数据模型
 * 从Login镜像和Cain镜像数据库查询账号信息
 */

import { loginPool, cainPool } from '../config/database';
import { RowDataPacket } from 'mysql2';

/**
 * 账号基础信息
 */
export interface Account {
  m_id: string;
  account_name: string;
  account_type: number;
  cera_point: number;
  mileage_point: number;
  is_banned: boolean;
  ban_expire_time: Date | null;
  register_time: Date;
  last_login_time: Date | null;
}

/**
 * 登录历史记录
 */
export interface LoginHistory {
  login_time: Date;
  logout_time: Date | null;
  login_ip: string;
  online_duration: number; // 秒
}

/**
 * 账号详细信息
 */
export interface AccountDetail extends Account {
  character_count: number;
  total_playtime: number; // 总游戏时长（秒）
  last_login_ip: string | null;
}

/**
 * 根据账号ID查询账号基础信息
 */
export async function findAccountById(mId: string): Promise<Account | null> {
  // 从Login库查询
  const sql = `
    SELECT
      m_id,
      account_name,
      account_type,
      cera_point,
      mileage_point,
      is_banned,
      ban_expire_time,
      register_time,
      last_login_time
    FROM accounts
    WHERE m_id = ?
  `;

  const [rows] = await loginPool.query<RowDataPacket[]>(sql, [mId]);

  if (rows.length === 0) {
    return null;
  }

  return rows[0] as Account;
}

/**
 * 根据账号名查询账号
 */
export async function findAccountByName(accountName: string): Promise<Account | null> {
  const sql = `
    SELECT
      m_id,
      account_name,
      account_type,
      cera_point,
      mileage_point,
      is_banned,
      ban_expire_time,
      register_time,
      last_login_time
    FROM accounts
    WHERE account_name = ?
  `;

  const [rows] = await loginPool.query<RowDataPacket[]>(sql, [accountName]);

  if (rows.length === 0) {
    return null;
  }

  return rows[0] as Account;
}

/**
 * 查询账号登录历史
 */
export async function findLoginHistory(
  mId: string,
  limit: number = 20
): Promise<LoginHistory[]> {
  const sql = `
    SELECT
      login_time,
      logout_time,
      login_ip,
      TIMESTAMPDIFF(SECOND, login_time, COALESCE(logout_time, NOW())) as online_duration
    FROM login_history
    WHERE m_id = ?
    ORDER BY login_time DESC
    LIMIT ?
  `;

  const [rows] = await loginPool.query<RowDataPacket[]>(sql, [mId, limit]);

  return rows as LoginHistory[];
}

/**
 * 检查账号是否存在
 */
export async function checkAccountExists(mId: string): Promise<boolean> {
  const sql = 'SELECT 1 FROM accounts WHERE m_id = ?';
  const [rows] = await loginPool.query<RowDataPacket[]>(sql, [mId]);
  return rows.length > 0;
}

/**
 * 检查账号是否被封禁
 */
export async function isAccountBanned(mId: string): Promise<boolean> {
  const sql = `
    SELECT is_banned, ban_expire_time
    FROM accounts
    WHERE m_id = ?
  `;

  const [rows] = await loginPool.query<RowDataPacket[]>(sql, [mId]);

  if (rows.length === 0) {
    return false;
  }

  const account = rows[0];

  // 如果没有封禁标志，返回false
  if (!account.is_banned) {
    return false;
  }

  // 如果有封禁过期时间，检查是否已过期
  if (account.ban_expire_time) {
    const now = new Date();
    const expireTime = new Date(account.ban_expire_time);
    return now < expireTime;
  }

  // 永久封禁
  return true;
}

/**
 * 获取账号统计信息
 */
export async function getAccountStats(mId: string): Promise<{
  character_count: number;
  total_playtime: number;
  max_level: number;
}> {
  const sql = `
    SELECT
      COUNT(*) as character_count,
      COALESCE(SUM(TIMESTAMPDIFF(SECOND, login_time, COALESCE(logout_time, NOW()))), 0) as total_playtime,
      COALESCE(MAX(lev), 0) as max_level
    FROM charac_info
    WHERE m_id = ? AND delete_flag = 0
  `;

  const [rows] = await cainPool.query<RowDataPacket[]>(sql, [mId]);

  if (rows.length === 0) {
    return {
      character_count: 0,
      total_playtime: 0,
      max_level: 0
    };
  }

  return rows[0] as any;
}
