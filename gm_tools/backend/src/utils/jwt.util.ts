/**
 * JWT工具类
 * 负责Token的生成、验证和解析
 */

import jwt from 'jsonwebtoken';
import { logger } from './logger.util';

const JWT_SECRET = process.env.JWT_SECRET || 'your_jwt_secret_key_change_in_production';
const JWT_ALGORITHM = (process.env.JWT_ALGORITHM || 'HS256') as jwt.Algorithm;
const JWT_EXPIRATION = parseInt(process.env.JWT_EXPIRATION || '1800'); // 30分钟
const REFRESH_TOKEN_EXPIRATION = parseInt(process.env.REFRESH_TOKEN_EXPIRATION || '604800'); // 7天

export interface JwtPayload {
  gm_id: number;
  username: string;
  gm_level: number;
  permissions: string[];
  device_id?: string;
  type: 'access' | 'refresh';
}

/**
 * 生成Access Token
 */
export function generateAccessToken(payload: Omit<JwtPayload, 'type'>): string {
  const tokenPayload: JwtPayload = {
    ...payload,
    type: 'access'
  };

  return jwt.sign(tokenPayload, JWT_SECRET, {
    algorithm: JWT_ALGORITHM,
    expiresIn: JWT_EXPIRATION
  });
}

/**
 * 生成Refresh Token
 */
export function generateRefreshToken(payload: Omit<JwtPayload, 'type' | 'permissions'>): string {
  const tokenPayload: Partial<JwtPayload> = {
    gm_id: payload.gm_id,
    username: payload.username,
    gm_level: payload.gm_level,
    device_id: payload.device_id,
    type: 'refresh'
  };

  return jwt.sign(tokenPayload, JWT_SECRET, {
    algorithm: JWT_ALGORITHM,
    expiresIn: REFRESH_TOKEN_EXPIRATION
  });
}

/**
 * 验证Token
 */
export function verifyToken(token: string): JwtPayload {
  try {
    const decoded = jwt.verify(token, JWT_SECRET, {
      algorithms: [JWT_ALGORITHM]
    }) as JwtPayload;

    return decoded;
  } catch (error) {
    if (error instanceof jwt.TokenExpiredError) {
      throw new Error('Token已过期');
    } else if (error instanceof jwt.JsonWebTokenError) {
      throw new Error('无效的Token');
    } else {
      logger.error('Token验证失败', { error });
      throw new Error('Token验证失败');
    }
  }
}

/**
 * 解析Token（不验证签名）
 */
export function decodeToken(token: string): JwtPayload | null {
  try {
    return jwt.decode(token) as JwtPayload;
  } catch (error) {
    logger.error('Token解析失败', { error });
    return null;
  }
}

/**
 * 检查Token是否过期
 */
export function isTokenExpired(token: string): boolean {
  const decoded = decodeToken(token);
  if (!decoded || !decoded.exp) {
    return true;
  }

  const currentTime = Math.floor(Date.now() / 1000);
  return decoded.exp < currentTime;
}
