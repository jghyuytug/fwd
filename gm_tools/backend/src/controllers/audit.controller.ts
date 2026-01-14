/**
 * 审计日志控制器
 * 处理审计日志查询和统计的HTTP请求
 */

import { Request, Response } from 'express';
import * as auditService from '../services/audit.service';

/**
 * 查询操作日志
 * GET /api/v1/audit/operations
 */
export async function queryOperationLogs(req: Request, res: Response): Promise<void> {
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
    page,
    page_size
  } = req.query;

  const query: auditService.OperationLogQuery = {
    gm_user: gm_user as string | undefined,
    gm_id: gm_id ? parseInt(gm_id as string) : undefined,
    operation_type: operation_type as string | undefined,
    operation_category: operation_category as string | undefined,
    target_type: target_type as string | undefined,
    target_id: target_id as string | undefined,
    result: result as string | undefined,
    start_date: start_date as string | undefined,
    end_date: end_date as string | undefined,
    page: page ? parseInt(page as string) : undefined,
    page_size: page_size ? parseInt(page_size as string) : undefined
  };

  const resultData = await auditService.queryOperationLogs(query);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: resultData
  });
}

/**
 * 查询登录日志
 * GET /api/v1/audit/logins
 */
export async function queryLoginLogs(req: Request, res: Response): Promise<void> {
  const {
    gm_user,
    gm_id,
    login_status,
    start_date,
    end_date,
    ip_address,
    page,
    page_size
  } = req.query;

  const query: auditService.LoginLogQuery = {
    gm_user: gm_user as string | undefined,
    gm_id: gm_id ? parseInt(gm_id as string) : undefined,
    login_status: login_status as 'SUCCESS' | 'FAILURE' | undefined,
    start_date: start_date as string | undefined,
    end_date: end_date as string | undefined,
    ip_address: ip_address as string | undefined,
    page: page ? parseInt(page as string) : undefined,
    page_size: page_size ? parseInt(page_size as string) : undefined
  };

  const result = await auditService.queryLoginLogs(query);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: result
  });
}

/**
 * 获取操作统计
 * GET /api/v1/audit/stats/operations
 */
export async function getOperationStats(req: Request, res: Response): Promise<void> {
  const { gm_id, start_date, end_date } = req.query;

  const query: auditService.StatsQuery = {
    gm_id: gm_id ? parseInt(gm_id as string) : undefined,
    start_date: start_date as string | undefined,
    end_date: end_date as string | undefined
  };

  const stats = await auditService.getOperationStats(query);

  res.status(200).json({
    code: 200,
    message: '统计成功',
    data: stats
  });
}

/**
 * 获取登录统计
 * GET /api/v1/audit/stats/logins
 */
export async function getLoginStats(req: Request, res: Response): Promise<void> {
  const { gm_id, start_date, end_date } = req.query;

  const query: auditService.StatsQuery = {
    gm_id: gm_id ? parseInt(gm_id as string) : undefined,
    start_date: start_date as string | undefined,
    end_date: end_date as string | undefined
  };

  const stats = await auditService.getLoginStats(query);

  res.status(200).json({
    code: 200,
    message: '统计成功',
    data: stats
  });
}

/**
 * 导出操作日志
 * GET /api/v1/audit/export/operations
 */
export async function exportOperationLogs(req: Request, res: Response): Promise<void> {
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
    format
  } = req.query;

  const query: auditService.OperationLogQuery = {
    gm_user: gm_user as string | undefined,
    gm_id: gm_id ? parseInt(gm_id as string) : undefined,
    operation_type: operation_type as string | undefined,
    operation_category: operation_category as string | undefined,
    target_type: target_type as string | undefined,
    target_id: target_id as string | undefined,
    result: result as string | undefined,
    start_date: start_date as string | undefined,
    end_date: end_date as string | undefined
  };

  const exportFormat = (format as auditService.ExportFormat) || 'json';

  const data = await auditService.exportOperationLogs(query, exportFormat);

  if (exportFormat === 'csv') {
    res.setHeader('Content-Type', 'text/csv; charset=utf-8');
    res.setHeader('Content-Disposition', `attachment; filename="operation_logs_${Date.now()}.csv"`);
    res.status(200).send(data);
  } else {
    res.status(200).json({
      code: 200,
      message: '导出成功',
      data
    });
  }
}

/**
 * 导出登录日志
 * GET /api/v1/audit/export/logins
 */
export async function exportLoginLogs(req: Request, res: Response): Promise<void> {
  const {
    gm_user,
    gm_id,
    login_status,
    start_date,
    end_date,
    ip_address,
    format
  } = req.query;

  const query: auditService.LoginLogQuery = {
    gm_user: gm_user as string | undefined,
    gm_id: gm_id ? parseInt(gm_id as string) : undefined,
    login_status: login_status as 'SUCCESS' | 'FAILURE' | undefined,
    start_date: start_date as string | undefined,
    end_date: end_date as string | undefined,
    ip_address: ip_address as string | undefined
  };

  const exportFormat = (format as auditService.ExportFormat) || 'json';

  const data = await auditService.exportLoginLogs(query, exportFormat);

  if (exportFormat === 'csv') {
    res.setHeader('Content-Type', 'text/csv; charset=utf-8');
    res.setHeader('Content-Disposition', `attachment; filename="login_logs_${Date.now()}.csv"`);
    res.status(200).send(data);
  } else {
    res.status(200).json({
      code: 200,
      message: '导出成功',
      data
    });
  }
}
