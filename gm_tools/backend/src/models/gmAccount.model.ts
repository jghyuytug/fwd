/**
 * GM账号数据模型
 * 负责GM账号相关的数据库操作
 */

import { gmToolsPool } from '../config/database';
import { RowDataPacket, ResultSetHeader } from 'mysql2/promise';
import { logger } from '../utils/logger.util';

export interface GmAccount {
  gm_id: number;
  username: string;
  password_hash: string;
  real_name: string | null;
  email: string | null;
  phone: string | null;
  gm_level: number;
  department: string | null;
  two_factor_enabled: boolean;
  two_factor_secret: string | null;
  backup_codes: string | null;
  is_active: boolean;
  is_locked: boolean;
  locked_until: Date | null;
  failed_login_attempts: number;
  created_at: Date;
  created_by: string | null;
  updated_at: Date;
  updated_by: string | null;
  last_login_at: Date | null;
  last_login_ip: string | null;
}

export interface GmLoginLog {
  log_id: number;
  gm_user: string;
  gm_id: number;
  login_result: 'SUCCESS' | 'FAILURE' | 'LOCKED';
  failure_reason: string | null;
  two_factor_used: boolean;
  two_factor_result: 'SUCCESS' | 'FAILURE' | null;
  ip_address: string;
  user_agent: string | null;
  device_fingerprint: string | null;
  created_at: Date;
}

export interface TrustedDevice {
  device_id: number;
  gm_user: string;
  gm_id: number;
  device_fingerprint: string;
  device_token: string;
  device_name: string | null;
  ip_address: string;
  user_agent: string | null;
  created_at: Date;
  expires_at: Date;
  last_used_at: Date | null;
  is_active: boolean;
}

/**
 * 根据用户名查询GM账号
 */
export async function findGmAccountByUsername(username: string): Promise<GmAccount | null> {
  try {
    const [rows] = await gmToolsPool.query<RowDataPacket[]>(
      'SELECT * FROM gm_accounts WHERE username = ? AND is_active = 1',
      [username]
    );

    if (rows.length === 0) {
      return null;
    }

    return rows[0] as GmAccount;
  } catch (error) {
    logger.error('查询GM账号失败', { username, error });
    throw error;
  }
}

/**
 * 根据ID查询GM账号
 */
export async function findGmAccountById(gmId: number): Promise<GmAccount | null> {
  try {
    const [rows] = await gmToolsPool.query<RowDataPacket[]>(
      'SELECT * FROM gm_accounts WHERE gm_id = ?',
      [gmId]
    );

    if (rows.length === 0) {
      return null;
    }

    return rows[0] as GmAccount;
  } catch (error) {
    logger.error('查询GM账号失败', { gmId, error });
    throw error;
  }
}

/**
 * 更新登录失败次数
 */
export async function incrementFailedLoginAttempts(gmId: number): Promise<void> {
  try {
    const maxAttempts = parseInt(process.env.LOGIN_MAX_FAILED_ATTEMPTS || '5');
    const lockoutDuration = parseInt(process.env.LOGIN_LOCKOUT_DURATION || '600');

    await gmToolsPool.query(
      `UPDATE gm_accounts
       SET failed_login_attempts = failed_login_attempts + 1,
           is_locked = CASE
             WHEN failed_login_attempts + 1 >= ? THEN 1
             ELSE is_locked
           END,
           locked_until = CASE
             WHEN failed_login_attempts + 1 >= ? THEN DATE_ADD(NOW(), INTERVAL ? SECOND)
             ELSE locked_until
           END
       WHERE gm_id = ?`,
      [maxAttempts, maxAttempts, lockoutDuration, gmId]
    );
  } catch (error) {
    logger.error('更新登录失败次数失败', { gmId, error });
    throw error;
  }
}

/**
 * 重置登录失败次数
 */
export async function resetFailedLoginAttempts(gmId: number): Promise<void> {
  try {
    await gmToolsPool.query(
      `UPDATE gm_accounts
       SET failed_login_attempts = 0,
           is_locked = 0,
           locked_until = NULL
       WHERE gm_id = ?`,
      [gmId]
    );
  } catch (error) {
    logger.error('重置登录失败次数失败', { gmId, error });
    throw error;
  }
}

/**
 * 更新最后登录信息
 */
export async function updateLastLogin(gmId: number, ipAddress: string): Promise<void> {
  try {
    await gmToolsPool.query(
      `UPDATE gm_accounts
       SET last_login_at = NOW(),
           last_login_ip = ?
       WHERE gm_id = ?`,
      [ipAddress, gmId]
    );
  } catch (error) {
    logger.error('更新最后登录信息失败', { gmId, error });
    throw error;
  }
}

/**
 * 记录登录日志
 */
