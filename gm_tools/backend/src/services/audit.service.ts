/**
 * 审计日志服务
 * 提供操作日志、登录日志的查询和统计功能
 */

import * as operationModel from '../models/operation.model';
import { gmToolsPool } from '../config/database';
import { RowDataPacket } from 'mysql2';
import { createApiError } from '../middlewares/errorHandler';
import { logger } from '../utils/logger.util';

/**
 * 操作日志查询参数
 */
export interface OperationLogQuery {
  gm_user?: string;
  gm_id?: number;
  operation_type?: string;
  operation_category?: string;
  target_type?: string;
  target_id?: string;
  result?: string;
  start_date?: string; // ISO格式日期
  end_date?: string;
  page?: number;
  page_size?: number;
}

/**
 * 登录日志查询参数
 */
export interface LoginLogQuery {
  gm_user?: string;
  gm_id?: number;
  login_status?: 'SUCCESS' | 'FAILURE';
  start_date?: string;
  end_date?: string;
  ip_address?: string;
  page?: number;
  page_size?: number;
}

/**
 * 登录日志记录
 */
export interface LoginLog {
  log_id: number;
  gm_user: string;
  gm_id: number;
  login_status: 'SUCCESS' | 'FAILURE';
  ip_address: string;
  user_agent: string | null;
  failure_reason: string | null;
  login_time: Date;
}

/**
 * 统计查询参数
 */
export interface StatsQuery {
  gm_id?: number;
  start_date?: string;
  end_date?: string;
}

/**
 * 导出格式
 */
export type ExportFormat = 'json' | 'csv';

/**
 * 查询操作日志
 */
export async function queryOperationLogs(
  query: OperationLogQuery
): Promise<{
  logs: operationModel.GmOperationLog[];
  total: number;
  page: number;
  page_size: number;
}> {
  logger.info('查询操作日志', { query });

  // 参数验证
  const page = query.page && query.page >= 1 ? query.page : 1;
  const pageSize = query.page_size && query.page_size >= 1 && query.page_size <= 100
    ? query.page_size
    : 50;

  // 转换日期参数
  const params: operationModel.OperationLogQueryParams = {
    gm_user: query.gm_user,
    gm_id: query.gm_id,
    operation_type: query.operation_type as operationModel.OperationType | undefined,
    operation_category: query.operation_category as operationModel.OperationCategory | undefined,
    target_type: query.target_type as operationModel.TargetType | undefined,
    target_id: query.target_id,
    result: query.result as operationModel.OperationResult | undefined,
    start_date: query.start_date ? new Date(query.start_date) : undefined,
    end_date: query.end_date ? new Date(query.end_date) : undefined,
    page,
    page_size: pageSize
  };

  // 验证日期格式
  if (query.start_date && isNaN(params.start_date!.getTime())) {
    throw createApiError('无效的开始日期格式', 400, 'INVALID_START_DATE');
  }

  if (query.end_date && isNaN(params.end_date!.getTime())) {
    throw createApiError('无效的结束日期格式', 400, 'INVALID_END_DATE');
  }

  try {
    const result = await operationModel.queryOperationLogs(params);
    logger.info('操作日志查询成功', {
      total: result.total,
      page: result.page,
      returned: result.logs.length
    });
    return result;
  } catch (error: any) {
    logger.error('查询操作日志失败', { error: error.message, query });
    throw createApiError('查询操作日志失败', 500, 'QUERY_FAILED', error.message);
  }
}

/**
 * 查询登录日志
 */
