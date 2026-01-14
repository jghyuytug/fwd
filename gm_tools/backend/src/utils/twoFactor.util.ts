/**
 * 双因素认证（2FA）工具类
 * 使用TOTP（Time-based One-Time Password）
 */

import speakeasy from 'speakeasy';
import QRCode from 'qrcode';
import { logger } from './logger.util';

const ISSUER = process.env.TWO_FACTOR_ISSUER || 'DNF_GM_Tools';
const WINDOW = parseInt(process.env.TWO_FACTOR_WINDOW || '2');

/**
 * 生成2FA密钥
 */
export function generateTwoFactorSecret(username: string): {
  secret: string;
  otpauthUrl: string;
} {
  const secret = speakeasy.generateSecret({
    name: `${ISSUER} (${username})`,
    issuer: ISSUER,
    length: 32
  });

  return {
    secret: secret.base32,
    otpauthUrl: secret.otpauth_url || ''
  };
}

/**
 * 生成QR码（Base64图片）
 */
export async function generateQRCode(otpauthUrl: string): Promise<string> {
  try {
    return await QRCode.toDataURL(otpauthUrl);
  } catch (error) {
    logger.error('生成QR码失败', { error });
    throw new Error('生成QR码失败');
  }
}

/**
 * 验证2FA代码
 */
export function verifyTwoFactorCode(secret: string, code: string): boolean {
  try {
    return speakeasy.totp.verify({
      secret,
      encoding: 'base32',
      token: code,
      window: WINDOW
    });
  } catch (error) {
    logger.error('2FA验证失败', { error });
    return false;
  }
}

/**
 * 生成备份代码
 * @param count 备份代码数量
 */
export function generateBackupCodes(count: number = 10): string[] {
  const codes: string[] = [];

  for (let i = 0; i < count; i++) {
    // 生成格式：XXXX-XXXX-XXXX-XXXX
    const code = Array(4)
      .fill(0)
      .map(() => Math.random().toString(36).substring(2, 6).toUpperCase())
      .join('-');

    codes.push(code);
  }

  return codes;
}

/**
 * 验证备份代码格式
 */
export function validateBackupCodeFormat(code: string): boolean {
  // 格式：XXXX-XXXX-XXXX-XXXX
  return /^[A-Z0-9]{4}-[A-Z0-9]{4}-[A-Z0-9]{4}-[A-Z0-9]{4}$/.test(code);
}
