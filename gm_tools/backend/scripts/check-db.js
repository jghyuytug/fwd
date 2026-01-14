/**
 * 检查数据库状态
 */

const mysql = require('mysql2/promise');
require('dotenv').config();

async function checkDatabase() {
  let connection;

  try {
    console.log('正在连接数据库...');

    // 创建数据库连接
    connection = await mysql.createConnection({
      host: process.env.DB_HOST || 'localhost',
      port: parseInt(process.env.DB_PORT) || 3307,
      user: process.env.DB_USER || 'root',
      password: process.env.DB_PASSWORD || 'TestRootPass123456',
      database: process.env.DB_NAME || 'gm_tools'
    });

    console.log('✅ 数据库连接成功\n');

    // 检查数据库是否存在
    const [databases] = await connection.query('SHOW DATABASES');
    console.log('📊 可用的数据库:');
    databases.forEach(db => console.log(`  - ${Object.values(db)[0]}`));
    console.log('');

    // 检查当前数据库的表
    const [tables] = await connection.query('SHOW TABLES');
    console.log('📋 gm_tools 数据库中的表:');
    if (tables.length === 0) {
      console.log('  ⚠️ 没有找到任何表！需要执行初始化脚本。');
    } else {
      tables.forEach(table => console.log(`  - ${Object.values(table)[0]}`));
    }
    console.log('');

    // 如果 gm_accounts 表存在，检查用户数据
    try {
      const [users] = await connection.query('SELECT gm_id, username, real_name, gm_level FROM gm_accounts');
      console.log('👥 gm_accounts 表中的用户:');
      if (users.length === 0) {
        console.log('  ⚠️ 表存在但没有用户数据！');
      } else {
        users.forEach(user => {
          console.log(`  - ID: ${user.gm_id}, 用户名: ${user.username}, 姓名: ${user.real_name}, 等级: ${user.gm_level}`);
        });
      }
    } catch (error) {
      console.log('  ⚠️ gm_accounts 表不存在或无法访问');
    }

    await connection.end();
    console.log('\n✅ 检查完成');
    process.exit(0);

  } catch (error) {
    console.error('❌ 检查失败:', error.message);
    console.error('错误详情:', error);
    if (connection) await connection.end();
    process.exit(1);
  }
}

checkDatabase();