export async function queryLoginLogs(
  query: LoginLogQuery
): Promise<{
  logs: LoginLog[];
  total: number;
  page: number;
  page_size: number;
}> {
  logger.info('查询登录日志', { query });

  // 参数验证
  const page = query.page && query.page >= 1 ? query.page : 1;
  const pageSize = query.page_size && query.page_size >= 1 && query.page_size <= 100
    ? query.page_size
    : 50;

  // 构建查询条件
  const conditions: string[] = [];
  const values: any[] = [];

  if (query.gm_user) {
    conditions.push('gm_user = ?');
    values.push(query.gm_user);
  }

  if (query.gm_id) {
    conditions.push('gm_id = ?');
    values.push(query.gm_id);
  }

  if (query.login_status) {
    conditions.push('login_status = ?');
    values.push(query.login_status);
  }

  if (query.ip_address) {
    conditions.push('ip_address = ?');
    values.push(query.ip_address);
  }

  if (query.start_date) {
    const startDate = new Date(query.start_date);
    if (isNaN(startDate.getTime())) {
      throw createApiError('无效的开始日期格式', 400, 'INVALID_START_DATE');
    }
    conditions.push('login_time >= ?');
    values.push(startDate);
  }

  if (query.end_date) {
    const endDate = new Date(query.end_date);
    if (isNaN(endDate.getTime())) {
      throw createApiError('无效的结束日期格式', 400, 'INVALID_END_DATE');
    }
    conditions.push('login_time <= ?');
    values.push(endDate);
  }

  const whereClause = conditions.length > 0 ? `WHERE ${conditions.join(' AND ')}` : '';

  try {
    // 查询总数
    const countSql = `SELECT COUNT(*) as total FROM gm_login_logs ${whereClause}`;
    const [countRows] = await gmToolsPool.query<RowDataPacket[]>(countSql, values);
    const total = countRows[0].total;

    // 查询数据
    const offset = (page - 1) * pageSize;
    const dataSql = `
      SELECT *
      FROM gm_login_logs
      ${whereClause}
      ORDER BY login_time DESC
      LIMIT ? OFFSET ?
    `;
    const [rows] = await gmToolsPool.query<RowDataPacket[]>(dataSql, [...values, pageSize, offset]);

    const logs: LoginLog[] = rows.map(row => ({
      log_id: row.log_id,
      gm_user: row.gm_user,
      gm_id: row.gm_id,
      login_status: row.login_status,
      ip_address: row.ip_address,
      user_agent: row.user_agent,
      failure_reason: row.failure_reason,
      login_time: row.login_time
    }));

    logger.info('登录日志查询成功', {
      total,
      page,
      returned: logs.length
    });

    return {
      logs,
      total,
      page,
      page_size: pageSize
    };
  } catch (error: any) {
    logger.error('查询登录日志失败', { error: error.message, query });
    throw createApiError('查询登录日志失败', 500, 'QUERY_FAILED', error.message);
  }
}

/**
 * 获取操作统计
 */
export async function getOperationStats(
  query: StatsQuery
): Promise<{
  total_operations: number;
  success_count: number;
  failure_count: number;
  success_rate: string;
  by_type: Record<string, number>;
  by_category: Record<string, number>;
  top_gms?: Array<{ gm_user: string; gm_id: number; operation_count: number }>;
}> {
  logger.info('获取操作统计', { query });

  // 转换日期参数
  const startDate = query.start_date ? new Date(query.start_date) : undefined;
  const endDate = query.end_date ? new Date(query.end_date) : undefined;

  // 验证日期格式
  if (query.start_date && isNaN(startDate!.getTime())) {
    throw createApiError('无效的开始日期格式', 400, 'INVALID_START_DATE');
  }

  if (query.end_date && isNaN(endDate!.getTime())) {
    throw createApiError('无效的结束日期格式', 400, 'INVALID_END_DATE');
  }

  try {
    // 获取基础统计
    const stats = await operationModel.getOperationStats(query.gm_id, startDate, endDate);

    // 计算成功率
    const successRate = stats.total_operations > 0
      ? ((stats.success_count / stats.total_operations) * 100).toFixed(2)
      : '0.00';

    // 如果没有指定GM，获取TOP GM列表
    let topGms: Array<{ gm_user: string; gm_id: number; operation_count: number }> | undefined;

    if (!query.gm_id) {
      const conditions: string[] = [];
      const values: any[] = [];

      if (startDate) {
        conditions.push('created_at >= ?');
        values.push(startDate);
      }

      if (endDate) {
        conditions.push('created_at <= ?');
        values.push(endDate);
      }

      const whereClause = conditions.length > 0 ? `WHERE ${conditions.join(' AND ')}` : '';

      const topGmSql = `
        SELECT
          gm_user,
          gm_id,
          COUNT(*) as operation_count
        FROM gm_operation_logs
        ${whereClause}
        GROUP BY gm_user, gm_id
        ORDER BY operation_count DESC
        LIMIT 10
      `;

      const [topGmRows] = await gmToolsPool.query<RowDataPacket[]>(topGmSql, values);
      topGms = topGmRows.map(row => ({
        gm_user: row.gm_user,
        gm_id: row.gm_id,
        operation_count: row.operation_count
      }));
    }

    const result = {
      total_operations: stats.total_operations,
      success_count: stats.success_count,
      failure_count: stats.failure_count,
      success_rate: successRate,
      by_type: stats.by_type,
      by_category: stats.by_category,
      top_gms: topGms
    };

    logger.info('操作统计获取成功', {
      total: result.total_operations,
      success_rate: result.success_rate
    });

    return result;
  } catch (error: any) {
    logger.error('获取操作统计失败', { error: error.message, query });
    throw createApiError('获取操作统计失败', 500, 'STATS_FAILED', error.message);
  }
}

