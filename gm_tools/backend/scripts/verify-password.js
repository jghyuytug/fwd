/**
 * 验证密码哈希
 */

const bcrypt = require('bcryptjs');

async function verifyPassword() {
  const password = 'Admin@123456';
  const hash = '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy';

  console.log('测试密码:', password);
  console.log('数据库哈希:', hash);
  console.log('');

  try {
    const isMatch = await bcrypt.compare(password, hash);
    console.log('密码匹配:', isMatch ? '✅ 是' : '❌ 否');

    if (!isMatch) {
      console.log('');
      console.log('生成新的哈希值...');
      const newHash = await bcrypt.hash(password, 10);
      console.log('新哈希值:', newHash);
      console.log('');
      console.log('验证新哈希值...');
      const newMatch = await bcrypt.compare(password, newHash);
      console.log('新哈希匹配:', newMatch ? '✅ 是' : '❌ 否');
    }

    process.exit(0);
  } catch (error) {
    console.error('错误:', error);
    process.exit(1);
  }
}

verifyPassword();
