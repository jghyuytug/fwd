/**
 * 加密工具类
 * 提供密码哈希、设备指纹等加密功能
 */

import bcrypt from 'bcryptjs';
import crypto from 'crypto';

const SALT_ROUNDS = 10;

/**
 * 密码哈希
 */
export async function hashPassword(password: string): Promise<string> {
  return bcrypt.hash(password, SALT_ROUNDS);
}

/**
 * 验证密码
 */
export async function verifyPassword(password: string, hash: string): Promise<boolean> {
  return bcrypt.compare(password, hash);
}

/**
 * 生成随机Token
 * @param length Token长度（字节）
 */
export function generateRandomToken(length: number = 32): string {
  return crypto.randomBytes(length).toString('hex');
}

/**
 * 生成设备Token
 */
export function generateDeviceToken(): string {
  const tokenLength = parseInt(process.env.DEVICE_TOKEN_LENGTH || '64');
  return generateRandomToken(tokenLength);
}

/**
 * 生成SHA256哈希
 */
export function sha256(data: string): string {
  return crypto.createHash('sha256').update(data).digest('hex');
}

/**
 * 验证设备指纹
 */
export function validateDeviceFingerprint(fingerprint: string): boolean {
  // 验证格式：64字符的十六进制字符串（SHA256）
  return /^[a-f0-9]{64}$/i.test(fingerprint);
}
