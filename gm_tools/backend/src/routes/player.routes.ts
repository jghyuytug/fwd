/**
 * 玩家查询路由
 * 定义所有玩家相关的API端点
 */

import { Router } from 'express';
import * as playerController from '../controllers/player.controller';
import { authenticate } from '../middlewares/auth';
import { requirePermission } from '../middlewares/rbac';
import { auditLog } from '../middlewares/audit';

const router = Router();

// 所有玩家查询路由都需要认证
router.use(authenticate);

/**
 * 角色相关路由
 */

// 搜索角色
// GET /api/v1/players/characters/search?name=xxx&fuzzy=true
router.get(
  '/characters/search',
  requirePermission('player:read'),
  auditLog('角色搜索'),
  playerController.searchCharacters
);

// 查询角色详细信息
// GET /api/v1/players/characters/:char_no
router.get(
  '/characters/:char_no',
  requirePermission('player:read'),
  auditLog('查询角色详情'),
  playerController.getCharacterDetail
);

// 查询角色装备
// GET /api/v1/players/characters/:char_no/equipment
router.get(
  '/characters/:char_no/equipment',
  requirePermission('player:read'),
  auditLog('查询角色装备'),
  playerController.getCharacterEquipment
);

// 查询角色背包
// GET /api/v1/players/characters/:char_no/inventory
router.get(
  '/characters/:char_no/inventory',
  requirePermission('player:read'),
  auditLog('查询角色背包'),
  playerController.getCharacterInventory
);

// 查询角色仓库
// GET /api/v1/players/characters/:char_no/warehouse
router.get(
  '/characters/:char_no/warehouse',
  requirePermission('player:read'),
  auditLog('查询角色仓库'),
  playerController.getCharacterWarehouse
);

// 查询角色金币
// GET /api/v1/players/characters/:char_no/money
router.get(
  '/characters/:char_no/money',
  requirePermission('player:read'),
  auditLog('查询角色金币'),
  playerController.getCharacterMoney
);

// 查询角色在线状态
// GET /api/v1/players/characters/:char_no/online
router.get(
  '/characters/:char_no/online',
  requirePermission('player:read'),
  auditLog('查询角色在线状态'),
  playerController.checkCharacterOnline
);

/**
 * 账号相关路由
 */

// 根据账号名搜索账号
// GET /api/v1/players/accounts/search?name=xxx
router.get(
  '/accounts/search',
  requirePermission('player:read:sensitive'),
  auditLog('搜索账号'),
  playerController.searchAccount
);

// 查询账号详细信息
// GET /api/v1/players/accounts/:m_id
router.get(
  '/accounts/:m_id',
  requirePermission('player:read:sensitive'),
  auditLog('查询账号详情'),
  playerController.getAccountDetail
);

export default router;
