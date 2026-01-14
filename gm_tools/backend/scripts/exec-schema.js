/**
 * 执行SQL架构文件
 */

const mysql = require('mysql2/promise');
const fs = require('fs');
const path = require('path');
require('dotenv').config();

async function executeSchema() {
  let connection;

  try {
    console.log('正在连接数据库...');

    // 创建数据库连接（启用multipleStatements）
    connection = await mysql.createConnection({
      host: process.env.DB_HOST || 'localhost',
      port: parseInt(process.env.DB_PORT) || 3307,
      user: process.env.DB_USER || 'root',
      password: process.env.DB_PASSWORD || 'TestRootPass123456',
      charset: 'utf8mb4',  // 支持中文和emoji
      multipleStatements: true  // 允许执行多条SQL语句
    });

    console.log('✅ 数据库连接成功\n');

    // 读取SQL文件
    const schemaPath = path.join(__dirname, '../../database/schema_gm_tools.sql');
    const sqlContent = fs.readFileSync(schemaPath, 'utf8');

    console.log('正在执行SQL架构文件...');

    // 执行SQL语句
    const results = await connection.query(sqlContent);

    console.log('✅ SQL架构执行成功\n');

    // 切换到gm_tools数据库
    await connection.query('USE gm_tools');

    // 检查创建的表
    const [tables] = await connection.query('SHOW TABLES');
    console.log('📋 已创建的表:');
    tables.forEach(table => console.log(`  - ${Object.values(table)[0]}`));
    console.log('');

    // 检查admin用户
    const [users] = await connection.query('SELECT gm_id, username, real_name, gm_level FROM gm_accounts');
    console.log('👥 gm_accounts 表中的用户:');
    users.forEach(user => {
      console.log(`  - ID: ${user.gm_id}, 用户名: ${user.username}, 姓名: ${user.real_name}, 等级: ${user.gm_level}`);
    });

    await connection.end();
    console.log('\n✅ 数据库初始化完成');
    console.log('  用户名: admin');
    console.log('  密码: Admin@123456');
    process.exit(0);

  } catch (error) {
    console.error('❌ 执行失败:', error.message);
    if (error.code) {
      console.error('错误代码:', error.code);
    }
    if (error.errno) {
      console.error('错误编号:', error.errno);
    }
    if (connection) await connection.end();
    process.exit(1);
  }
}

executeSchema();
