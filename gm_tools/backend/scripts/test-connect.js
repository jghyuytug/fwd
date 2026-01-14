/**
 * 测试MySQL连接
 */

const mysql = require('mysql2/promise');
require('dotenv').config();

async function testConnect() {
  let connection;

  try {
    const config = {
      host: process.env.DB_HOST || 'localhost',
      port: parseInt(process.env.DB_PORT) || 3307,
      user: process.env.DB_USER || 'root',
      password: process.env.DB_PASSWORD || 'TestRootPass123456'
    };

    console.log('连接配置:');
    console.log('  Host:', config.host);
    console.log('  Port:', config.port);
    console.log('  User:', config.user);
    console.log('  Password:', config.password ? '***' : '(empty)');
    console.log('');

    console.log('尝试连接...');
    connection = await mysql.createConnection(config);

    console.log('✅ 连接成功！');

    const [result] = await connection.query('SELECT 1+1 AS result');
    console.log('测试查询结果:', result);

    await connection.end();
    process.exit(0);

  } catch (error) {
    console.error('❌ 连接失败');
    console.error('错误信息:', error.message);
    console.error('错误代码:', error.code);
    console.error('错误编号:', error.errno);
    console.error('SQL状态:', error.sqlState);
    console.error('');
    console.error('完整错误:', error);
    if (connection) await connection.end();
    process.exit(1);
  }
}

testConnect();
