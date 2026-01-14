/**
 * GM操作数据模型
 * 记录所有GM操作日志到gm_tools数据库
 */

import { gmToolsPool } from '../config/database';
import { RowDataPacket, ResultSetHeader } from 'mysql2';

/**
 * 操作类型枚举
 */
export enum OperationType {
  ITEM_GRANT = 'ITEM_GRANT',           // 物品赠送
  MAIL_SEND = 'MAIL_SEND',             // 邮件发送
  ACCOUNT_BAN = 'ACCOUNT_BAN',         // 账号封禁
  ACCOUNT_UNBAN = 'ACCOUNT_UNBAN',     // 账号解封
  LEVEL_MODIFY = 'LEVEL_MODIFY',       // 等级修改
  MONEY_MODIFY = 'MONEY_MODIFY',       // 金币修改
  CHARACTER_DELETE = 'CHARACTER_DELETE', // 角色删除
  CHARACTER_RESTORE = 'CHARACTER_RESTORE' // 角色恢复
}

/**
 * 操作分类
 */
export enum OperationCategory {
  ITEM = 'ITEM',       // 物品相关
  ACCOUNT = 'ACCOUNT', // 账号相关
  CHARACTER = 'CHARACTER', // 角色相关
  MAIL = 'MAIL'        // 邮件相关
}

/**
 * 目标类型
 */
export enum TargetType {
  ACCOUNT = 'ACCOUNT',     // 账号
  CHARACTER = 'CHARACTER'  // 角色
}

/**
 * 操作结果
 */
export enum OperationResult {
  SUCCESS = 'SUCCESS',   // 成功
  FAILURE = 'FAILURE',   // 失败
  PENDING = 'PENDING'    // 待处理（异步操作）
}

/**
 * GM操作日志
 */
export interface GmOperationLog {
  log_id: number;
  gm_user: string;
  gm_id: number;
  gm_level: number;
  operation_type: OperationType;
  operation_category: OperationCategory;
  target_type: TargetType;
  target_id: string;
  target_name: string | null;
  operation_detail: any; // JSON
  before_state: any | null; // JSON
  after_state: any | null; // JSON
  result: OperationResult;
  error_message: string | null;
  ip_address: string;
  user_agent: string | null;
  created_at: Date;
}

/**
 * 创建操作日志参数
 */
export interface CreateOperationLogParams {
  gm_user: string;
  gm_id: number;
  gm_level: number;
  operation_type: OperationType;
  operation_category: OperationCategory;
  target_type: TargetType;
  target_id: string;
  target_name?: string;
  operation_detail: any;
  before_state?: any;
  after_state?: any;
  result: OperationResult;
  error_message?: string;
  ip_address: string;
  user_agent?: string;
}

/**
 * 操作日志查询参数
 */
export interface OperationLogQueryParams {
  gm_user?: string;
  gm_id?: number;
  operation_type?: OperationType;
  operation_category?: OperationCategory;
  target_type?: TargetType;
  target_id?: string;
  result?: OperationResult;
  start_date?: Date;
  end_date?: Date;
  page?: number;
  page_size?: number;
}

/**
 * 操作日志查询结果
 */
export interface OperationLogQueryResult {
  logs: GmOperationLog[];
  total: number;
  page: number;
  page_size: number;
}

/**
 * 创建GM操作日志
 */
export async function createOperationLog(
  params: CreateOperationLogParams
): Promise<number> {
  const sql = `
    INSERT INTO gm_operation_logs (
      gm_user, gm_id, gm_level,
      operation_type, operation_category,
      target_type, target_id, target_name,
      operation_detail, before_state, after_state,
      result, error_message,
      ip_address, user_agent
    ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
  `;

  const [result] = await gmToolsPool.query<ResultSetHeader>(sql, [
    params.gm_user,
    params.gm_id,
    params.gm_level,
    params.operation_type,
    params.operation_category,
    params.target_type,
    params.target_id,
    params.target_name || null,
    JSON.stringify(params.operation_detail),
    params.before_state ? JSON.stringify(params.before_state) : null,
    params.after_state ? JSON.stringify(params.after_state) : null,
    params.result,
    params.error_message || null,
    params.ip_address,
    params.user_agent || null
  ]);

  return result.insertId;
}

/**
 * 查询操作日志
 */
