/**
 * 角色数据模型
 * 从Cain镜像数据库查询角色信息
 */

import { cainPool } from '../config/database';
import { RowDataPacket } from 'mysql2';

/**
 * 角色基础信息
 */
export interface Character {
  char_no: number;
  char_name: string;
  m_id: string;
  job: number;
  job_name?: string;
  lev: number;
  grow_type: number;
  grow_type_name?: string;
  exp: number;
  hp: number;
  mp: number;
  delete_flag: number;
  login_time: Date | null;
  logout_time: Date | null;
  create_time: Date;
  dungeon_clear_point: number;
}

/**
 * 角色详细信息（包含更多字段）
 */
export interface CharacterDetail extends Character {
  guild_id: number | null;
  guild_name: string | null;
  fame: number;
  money: bigint;
  pvp_point: number;
  pvp_grade: number;
  title_id: number;
  title_name: string | null;
}

/**
 * 角色搜索参数
 */
export interface CharacterSearchParams {
  name?: string;
  fuzzy?: boolean;
  m_id?: string;
  min_level?: number;
  max_level?: number;
  job?: number;
  page?: number;
  page_size?: number;
}

/**
 * 角色搜索结果
 */
export interface CharacterSearchResult {
  characters: Character[];
  total: number;
  page: number;
  page_size: number;
}

/**
 * 职业名称映射
 */
const JOB_NAMES: Record<number, string> = {
  0: '鬼剑士',
  1: '格斗家',
  2: '神枪手',
  3: '魔法师',
  4: '圣职者',
  5: '暗夜使者',
  6: '守护者',
  7: '魔枪士',
  8: '缔造者',
  9: '黑暗武士',
  10: '枪剑士'
};

/**
 * 转职名称映射（基础职业->转职）
 * 这里只是示例，实际需要根据grow_type完整映射
 */
const GROW_TYPE_NAMES: Record<number, Record<number, string>> = {
  // 鬼剑士
  0: {
    0: '剑魂',
    1: '狂战士',
    2: '阿修罗',
    3: '鬼泣',
    4: '剑宗',
    5: '暗帝',
    6: '剑魔'
  },
  // 格斗家
  1: {
    0: '气功师',
    1: '散打',
    2: '街霸',
    3: '柔道',
    4: '风林火山',
    5: '武神',
    6: '毒王'
  },
  // 神枪手
  2: {
    0: '漫游枪手',
    1: '枪炮师',
    2: '机械师',
    3: '弹药专家',
    4: '特工',
    5: '枪剑士'
  },
  // 魔法师
  3: {
    0: '元素师',
    1: '召唤师',
    2: '战斗法师',
    3: '魔道学者',
    4: '冰结师',
    5: '血法师',
    6: '次元行者'
  },
  // 圣职者
  4: {
    0: '圣骑士',
    1: '蓝拳圣使',
    2: '驱魔师',
    3: '复仇者',
    4: '诱魔者',
    5: '龙骑士'
  }
  // 其他职业...
};

/**
 * 获取职业名称
 */
export function getJobName(job: number): string {
  return JOB_NAMES[job] || `未知职业(${job})`;
}

/**
 * 获取转职名称
 */
export function getGrowTypeName(job: number, growType: number): string {
  return GROW_TYPE_NAMES[job]?.[growType] || `未知转职(${growType})`;
}

/**
 * 根据角色名搜索角色
 */
