/**
 * 数据库初始化脚本（JavaScript版本）
 * 创建测试管理员账号
 */

const mysql = require('mysql2/promise');
const bcrypt = require('bcryptjs');
require('dotenv').config();

async function initDatabase() {
  let connection;

  try {
    console.log('开始初始化数据库...');

    // 创建数据库连接
    connection = await mysql.createConnection({
      host: process.env.DB_HOST || 'localhost',
      port: parseInt(process.env.DB_PORT) || 3307,
      user: process.env.DB_USER || 'root',
      password: process.env.DB_PASSWORD || 'TestRootPass123456',
      database: process.env.DB_NAME || 'gm_tools'
    });

    console.log('✅ 数据库连接成功');

    // 检查是否已存在admin用户
    const [users] = await connection.query(
      'SELECT gm_id FROM gm_accounts WHERE username = ?',
      ['admin']
    );

    if (users.length > 0) {
      console.log('✅ Admin用户已存在，无需创建');
      await connection.end();
      process.exit(0);
    }

    // 创建密码哈希 (密码: Admin123!@#)
    const passwordHash = await bcrypt.hash('Admin123!@#', 10);

    // 插入admin用户
    await connection.query(
      `INSERT INTO gm_accounts (username, password_hash, real_name, gm_level, two_factor_enabled)
       VALUES (?, ?, ?, ?, ?)`,
      ['admin', passwordHash, '系统管理员', 5, false]
    );

    console.log('✅ Admin用户创建成功');
    console.log('   用户名: admin');
    console.log('   密码: Admin123!@#');
    console.log('   GM等级: 5 (超级管理员)');

    await connection.end();
    process.exit(0);

  } catch (error) {
    console.error('❌ 初始化失败:',error.message);
    if (connection) await connection.end();
    process.exit(1);
  }
}

initDatabase();
