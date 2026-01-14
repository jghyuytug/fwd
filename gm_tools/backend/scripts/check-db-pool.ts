/**
 * 使用现有连接池检查数据库状态
 */

import { gmToolsPool } from '../src/config/database';

async function checkDatabase() {
  try {
    console.log('正在检查数据库...\n');

    // 检查当前数据库的表
    const [tables] = await gmToolsPool.query('SHOW TABLES');
    console.log('📋 gm_tools 数据库中的表:');
    if (Array.isArray(tables) && tables.length === 0) {
      console.log('  ⚠️ 没有找到任何表！需要执行初始化脚本。');
    } else if (Array.isArray(tables)) {
      tables.forEach((table: any) => console.log(`  - ${Object.values(table)[0]}`));
    }
    console.log('');

    // 如果 gm_accounts 表存在，检查用户数据
    try {
      const [users] = await gmToolsPool.query('SELECT gm_id, username, real_name, gm_level FROM gm_accounts');
      console.log('👥 gm_accounts 表中的用户:');
      if (Array.isArray(users) && users.length === 0) {
        console.log('  ⚠️ 表存在但没有用户数据！');
      } else if (Array.isArray(users)) {
        users.forEach((user: any) => {
          console.log(`  - ID: ${user.gm_id}, 用户名: ${user.username}, 姓名: ${user.real_name}, 等级: ${user.gm_level}`);
        });
      }
    } catch (error: any) {
      console.log('  ⚠️ gm_accounts 表不存在:', error.message);
    }

    console.log('\n✅ 检查完成');
    await gmToolsPool.end();
    process.exit(0);

  } catch (error: any) {
    console.error('❌ 检查失败:', error.message);
    console.error('错误详情:', error);
    await gmToolsPool.end();
    process.exit(1);
  }
}

checkDatabase();
