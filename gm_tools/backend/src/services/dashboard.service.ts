/**
 * Dashboard服务
 * 提供仪表板数据查询功能
 */

import { gmToolsPool } from '../config/database';
import { logger } from '../utils/logger.util';
import { RowDataPacket } from 'mysql2';

interface DashboardStats {
  onlineGMs: number;
  todayLogins: number;
  todayOperations: number;
  pendingAlerts: number;
}

interface RecentOperation {
  log_id: number;
  gm_user: string;
  operation_type: string;
  operation_desc: string;  // 使用 reason 或 operation_category
  target_type: string;
  target_id: string;
  target_name: string;
  created_at: string;
}

interface Notification {
  id: number;
  type: string;
  title: string;
  message: string;
  created_at: string;
  is_read: boolean;
}

/**
 * 获取仪表板统计数据
 */
export async function getDashboardStats(): Promise<DashboardStats> {
  try {
    const connection = await gmToolsPool.getConnection();

    try {
      // 在线GM数量（最近15分钟内登录的）
      const [onlineGMsResult] = await connection.query<RowDataPacket[]>(
        `SELECT COUNT(DISTINCT gm_id) as count
         FROM gm_login_logs
         WHERE login_result = 'SUCCESS'
         AND created_at >= DATE_SUB(NOW(), INTERVAL 15 MINUTE)`
      );
      const onlineGMs = onlineGMsResult[0]?.count || 0;

      // 今日登录次数
      const [todayLoginsResult] = await connection.query<RowDataPacket[]>(
        `SELECT COUNT(*) as count
         FROM gm_login_logs
         WHERE login_result = 'SUCCESS'
         AND DATE(created_at) = CURDATE()`
      );
      const todayLogins = todayLoginsResult[0]?.count || 0;

      // 今日操作次数
      const [todayOpsResult] = await connection.query<RowDataPacket[]>(
        `SELECT COUNT(*) as count
         FROM gm_operation_logs
         WHERE DATE(created_at) = CURDATE()`
      );
      const todayOperations = todayOpsResult[0]?.count || 0;

      // 待处理警报（这里用失败的登录尝试作为示例）
      const [pendingAlertsResult] = await connection.query<RowDataPacket[]>(
        `SELECT COUNT(*) as count
         FROM gm_login_logs
         WHERE login_result = 'FAILURE'
         AND created_at >= DATE_SUB(NOW(), INTERVAL 1 HOUR)`
      );
      const pendingAlerts = pendingAlertsResult[0]?.count || 0;

      return {
        onlineGMs,
        todayLogins,
        todayOperations,
        pendingAlerts
      };
    } finally {
      connection.release();
    }
  } catch (error) {
    logger.error('获取仪表板统计失败', { error });
    throw error;
  }
}

/**
 * 获取最近操作记录
 */
export async function getRecentOperations(limit: number = 10): Promise<RecentOperation[]> {
  try {
    const connection = await gmToolsPool.getConnection();

    try {
      const [operations] = await connection.query<RowDataPacket[]>(
        `SELECT
          log_id,
          gm_user,
          operation_type,
          COALESCE(reason, operation_type) as operation_desc,
          target_type,
          target_id,
          COALESCE(target_name, target_id) as target_name,
          created_at
         FROM gm_operation_logs
         ORDER BY created_at DESC
         LIMIT ?`,
        [limit]
      );

      return operations as RecentOperation[];
    } finally {
      connection.release();
    }
  } catch (error) {
    logger.error('获取最近操作失败', { error });
    throw error;
  }
}

/**
 * 获取通知列表
 */
export async function getNotifications(
  page: number = 1,
  pageSize: number = 10
): Promise<{ notifications: Notification[]; total: number }> {
  try {
    const connection = await gmToolsPool.getConnection();

    try {
      const offset = (page - 1) * pageSize;

      // 获取总数
      const [countResult] = await connection.query<RowDataPacket[]>(
        `SELECT COUNT(*) as total FROM gm_notifications`
      );
      const total = countResult[0]?.total || 0;

      // 获取通知列表
      const [notifications] = await connection.query<RowDataPacket[]>(
        `SELECT
          notification_id,
          type,
          title,
          content,
          created_at,
          is_read as \`read\`,
          read_at,
          read_by_gm_id
         FROM gm_notifications
         ORDER BY created_at DESC
         LIMIT ? OFFSET ?`,
        [pageSize, offset]
      );

      return {
        notifications: notifications as Notification[],
        total
      };
    } finally {
      connection.release();
    }
  } catch (error) {
    logger.error('获取通知列表失败', { error });
    throw error;
  }
}

/**
 * 标记单个通知为已读
 */
export async function markNotificationAsRead(
  notificationId: string,
  gmId: number
): Promise<void> {
  try {
    const connection = await gmToolsPool.getConnection();

    try {
      await connection.query(
        `UPDATE gm_notifications
         SET is_read = TRUE,
             read_at = NOW(),
             read_by_gm_id = ?
         WHERE notification_id = ?`,
        [gmId, notificationId]
      );
    } finally {
      connection.release();
    }
  } catch (error) {
    logger.error('标记通知已读失败', { error, notificationId });
    throw error;
  }
}

/**
 * 标记所有通知为已读
 */
export async function markAllNotificationsAsRead(gmId: number): Promise<void> {
  try {
    const connection = await gmToolsPool.getConnection();

    try {
      await connection.query(
        `UPDATE gm_notifications
         SET is_read = TRUE,
             read_at = NOW(),
             read_by_gm_id = ?
         WHERE is_read = FALSE`,
        [gmId]
      );
    } finally {
      connection.release();
    }
  } catch (error) {
    logger.error('标记所有通知已读失败', { error });
    throw error;
  }
}
