/**
 * 审计日志路由
 * 提供操作日志、登录日志的查询、统计和导出功能
 */

import express from 'express';
import * as auditController from '../controllers/audit.controller';
import { requirePermission } from '../middlewares/rbac';
import { auditLog } from '../middlewares/audit';

const router = express.Router();

/**
 * 查询操作日志
 * GET /api/v1/audit/operations
 * 权限要求: audit:read
 *
 * 查询参数:
 * - gm_user: GM用户名
 * - gm_id: GM ID
 * - operation_type: 操作类型（ITEM_GRANT, ACCOUNT_BAN等）
 * - operation_category: 操作分类（ITEM, ACCOUNT, CHARACTER, MAIL）
 * - target_type: 目标类型（ACCOUNT, CHARACTER）
 * - target_id: 目标ID
 * - result: 操作结果（SUCCESS, FAILURE, PENDING）
 * - start_date: 开始日期（ISO格式，如 2025-01-01T00:00:00Z）
 * - end_date: 结束日期（ISO格式）
 * - page: 页码（默认1）
 * - page_size: 每页数量（默认50，最大100）
 *
 * 响应:
 * {
 *   "code": 200,
 *   "message": "查询成功",
 *   "data": {
 *     "logs": [...],
 *     "total": 123,
 *     "page": 1,
 *     "page_size": 50
 *   }
 * }
 */
router.get(
  '/operations',
  requirePermission('audit:read'),
  auditLog('查询操作日志'),
  auditController.queryOperationLogs
);

/**
 * 查询登录日志
 * GET /api/v1/audit/logins
 * 权限要求: audit:read
 *
 * 查询参数:
 * - gm_user: GM用户名
 * - gm_id: GM ID
 * - login_status: 登录状态（SUCCESS, FAILURE）
 * - ip_address: IP地址
 * - start_date: 开始日期（ISO格式）
 * - end_date: 结束日期（ISO格式）
 * - page: 页码（默认1）
 * - page_size: 每页数量（默认50，最大100）
 *
 * 响应:
 * {
 *   "code": 200,
 *   "message": "查询成功",
 *   "data": {
 *     "logs": [
 *       {
 *         "log_id": 1,
 *         "gm_user": "admin",
 *         "gm_id": 1,
 *         "login_status": "SUCCESS",
 *         "ip_address": "192.168.1.100",
 *         "user_agent": "Mozilla/5.0...",
 *         "failure_reason": null,
 *         "login_time": "2025-01-09T10:00:00.000Z"
 *       }
 *     ],
 *     "total": 45,
 *     "page": 1,
 *     "page_size": 50
 *   }
 * }
 */
router.get(
  '/logins',
  requirePermission('audit:read'),
  auditLog('查询登录日志'),
  auditController.queryLoginLogs
);

/**
 * 获取操作统计
 * GET /api/v1/audit/stats/operations
 * 权限要求: audit:read
 *
 * 查询参数:
 * - gm_id: GM ID（可选，不指定则统计所有GM）
 * - start_date: 开始日期（ISO格式）
 * - end_date: 结束日期（ISO格式）
 *
 * 响应:
 * {
 *   "code": 200,
 *   "message": "统计成功",
 *   "data": {
 *     "total_operations": 500,
 *     "success_count": 480,
 *     "failure_count": 20,
 *     "success_rate": "96.00",
 *     "by_type": {
 *       "ITEM_GRANT": 200,
 *       "ACCOUNT_BAN": 50,
 *       "LEVEL_MODIFY": 100
 *     },
 *     "by_category": {
 *       "ITEM": 250,
 *       "ACCOUNT": 100,
 *       "CHARACTER": 150
 *     },
 *     "top_gms": [
 *       {
 *         "gm_user": "admin",
 *         "gm_id": 1,
 *         "operation_count": 300
 *       }
 *     ]
 *   }
 * }
 */
router.get(
  '/stats/operations',
  requirePermission('audit:read'),
  auditLog('查询操作统计'),
  auditController.getOperationStats
);

/**
 * 获取登录统计
 * GET /api/v1/audit/stats/logins
 * 权限要求: audit:read
 *
 * 查询参数:
 * - gm_id: GM ID（可选）
 * - start_date: 开始日期（ISO格式）
 * - end_date: 结束日期（ISO格式）
 *
 * 响应:
 * {
 *   "code": 200,
 *   "message": "统计成功",
 *   "data": {
 *     "total_logins": 1000,
 *     "success_count": 980,
 *     "failure_count": 20,
 *     "success_rate": "98.00",
 *     "unique_ips": 25,
 *     "top_ips": [
 *       {
 *         "ip_address": "192.168.1.100",
 *         "login_count": 300
 *       }
 *     ]
 *   }
 * }
 */
router.get(
  '/stats/logins',
  requirePermission('audit:read'),
  auditLog('查询登录统计'),
  auditController.getLoginStats
);

/**
 * 导出操作日志
 * GET /api/v1/audit/export/operations
 * 权限要求: audit:export
 *
 * 查询参数:
 * - gm_user: GM用户名
 * - gm_id: GM ID
 * - operation_type: 操作类型
 * - operation_category: 操作分类
 * - target_type: 目标类型
 * - target_id: 目标ID
 * - result: 操作结果
 * - start_date: 开始日期（ISO格式）
 * - end_date: 结束日期（ISO格式）
 * - format: 导出格式（json 或 csv，默认 json）
 *
 * 响应:
 * - format=json: JSON格式的日志数组
 * - format=csv: CSV文件下载
 *
 * 注意: 最多导出10000条记录
 */
router.get(
  '/export/operations',
  requirePermission('audit:export'),
  auditLog('导出操作日志'),
  auditController.exportOperationLogs
);

/**
 * 导出登录日志
 * GET /api/v1/audit/export/logins
 * 权限要求: audit:export
 *
 * 查询参数:
 * - gm_user: GM用户名
 * - gm_id: GM ID
 * - login_status: 登录状态
 * - ip_address: IP地址
 * - start_date: 开始日期（ISO格式）
 * - end_date: 结束日期（ISO格式）
 * - format: 导出格式（json 或 csv，默认 json）
 *
 * 响应:
 * - format=json: JSON格式的日志数组
 * - format=csv: CSV文件下载
 *
 * 注意: 最多导出10000条记录
 */
router.get(
  '/export/logins',
  requirePermission('audit:export'),
  auditLog('导出登录日志'),
  auditController.exportLoginLogs
);

export default router;
