/**
 * 认证中间件
 * 验证JWT Token并解析用户信息
 */

import { Request, Response, NextFunction } from 'express';
import { verifyToken, JwtPayload } from '../utils/jwt.util';
import { createApiError } from './errorHandler';
import { logger } from '../utils/logger.util';

// 扩展Express Request类型
declare global {
  namespace Express {
    interface Request {
      user?: JwtPayload;
    }
  }
}

/**
 * JWT认证中间件
 */
export async function authenticate(
  req: Request,
  res: Response,
  next: NextFunction
): Promise<void> {
  try {
    // 从Header中获取Token
    const authHeader = req.headers.authorization;

    if (!authHeader) {
      throw createApiError('缺少认证Token', 401, 'MISSING_TOKEN');
    }

    // 解析Bearer Token
    const parts = authHeader.split(' ');
    if (parts.length !== 2 || parts[0] !== 'Bearer') {
      throw createApiError('Token格式错误', 401, 'INVALID_TOKEN_FORMAT');
    }

    const token = parts[1];

    // 验证Token
    const payload = verifyToken(token);

    // 检查Token类型
    if (payload.type !== 'access') {
      throw createApiError('无效的Token类型', 401, 'INVALID_TOKEN_TYPE');
    }

    // 将用户信息附加到请求对象
    req.user = payload;

    // TODO: 检查Token是否在黑名单中（Redis）

    logger.debug('用户认证成功', {
      gm_id: payload.gm_id,
      username: payload.username,
      path: req.path
    });

    next();
  } catch (error) {
    next(error);
  }
}

/**
 * 可选认证中间件（Token存在时验证，不存在则跳过）
 */
export async function optionalAuthenticate(
  req: Request,
  res: Response,
  next: NextFunction
): Promise<void> {
  try {
    const authHeader = req.headers.authorization;

    if (authHeader) {
      const parts = authHeader.split(' ');
      if (parts.length === 2 && parts[0] === 'Bearer') {
        const token = parts[1];
        const payload = verifyToken(token);

        if (payload.type === 'access') {
          req.user = payload;
        }
      }
    }

    next();
  } catch (error) {
    // 可选认证失败时不抛出错误，继续处理
    logger.warn('可选认证失败', { error: (error as Error).message });
    next();
  }
}