/**
 * 获取登录统计
 */
export async function getLoginStats(
  query: StatsQuery
): Promise<{
  total_logins: number;
  success_count: number;
  failure_count: number;
  success_rate: string;
  unique_ips: number;
  top_ips?: Array<{ ip_address: string; login_count: number }>;
}> {
  logger.info('获取登录统计', { query });

  // 转换日期参数
  const conditions: string[] = [];
  const values: any[] = [];

  if (query.gm_id) {
    conditions.push('gm_id = ?');
    values.push(query.gm_id);
  }

  if (query.start_date) {
    const startDate = new Date(query.start_date);
    if (isNaN(startDate.getTime())) {
      throw createApiError('无效的开始日期格式', 400, 'INVALID_START_DATE');
    }
    conditions.push('login_time >= ?');
    values.push(startDate);
  }

  if (query.end_date) {
    const endDate = new Date(query.end_date);
    if (isNaN(endDate.getTime())) {
      throw createApiError('无效的结束日期格式', 400, 'INVALID_END_DATE');
    }
    conditions.push('login_time <= ?');
    values.push(endDate);
  }

  const whereClause = conditions.length > 0 ? `WHERE ${conditions.join(' AND ')}` : '';

  try {
    // 获取基础统计
    const statsSql = `
      SELECT
        COUNT(*) as total_logins,
        SUM(CASE WHEN login_status = 'SUCCESS' THEN 1 ELSE 0 END) as success_count,
        SUM(CASE WHEN login_status = 'FAILURE' THEN 1 ELSE 0 END) as failure_count,
        COUNT(DISTINCT ip_address) as unique_ips
      FROM gm_login_logs
      ${whereClause}
    `;

    const [statsRows] = await gmToolsPool.query<RowDataPacket[]>(statsSql, values);
    const stats = statsRows[0];

    const totalLogins = stats.total_logins || 0;
    const successCount = stats.success_count || 0;
    const failureCount = stats.failure_count || 0;
    const uniqueIps = stats.unique_ips || 0;

    const successRate = totalLogins > 0
      ? ((successCount / totalLogins) * 100).toFixed(2)
      : '0.00';

    // 获取TOP IP列表
    const topIpSql = `
      SELECT
        ip_address,
        COUNT(*) as login_count
      FROM gm_login_logs
      ${whereClause}
      GROUP BY ip_address
      ORDER BY login_count DESC
      LIMIT 10
    `;

    const [topIpRows] = await gmToolsPool.query<RowDataPacket[]>(topIpSql, values);
    const topIps = topIpRows.map(row => ({
      ip_address: row.ip_address,
      login_count: row.login_count
    }));

    const result = {
      total_logins: totalLogins,
      success_count: successCount,
      failure_count: failureCount,
      success_rate: successRate,
      unique_ips: uniqueIps,
      top_ips: topIps
    };

    logger.info('登录统计获取成功', {
      total: result.total_logins,
      success_rate: result.success_rate
    });

    return result;
  } catch (error: any) {
    logger.error('获取登录统计失败', { error: error.message, query });
    throw createApiError('获取登录统计失败', 500, 'STATS_FAILED', error.message);
  }
}