export async function createLoginLog(data: {
  gm_user: string;
  gm_id: number;
  login_result: 'SUCCESS' | 'FAILURE' | 'LOCKED';
  failure_reason?: string;
  two_factor_used?: boolean;
  two_factor_result?: 'SUCCESS' | 'FAILURE';
  ip_address: string;
  user_agent?: string;
  device_fingerprint?: string;
}): Promise<number> {
  try {
    const [result] = await gmToolsPool.query<ResultSetHeader>(
      `INSERT INTO gm_login_logs (
        gm_user, gm_id, login_result, failure_reason,
        two_factor_used, two_factor_result,
        ip_address, user_agent, device_fingerprint
      ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)`,
      [
        data.gm_user,
        data.gm_id,
        data.login_result,
        data.failure_reason || null,
        data.two_factor_used || false,
        data.two_factor_result || null,
        data.ip_address,
        data.user_agent || null,
        data.device_fingerprint || null
      ]
    );

    return result.insertId;
  } catch (error) {
    logger.error('记录登录日志失败', { data, error });
    throw error;
  }
}

/**
 * 查询信任设备（通过设备Token）
 */
export async function findTrustedDeviceByToken(deviceToken: string): Promise<TrustedDevice | null> {
  try {
    const [rows] = await gmToolsPool.query<RowDataPacket[]>(
      `SELECT * FROM gm_trusted_devices
       WHERE device_token = ?
       AND is_active = 1
       AND expires_at > NOW()`,
      [deviceToken]
    );

    if (rows.length === 0) {
      return null;
    }

    return rows[0] as TrustedDevice;
  } catch (error) {
    logger.error('查询信任设备失败', { deviceToken, error });
    throw error;
  }
}

/**
 * 创建信任设备
 */
export async function createTrustedDevice(data: {
  gm_user: string;
  gm_id: number;
  device_fingerprint: string;
  device_token: string;
  device_name?: string;
  ip_address: string;
  user_agent?: string;
}): Promise<number> {
  try {
    const trustDuration = parseInt(process.env.DEVICE_TRUST_DURATION || '2592000'); // 30天

    const [result] = await gmToolsPool.query<ResultSetHeader>(
      `INSERT INTO gm_trusted_devices (
        gm_user, gm_id, device_fingerprint, device_token,
        device_name, ip_address, user_agent, expires_at
      ) VALUES (?, ?, ?, ?, ?, ?, ?, DATE_ADD(NOW(), INTERVAL ? SECOND))`,
      [
        data.gm_user,
        data.gm_id,
        data.device_fingerprint,
        data.device_token,
        data.device_name || null,
        data.ip_address,
        data.user_agent || null,
        trustDuration
      ]
    );

    return result.insertId;
  } catch (error) {
    logger.error('创建信任设备失败', { data, error });
    throw error;
  }
}

/**
 * 更新信任设备最后使用时间
 */
export async function updateTrustedDeviceLastUsed(deviceId: number): Promise<void> {
  try {
    await gmToolsPool.query(
      'UPDATE gm_trusted_devices SET last_used_at = NOW() WHERE device_id = ?',
      [deviceId]
    );
  } catch (error) {
    logger.error('更新设备使用时间失败', { deviceId, error });
    throw error;
  }
}

/**
 * 查询用户的所有信任设备
 */
export async function findTrustedDevicesByGmId(gmId: number): Promise<TrustedDevice[]> {
  try {
    const [rows] = await gmToolsPool.query<RowDataPacket[]>(
      `SELECT * FROM gm_trusted_devices
       WHERE gm_id = ? AND is_active = 1
       ORDER BY created_at DESC`,
      [gmId]
    );

    return rows as TrustedDevice[];
  } catch (error) {
    logger.error('查询信任设备列表失败', { gmId, error });
    throw error;
  }
}

/**
 * 撤销信任设备
 */
export async function revokeTrustedDevice(deviceId: number, gmId: number): Promise<void> {
  try {
    await gmToolsPool.query(
      'UPDATE gm_trusted_devices SET is_active = 0 WHERE device_id = ? AND gm_id = ?',
      [deviceId, gmId]
    );
  } catch (error) {
    logger.error('撤销信任设备失败', { deviceId, gmId, error });
    throw error;
  }
}

/**
 * 启用2FA
 */
export async function enableTwoFactor(gmId: number, secret: string, backupCodes: string): Promise<void> {
  try {
    await gmToolsPool.query(
      `UPDATE gm_accounts
       SET two_factor_enabled = 1,
           two_factor_secret = ?,
           backup_codes = ?
       WHERE gm_id = ?`,
      [secret, backupCodes, gmId]
    );
  } catch (error) {
    logger.error('启用2FA失败', { gmId, error });
    throw error;
  }
}

/**
 * 禁用2FA
 */
export async function disableTwoFactor(gmId: number): Promise<void> {
  try {
    await gmToolsPool.query(
      `UPDATE gm_accounts
       SET two_factor_enabled = 0,
           two_factor_secret = NULL,
           backup_codes = NULL
       WHERE gm_id = ?`,
      [gmId]
    );
  } catch (error) {
    logger.error('禁用2FA失败', { gmId, error });
    throw error;
  }
}
