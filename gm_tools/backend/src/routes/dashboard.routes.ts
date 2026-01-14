/**
 * Dashboard路由
 * 处理仪表板相关的API请求
 */

import express from 'express';
import * as dashboardController from '../controllers/dashboard.controller';
import { authenticate } from '../middlewares/auth';

const router = express.Router();

// 所有dashboard路由都需要认证
router.use(authenticate);

/**
 * @route GET /api/v1/dashboard/stats
 * @desc 获取仪表板统计数据
 * @access Private
 */
router.get('/stats', dashboardController.getStats);

/**
 * @route GET /api/v1/dashboard/recent-operations
 * @desc 获取最近操作记录
 * @access Private
 * @query limit - 返回记录数量（默认10，最大100）
 */
router.get('/recent-operations', dashboardController.getRecentOperations);

/**
 * @route GET /api/v1/dashboard/notifications
 * @desc 获取通知列表
 * @access Private
 * @query page - 页码（默认1）
 * @query page_size - 每页大小（默认10，最大100）
 */
router.get('/notifications', dashboardController.getNotifications);

/**
 * @route PUT /api/v1/dashboard/notifications/read-all
 * @desc 标记所有通知为已读
 * @access Private
 */
router.put('/notifications/read-all', dashboardController.markAllNotificationsAsRead);

/**
 * @route PUT /api/v1/dashboard/notifications/:notificationId/read
 * @desc 标记单个通知为已读
 * @access Private
 * @param notificationId - 通知ID
 */
router.put('/notifications/:notificationId/read', dashboardController.markNotificationAsRead);

export default router;
