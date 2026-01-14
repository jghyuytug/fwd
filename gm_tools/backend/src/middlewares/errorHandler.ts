/**
 * 全局错误处理中间件
 */

import { Request, Response, NextFunction } from 'express';
import { logger } from '../utils/logger.util';

export interface ApiError extends Error {
  statusCode?: number;
  code?: string;
  details?: any;
}

/**
 * 全局错误处理
 */
export function errorHandler(
  err: ApiError,
  req: Request,
  res: Response,
  next: NextFunction
): void {
  // 记录错误日志
  logger.error('API错误', {
    error: err.message,
    stack: err.stack,
    path: req.path,
    method: req.method,
    ip: req.ip,
    user: (req as any).user?.username
  });

  // 确定HTTP状态码
  const statusCode = err.statusCode || 500;

  // 构建错误响应
  const response: any = {
    code: statusCode,
    message: err.message || '服务器内部错误',
    timestamp: new Date().toISOString(),
    path: req.path
  };

  // 开发环境返回详细错误信息
  if (process.env.NODE_ENV === 'development') {
    response.stack = err.stack;
    response.details = err.details;
  }

  res.status(statusCode).json(response);
}

/**
 * 404处理
 */
export function notFoundHandler(req: Request, res: Response): void {
  res.status(404).json({
    code: 404,
    message: `无法找到请求的资源: ${req.method} ${req.path}`,
    timestamp: new Date().toISOString()
  });
}

/**
 * 创建API错误
 */
export function createApiError(
  message: string,
  statusCode: number = 500,
  code?: string,
  details?: any
): ApiError {
  const error = new Error(message) as ApiError;
  error.statusCode = statusCode;
  error.code = code;
  error.details = details;
  return error;
}
