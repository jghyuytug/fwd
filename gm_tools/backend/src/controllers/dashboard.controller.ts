/**
 * Dashboard控制器
 * 处理仪表板相关的HTTP请求
 */

import { Request, Response } from 'express';
import * as dashboardService from '../services/dashboard.service';
import { logger } from '../utils/logger.util';

/**
 * 获取仪表板统计数据
 * GET /api/v1/dashboard/stats
 */
export async function getStats(req: Request, res: Response): Promise<void> {
  try {
    const stats = await dashboardService.getDashboardStats();

    res.json({
      code: 0,
      message: 'success',
      data: stats
    });
  } catch (error) {
    logger.error('获取仪表板统计失败', { error });
    res.status(500).json({
      code: 500,
      message: '获取统计数据失败',
      data: null
    });
  }
}

/**
 * 获取最近操作记录
 * GET /api/v1/dashboard/recent-operations
 */
export async function getRecentOperations(req: Request, res: Response): Promise<void> {
  try {
    const limit = parseInt(req.query.limit as string) || 10;

    if (limit < 1 || limit > 100) {
      res.status(400).json({
        code: 400,
        message: 'limit参数必须在1-100之间',
        data: null
      });
      return;
    }

    const operations = await dashboardService.getRecentOperations(limit);

    res.json({
      code: 0,
      message: 'success',
      data: operations
    });
  } catch (error) {
    logger.error('获取最近操作失败', { error });
    res.status(500).json({
      code: 500,
      message: '获取操作记录失败',
      data: null
    });
  }
}

/**
 * 获取通知列表
 * GET /api/v1/dashboard/notifications
 */
export async function getNotifications(req: Request, res: Response): Promise<void> {
  try {
    const page = parseInt(req.query.page as string) || 1;
    const pageSize = parseInt(req.query.page_size as string) || 10;

    if (page < 1) {
      res.status(400).json({
        code: 400,
        message: 'page参数必须大于0',
        data: null
      });
      return;
    }

    if (pageSize < 1 || pageSize > 100) {
      res.status(400).json({
        code: 400,
        message: 'page_size参数必须在1-100之间',
        data: null
      });
      return;
    }

    const result = await dashboardService.getNotifications(page, pageSize);

    res.json({
      code: 0,
      message: 'success',
      data: result.notifications
    });
  } catch (error) {
    logger.error('获取通知列表失败', { error });
    res.status(500).json({
      code: 500,
      message: '获取通知失败',
      data: null
    });
  }
}

/**
 * 标记单个通知为已读
 * PUT /api/v1/dashboard/notifications/:notificationId/read
 */
export async function markNotificationAsRead(req: Request, res: Response): Promise<void> {
  try {
    const { notificationId } = req.params;
    const gmId = (req as any).user?.gm_id;

    if (!notificationId) {
      res.status(400).json({
        code: 400,
        message: '缺少通知ID',
        data: null
      });
      return;
    }

    await dashboardService.markNotificationAsRead(notificationId, gmId);

    res.json({
      code: 0,
      message: '标记成功',
      data: null
    });
  } catch (error) {
    logger.error('标记通知已读失败', { error });
    res.status(500).json({
      code: 500,
      message: '操作失败',
      data: null
    });
  }
}

/**
 * 标记所有通知为已读
 * PUT /api/v1/dashboard/notifications/read-all
 */
export async function markAllNotificationsAsRead(req: Request, res: Response): Promise<void> {
  try {
    const gmId = (req as any).user?.gm_id;

    await dashboardService.markAllNotificationsAsRead(gmId);

    res.json({
      code: 0,
      message: '全部标记成功',
      data: null
    });
  } catch (error) {
    logger.error('标记所有通知已读失败', { error });
    res.status(500).json({
      code: 500,
      message: '操作失败',
      data: null
    });
  }
}