export async function queryOperationLogs(
  params: OperationLogQueryParams
): Promise<OperationLogQueryResult> {
  const {
    gm_user,
    gm_id,
    operation_type,
    operation_category,
    target_type,
    target_id,
    result,
    start_date,
    end_date,
    page = 1,
    page_size = 50
  } = params;

  // 构建查询条件
  const conditions: string[] = [];
  const values: any[] = [];

  if (gm_user) {
    conditions.push('gm_user = ?');
    values.push(gm_user);
  }

  if (gm_id) {
    conditions.push('gm_id = ?');
    values.push(gm_id);
  }

  if (operation_type) {
    conditions.push('operation_type = ?');
    values.push(operation_type);
  }

  if (operation_category) {
    conditions.push('operation_category = ?');
    values.push(operation_category);
  }

  if (target_type) {
    conditions.push('target_type = ?');
    values.push(target_type);
  }

  if (target_id) {
    conditions.push('target_id = ?');
    values.push(target_id);
  }

  if (result) {
    conditions.push('result = ?');
    values.push(result);
  }

  if (start_date) {
    conditions.push('created_at >= ?');
    values.push(start_date);
  }

  if (end_date) {
    conditions.push('created_at <= ?');
    values.push(end_date);
  }

  const whereClause = conditions.length > 0 ? `WHERE ${conditions.join(' AND ')}` : '';

  // 查询总数
  const countSql = `SELECT COUNT(*) as total FROM gm_operation_logs ${whereClause}`;
  const [countRows] = await gmToolsPool.query<RowDataPacket[]>(countSql, values);
  const total = countRows[0].total;

  // 查询数据
  const offset = (page - 1) * page_size;
  const dataSql = `
    SELECT *
    FROM gm_operation_logs
    ${whereClause}
    ORDER BY created_at DESC
    LIMIT ? OFFSET ?
  `;
  const [rows] = await gmToolsPool.query<RowDataPacket[]>(dataSql, [...values, page_size, offset]);

  // 解析JSON字段
  const logs: GmOperationLog[] = rows.map(row => ({
    ...row,
    operation_detail: row.operation_detail ? JSON.parse(row.operation_detail) : null,
    before_state: row.before_state ? JSON.parse(row.before_state) : null,
    after_state: row.after_state ? JSON.parse(row.after_state) : null
  }));

  return {
    logs,
    total,
    page,
    page_size
  };
}

/**
 * 根据ID查询操作日志
 */
export async function findOperationLogById(logId: number): Promise<GmOperationLog | null> {
  const sql = 'SELECT * FROM gm_operation_logs WHERE log_id = ?';
  const [rows] = await gmToolsPool.query<RowDataPacket[]>(sql, [logId]);

  if (rows.length === 0) {
    return null;
  }

  const row = rows[0];
  return {
    ...row,
    operation_detail: row.operation_detail ? JSON.parse(row.operation_detail) : null,
    before_state: row.before_state ? JSON.parse(row.before_state) : null,
    after_state: row.after_state ? JSON.parse(row.after_state) : null
  };
}

/**
 * 获取操作统计
 */
export async function getOperationStats(
  gmId?: number,
  startDate?: Date,
  endDate?: Date
): Promise<{
  total_operations: number;
  success_count: number;
  failure_count: number;
  by_type: Record<string, number>;
  by_category: Record<string, number>;
}> {
  const conditions: string[] = [];
  const values: any[] = [];

  if (gmId) {
    conditions.push('gm_id = ?');
    values.push(gmId);
  }

  if (startDate) {
    conditions.push('created_at >= ?');
    values.push(startDate);
  }

  if (endDate) {
    conditions.push('created_at <= ?');
    values.push(endDate);
  }

  const whereClause = conditions.length > 0 ? `WHERE ${conditions.join(' AND ')}` : '';

  const sql = `
    SELECT
      COUNT(*) as total_operations,
      SUM(CASE WHEN result = 'SUCCESS' THEN 1 ELSE 0 END) as success_count,
      SUM(CASE WHEN result = 'FAILURE' THEN 1 ELSE 0 END) as failure_count,
      operation_type,
      operation_category
    FROM gm_operation_logs
    ${whereClause}
    GROUP BY operation_type, operation_category
  `;

  const [rows] = await gmToolsPool.query<RowDataPacket[]>(sql, values);

  const byType: Record<string, number> = {};
  const byCategory: Record<string, number> = {};
  let total = 0;
  let successCount = 0;
  let failureCount = 0;

  rows.forEach(row => {
    total += row.total_operations;
    successCount += row.success_count;
    failureCount += row.failure_count;

    byType[row.operation_type] = (byType[row.operation_type] || 0) + row.total_operations;
    byCategory[row.operation_category] = (byCategory[row.operation_category] || 0) + row.total_operations;
  });

  return {
    total_operations: total,
    success_count: successCount,
    failure_count: failureCount,
    by_type: byType,
    by_category: byCategory
  };
}
