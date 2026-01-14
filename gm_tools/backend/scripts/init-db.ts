/**
 * 数据库初始化脚本
 * 创建测试管理员账号
 */

import { gmToolsPool } from '../src/config/database';
import bcrypt from 'bcryptjs';

async function initDatabase() {
  try {
    console.log('开始初始化数据库...');

    // 检查是否已存在admin用户
    const [users] = await gmToolsPool.query(
      'SELECT gm_id FROM gm_accounts WHERE username = ?',
      ['admin']
    );

    if (Array.isArray(users) && users.length > 0) {
      console.log('✅ Admin用户已存在，无需创建');
      process.exit(0);
    }

    // 创建密码哈希 (密码: Admin123!@#)
    const passwordHash = await bcrypt.hash('Admin123!@#', 10);

    // 插入admin用户
    await gmToolsPool.query(
      `INSERT INTO gm_accounts (username, password_hash, real_name, gm_level, two_factor_enabled)
       VALUES (?, ?, ?, ?, ?)`,
      ['admin', passwordHash, '系统管理员', 5, false]
    );

    console.log('✅ Admin用户创建成功');
    console.log('   用户名: admin');
    console.log('   密码: Admin123!@#');
    console.log('   GM等级: 5 (超级管理员)');

    process.exit(0);

  } catch (error) {
    console.error('❌ 初始化失败:', error);
    process.exit(1);
  }
}

initDatabase();