/**
 * 导出操作日志
 */
export async function exportOperationLogs(
  query: OperationLogQuery,
  format: ExportFormat = 'json'
): Promise<string | object[]> {
  logger.info('导出操作日志', { query, format });

  // 移除分页参数，导出所有匹配记录
  const exportQuery = { ...query, page: undefined, page_size: undefined };

  // 查询所有匹配的日志（限制最多10000条）
  const result = await queryOperationLogs({ ...exportQuery, page: 1, page_size: 10000 });

  if (format === 'csv') {
    return convertToCSV(result.logs);
  }

  return result.logs;
}

/**
 * 导出登录日志
 */
export async function exportLoginLogs(
  query: LoginLogQuery,
  format: ExportFormat = 'json'
): Promise<string | object[]> {
  logger.info('导出登录日志', { query, format });

  // 移除分页参数，导出所有匹配记录
  const exportQuery = { ...query, page: undefined, page_size: undefined };

  // 查询所有匹配的日志（限制最多10000条）
  const result = await queryLoginLogs({ ...exportQuery, page: 1, page_size: 10000 });

  if (format === 'csv') {
    return convertLoginLogsToCSV(result.logs);
  }

  return result.logs;
}

/**
 * 将操作日志转换为CSV格式
 */
function convertToCSV(logs: operationModel.GmOperationLog[]): string {
  if (logs.length === 0) {
    return '';
  }

  // CSV表头
  const headers = [
    'log_id',
    'gm_user',
    'gm_id',
    'gm_level',
    'operation_type',
    'operation_category',
    'target_type',
    'target_id',
    'target_name',
    'result',
    'error_message',
    'ip_address',
    'created_at'
  ];

  // CSV行
  const rows = logs.map(log => [
    log.log_id,
    log.gm_user,
    log.gm_id,
    log.gm_level,
    log.operation_type,
    log.operation_category,
    log.target_type,
    log.target_id,
    log.target_name || '',
    log.result,
    log.error_message || '',
    log.ip_address,
    log.created_at.toISOString()
  ]);

  // 转义CSV字段
  const escapeCSV = (value: any): string => {
    const str = String(value);
    if (str.includes(',') || str.includes('"') || str.includes('\n')) {
      return `"${str.replace(/"/g, '""')}"`;
    }
    return str;
  };

  // 生成CSV
  const csvLines = [
    headers.join(','),
    ...rows.map(row => row.map(escapeCSV).join(','))
  ];

  return csvLines.join('\n');
}

/**
 * 将登录日志转换为CSV格式
 */
function convertLoginLogsToCSV(logs: LoginLog[]): string {
  if (logs.length === 0) {
    return '';
  }

  // CSV表头
  const headers = [
    'log_id',
    'gm_user',
    'gm_id',
    'login_status',
    'ip_address',
    'failure_reason',
    'login_time'
  ];

  // CSV行
  const rows = logs.map(log => [
    log.log_id,
    log.gm_user,
    log.gm_id,
    log.login_status,
    log.ip_address,
    log.failure_reason || '',
    log.login_time.toISOString()
  ]);

  // 转义CSV字段
  const escapeCSV = (value: any): string => {
    const str = String(value);
    if (str.includes(',') || str.includes('"') || str.includes('\n')) {
      return `"${str.replace(/"/g, '""')}"`;
    }
    return str;
  };

  // 生成CSV
  const csvLines = [
    headers.join(','),
    ...rows.map(row => row.map(escapeCSV).join(','))
  ];

  return csvLines.join('\n');
}
