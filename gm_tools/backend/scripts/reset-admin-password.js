/**
 * 重置admin密码
 */

const mysql = require('mysql2/promise');
const bcrypt = require('bcryptjs');
require('dotenv').config();

async function resetPassword() {
  let connection;

  try {
    console.log('正在连接数据库...');

    connection = await mysql.createConnection({
      host: process.env.DB_HOST || 'localhost',
      port: parseInt(process.env.DB_PORT) || 3307,
      user: process.env.DB_USER || 'root',
      password: process.env.DB_PASSWORD || 'TestRootPass123456',
      database: 'gm_tools',
      charset: 'utf8mb4'
    });

    console.log('✅ 数据库连接成功\n');

    // 生成新密码哈希
    const password = 'Admin@123456';
    console.log('生成密码哈希...');
    const passwordHash = await bcrypt.hash(password, 10);
    console.log('新密码哈希:', passwordHash);
    console.log('');

    // 更新数据库
    console.log('更新数据库...');
    await connection.query(
      'UPDATE gm_accounts SET password_hash = ? WHERE username = ?',
      [passwordHash, 'admin']
    );

    // 验证更新
    const [users] = await connection.query(
      'SELECT username, password_hash FROM gm_accounts WHERE username = ?',
      ['admin']
    );

    if (users.length > 0) {
      console.log('✅ 密码更新成功');
      console.log('用户名:', users[0].username);
      console.log('新哈希:', users[0].password_hash);
      console.log('');

      // 验证新密码
      const isMatch = await bcrypt.compare(password, users[0].password_hash);
      console.log('密码验证:', isMatch ? '✅ 成功' : '❌ 失败');
    }

    await connection.end();
    console.log('\n✅ 完成');
    console.log('  用户名: admin');
    console.log('  密码: Admin@123456');
    process.exit(0);

  } catch (error) {
    console.error('❌ 错误:', error.message);
    if (connection) await connection.end();
    process.exit(1);
  }
}

resetPassword();
