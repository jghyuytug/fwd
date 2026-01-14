/**
 * 审计日志中间件
 * 记录所有API请求和GM操作
 */

import { Request, Response, NextFunction } from 'express';
import { gmToolsPool } from '../config/database';
import { logger } from '../utils/logger.util';

/**
 * 记录GM操作日志
 * @param operationType 操作类型
 * @param operationCategory 操作类别
 */
export function auditOperation(operationType: string, operationCategory: string) {
  return async (req: Request, res: Response, next: NextFunction): Promise<void> => {
    // 保存原始的 res.json 方法
    const originalJson = res.json.bind(res);

    // 重写 res.json 方法以拦截响应
    res.json = function(body: any): Response {
      // 异步记录操作日志（不阻塞响应）
      setImmediate(async () => {
        try {
          if (!req.user) {
            return;
          }

          const { gm_id, username, gm_level } = req.user;

          // 提取操作目标信息
          const targetType = req.params.char_no ? 'CHARACTER' :
                           req.params.m_id ? 'ACCOUNT' :
                           'SYSTEM';
          const targetId = req.params.char_no || req.params.m_id || null;
          const targetName = body.data?.char_name || body.data?.account_name || null;

          // 提取操作详情
          const operationDetail = JSON.stringify({
            method: req.method,
            path: req.path,
            query: req.query,
            body: req.body,
            response: body
          });

          // 判断操作结果
          const result = res.statusCode >= 200 && res.statusCode < 300 ? 'SUCCESS' : 'FAILURE';
          const errorMessage = result === 'FAILURE' ? body.message : null;

          // 插入操作日志
          await gmToolsPool.query(`
            INSERT INTO gm_operation_logs (
              gm_user, gm_id, gm_level,
              operation_type, operation_category,
              target_type, target_id, target_name,
              operation_detail,
              result, error_message,
              ip_address, user_agent
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
          `, [
            username,
            gm_id,
            gm_level,
            operationType,
            operationCategory,
            targetType,
            targetId,
            targetName,
            operationDetail,
            result,
            errorMessage,
            req.ip,
            req.headers['user-agent']
          ]);

          logger.info('GM操作日志已记录', {
            gm_id,
            username,
            operation_type: operationType,
            result,
            target_type: targetType,
            target_id: targetId
          });

        } catch (error) {
          logger.error('记录操作日志失败', { error });
        }
      });

      // 调用原始的 json 方法
      return originalJson(body);
    };

    next();
  };
}

/**
 * HTTP请求日志中间件（使用Morgan）
 */
import morgan from 'morgan';

export const requestLogger = morgan('combined', {
  stream: {
    write: (message: string) => {
      logger.http(message.trim());
    }
  }
});

/**
 * auditLog 是 auditOperation 的别名，用于简化调用
 */
export const auditLog = auditOperation;
