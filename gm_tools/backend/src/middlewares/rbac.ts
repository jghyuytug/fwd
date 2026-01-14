/**
 * 基于角色的访问控制（RBAC）中间件
 * 验证用户权限
 */

import { Request, Response, NextFunction } from 'express';
import { createApiError } from './errorHandler';
import { logger } from '../utils/logger.util';

/**
 * 权限验证中间件
 * @param requiredPermission 需要的权限
 */
export function requirePermission(requiredPermission: string) {
  return async (req: Request, res: Response, next: NextFunction): Promise<void> => {
    try {
      // 检查用户是否已认证
      if (!req.user) {
        throw createApiError('未认证', 401, 'UNAUTHENTICATED');
      }

      const { gm_id, username, gm_level, permissions } = req.user;

      // 超级管理员（GM等级5）拥有所有权限
      if (gm_level === 5) {
        logger.debug('超级管理员访问', {
          gm_id,
          username,
          permission: requiredPermission,
          path: req.path
        });
        next();
        return;
      }

      // 检查用户是否拥有所需权限
      const hasPermission = permissions && permissions.includes(requiredPermission);

      if (!hasPermission) {
        logger.warn('权限不足', {
          gm_id,
          username,
          required: requiredPermission,
          has: permissions,
          path: req.path
        });

        throw createApiError(
          `权限不足，需要权限：${requiredPermission}`,
          403,
          'INSUFFICIENT_PERMISSIONS',
          { required_permission: requiredPermission }
        );
      }

      logger.debug('权限验证通过', {
        gm_id,
        username,
        permission: requiredPermission,
        path: req.path
      });

      next();
    } catch (error) {
      next(error);
    }
  };
}

/**
 * GM等级验证中间件
 * @param requiredLevel 需要的最低GM等级
 */
export function requireLevel(requiredLevel: number) {
  return async (req: Request, res: Response, next: NextFunction): Promise<void> => {
    try {
      if (!req.user) {
        throw createApiError('未认证', 401, 'UNAUTHENTICATED');
      }

      const { gm_id, username, gm_level } = req.user;

      if (gm_level < requiredLevel) {
        logger.warn('GM等级不足', {
          gm_id,
          username,
          current_level: gm_level,
          required_level: requiredLevel,
          path: req.path
        });

        throw createApiError(
          `GM等级不足，需要等级：${requiredLevel}，当前等级：${gm_level}`,
          403,
          'INSUFFICIENT_LEVEL',
          { required_level: requiredLevel, current_level: gm_level }
        );
      }

      next();
    } catch (error) {
      next(error);
    }
  };
}

/**
 * 多权限验证（需要其中一个）
 * @param requiredPermissions 需要的权限列表
 */
export function requireAnyPermission(requiredPermissions: string[]) {
  return async (req: Request, res: Response, next: NextFunction): Promise<void> => {
    try {
      if (!req.user) {
        throw createApiError('未认证', 401, 'UNAUTHENTICATED');
      }

      const { gm_id, username, gm_level, permissions } = req.user;

      // 超级管理员通过
      if (gm_level === 5) {
        next();
        return;
      }

      // 检查是否拥有任意一个所需权限
      const hasAnyPermission = requiredPermissions.some(
        (perm) => permissions && permissions.includes(perm)
      );

      if (!hasAnyPermission) {
        logger.warn('权限不足（多权限验证）', {
          gm_id,
          username,
          required: requiredPermissions,
          has: permissions,
          path: req.path
        });

        throw createApiError(
          `权限不足，需要以下任意权限：${requiredPermissions.join(', ')}`,
          403,
          'INSUFFICIENT_PERMISSIONS',
          { required_permissions: requiredPermissions }
        );
      }

      next();
    } catch (error) {
      next(error);
    }
  };
}