export async function searchCharactersByName(
  params: CharacterSearchParams
): Promise<CharacterSearchResult> {
  const {
    name,
    fuzzy = false,
    m_id,
    min_level,
    max_level,
    job,
    page = 1,
    page_size = 20
  } = params;

  // 构建查询条件
  const conditions: string[] = ['delete_flag = 0'];
  const values: any[] = [];

  if (name) {
    if (fuzzy) {
      conditions.push('char_name LIKE ?');
      values.push(`%${name}%`);
    } else {
      conditions.push('char_name = ?');
      values.push(name);
    }
  }

  if (m_id) {
    conditions.push('m_id = ?');
    values.push(m_id);
  }

  if (min_level !== undefined) {
    conditions.push('lev >= ?');
    values.push(min_level);
  }

  if (max_level !== undefined) {
    conditions.push('lev <= ?');
    values.push(max_level);
  }

  if (job !== undefined) {
    conditions.push('job = ?');
    values.push(job);
  }

  const whereClause = conditions.join(' AND ');

  // 查询总数
  const countSql = `SELECT COUNT(*) as total FROM charac_info WHERE ${whereClause}`;
  const [countRows] = await cainPool.query<RowDataPacket[]>(countSql, values);
  const total = countRows[0].total;

  // 查询数据
  const offset = (page - 1) * page_size;
  const dataSql = `
    SELECT
      char_no, char_name, m_id, job, lev, grow_type,
      exp, hp, mp, delete_flag,
      login_time, logout_time, create_time, dungeon_clear_point
    FROM charac_info
    WHERE ${whereClause}
    ORDER BY char_no DESC
    LIMIT ? OFFSET ?
  `;
  const [rows] = await cainPool.query<RowDataPacket[]>(dataSql, [...values, page_size, offset]);

  // 转换数据并添加职业名称
  const characters: Character[] = rows.map(row => ({
    ...row,
    job_name: getJobName(row.job),
    grow_type_name: getGrowTypeName(row.job, row.grow_type),
    money: row.money ? BigInt(row.money) : BigInt(0)
  }));

  return {
    characters,
    total,
    page,
    page_size
  };
}

/**
 * 根据角色编号查询详细信息
 */
export async function findCharacterByCharNo(charNo: number): Promise<CharacterDetail | null> {
  const sql = `
    SELECT
      c.char_no, c.char_name, c.m_id, c.job, c.lev, c.grow_type,
      c.exp, c.hp, c.mp, c.delete_flag,
      c.login_time, c.logout_time, c.create_time, c.dungeon_clear_point,
      c.guild_id, g.guild_name,
      c.fame, c.money, c.pvp_point, c.pvp_grade,
      c.title_id
    FROM charac_info c
    LEFT JOIN guild g ON c.guild_id = g.guild_id
    WHERE c.char_no = ? AND c.delete_flag = 0
  `;

  const [rows] = await cainPool.query<RowDataPacket[]>(sql, [charNo]);

  if (rows.length === 0) {
    return null;
  }

  const row = rows[0];

  return {
    ...row,
    job_name: getJobName(row.job),
    grow_type_name: getGrowTypeName(row.job, row.grow_type),
    title_name: null, // TODO: 从title表查询
    money: row.money ? BigInt(row.money) : BigInt(0)
  };
}

/**
 * 根据账号ID查询角色列表
 */
export async function findCharactersByAccountId(mId: string): Promise<Character[]> {
  const sql = `
    SELECT
      char_no, char_name, m_id, job, lev, grow_type,
      exp, hp, mp, delete_flag,
      login_time, logout_time, create_time, dungeon_clear_point
    FROM charac_info
    WHERE m_id = ? AND delete_flag = 0
    ORDER BY char_no DESC
  `;

  const [rows] = await cainPool.query<RowDataPacket[]>(sql, [mId]);

  return rows.map(row => ({
    ...row,
    job_name: getJobName(row.job),
    grow_type_name: getGrowTypeName(row.job, row.grow_type),
    money: row.money ? BigInt(row.money) : BigInt(0)
  }));
}

/**
 * 检查角色是否存在
 */
export async function checkCharacterExists(charNo: number): Promise<boolean> {
  const sql = 'SELECT 1 FROM charac_info WHERE char_no = ? AND delete_flag = 0';
  const [rows] = await cainPool.query<RowDataPacket[]>(sql, [charNo]);
  return rows.length > 0;
}
