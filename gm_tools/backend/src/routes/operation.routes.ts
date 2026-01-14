/**
 * GM操作路由
 * 处理物品赠送、邮件发送、账号封禁、数据修改等GM操作
 */

import express from 'express';
import * as operationController from '../controllers/operation.controller';
import { requirePermission } from '../middlewares/rbac';
import { auditLog } from '../middlewares/audit';

const router = express.Router();

/**
 * 物品赠送
 * POST /api/v1/operations/grant-item
 * 权限要求: item:grant
 *
 * 请求体:
 * {
 *   "char_no": 123456,          // 角色编号
 *   "item_id": 100001,          // 物品ID
 *   "amount": 10,               // 数量
 *   "reason": "活动补偿",       // 赠送原因
 *   "expire_days": 7            // 过期天数（可选，0表示永久）
 * }
 *
 * 响应:
 * {
 *   "code": 200,
 *   "message": "物品赠送成功",
 *   "data": {
 *     "operation_id": 12345,
 *     "char_info": { "char_no": 123456, "char_name": "玩家A" },
 *     "item_info": { "item_id": 100001, "item_name": "复活币", "amount": 10 },
 *     "executed_at": "2025-01-09T10:30:00.000Z"
 *   }
 * }
 */
router.post(
  '/grant-item',
  requirePermission('item:grant'),
  auditLog('赠送物品'),
  operationController.grantItem
);

/**
 * 发送邮件
 * POST /api/v1/operations/send-mail
 * 权限要求: item:grant
 *
 * 请求体:
 * {
 *   "char_no": 123456,          // 角色编号
 *   "title": "系统邮件",        // 邮件标题
 *   "content": "这是邮件内容",  // 邮件内容
 *   "item_id": 100001,          // 附件物品ID（可选）
 *   "item_amount": 5,           // 附件物品数量（可选）
 *   "gold": 1000000,            // 附件金币（可选）
 *   "expire_days": 30           // 邮件过期天数（可选，默认30天）
 * }
 *
 * 响应:
 * {
 *   "code": 200,
 *   "message": "邮件发送成功",
 *   "data": {
 *     "operation_id": 12346,
 *     "char_info": { "char_no": 123456, "char_name": "玩家A" },
 *     "mail_info": { "mail_id": 789, "title": "系统邮件" },
 *     "executed_at": "2025-01-09T10:35:00.000Z"
 *   }
 * }
 */
router.post(
  '/send-mail',
  requirePermission('item:grant'),
  auditLog('发送邮件'),
  operationController.sendMail
);

/**
 * 封禁账号
 * POST /api/v1/operations/ban-account
 * 权限要求: account:ban
 *
 * 请求体:
 * {
 *   "m_id": "test_user",        // 账号ID
 *   "ban_type": 1,              // 封禁类型（1-封禁，2-禁言，3-警告）
 *   "ban_duration": 86400,      // 封禁时长（秒，0表示永久）
 *   "reason": "使用外挂",       // 封禁原因
 *   "evidence": "截图链接"      // 证据（可选）
 * }
 *
 * 响应:
 * {
 *   "code": 200,
 *   "message": "账号封禁成功",
 *   "data": {
 *     "operation_id": 12347,
 *     "account_info": { "m_id": "test_user" },
 *     "ban_info": {
 *       "ban_type": 1,
 *       "ban_until": "2025-01-10T10:40:00.000Z"
 *     },
 *     "executed_at": "2025-01-09T10:40:00.000Z"
 *   }
 * }
 */
router.post(
  '/ban-account',
  requirePermission('account:ban'),
  auditLog('封禁账号'),
  operationController.banAccount
);

/**
 * 解封账号
 * POST /api/v1/operations/unban-account
 * 权限要求: account:unban
 *
 * 请求体:
 * {
 *   "m_id": "test_user",        // 账号ID
 *   "reason": "误封解除"        // 解封原因
 * }
 *
 * 响应:
 * {
 *   "code": 200,
 *   "message": "账号解封成功",
 *   "data": {
 *     "operation_id": 12348,
 *     "account_info": { "m_id": "test_user" },
 *     "executed_at": "2025-01-09T10:45:00.000Z"
 *   }
 * }
 */
router.post(
  '/unban-account',
  requirePermission('account:unban'),
  auditLog('解封账号'),
  operationController.unbanAccount
);

/**
 * 修改等级
 * POST /api/v1/operations/modify-level
 * 权限要求: player:modify
 *
 * 请求体:
 * {
 *   "char_no": 123456,          // 角色编号
 *   "new_level": 85,            // 新等级（1-100）
 *   "reason": "活动奖励"        // 修改原因
 * }
 *
 * 响应:
 * {
 *   "code": 200,
 *   "message": "等级修改成功",
 *   "data": {
 *     "operation_id": 12349,
 *     "char_info": { "char_no": 123456, "char_name": "玩家A" },
 *     "level_info": {
 *       "old_level": 80,
 *       "new_level": 85
 *     },
 *     "executed_at": "2025-01-09T10:50:00.000Z"
 *   }
 * }
 */
router.post(
  '/modify-level',
  requirePermission('player:modify'),
  auditLog('修改等级'),
  operationController.modifyLevel
);

/**
 * 修改金币
 * POST /api/v1/operations/modify-money
 * 权限要求: player:modify
 *
 * 请求体:
 * {
 *   "char_no": 123456,          // 角色编号
 *   "gold": 1000000,            // 金币数量（可选）
 *   "silver": 500000,           // 银币数量（可选）
 *   "reason": "补偿发放"        // 修改原因
 * }
 *
 * 注意: gold 和 silver 至少提供一个
 *
 * 响应:
 * {
 *   "code": 200,
 *   "message": "金币修改成功",
 *   "data": {
 *     "operation_id": 12350,
 *     "char_info": { "char_no": 123456, "char_name": "玩家A" },
 *     "money_info": {
 *       "old_money": { "gold": "500000", "silver": "200000" },
 *       "new_money": { "gold": "1000000", "silver": "500000" }
 *     },
 *     "executed_at": "2025-01-09T10:55:00.000Z"
 *   }
 * }
 */
router.post(
  '/modify-money',
  requirePermission('player:modify'),
  auditLog('修改金币'),
  operationController.modifyMoney
);

export default router;
