const express = require('express');
const { createProxyMiddleware } = require('http-proxy-middleware');
const crypto = require('crypto');

const app = express();
const PORT = 3000;

// 添加请求日志中间件
app.use((req, res, next) => {
  console.log(`${new Date().toISOString()} - ${req.method} ${req.url} - IP: ${req.ip}`);
  next();
});

// AES加密解密函数 (与客户端保持一致)
function aesDecrypt(encryptedData) {
  const key = 'dnf_login_key_123';
  const iv = 'dnf_login_iv_456';

  const decipher = crypto.createDecipheriv('aes-128-cbc', key, iv);
  let decrypted = decipher.update(encryptedData, 'binary', 'utf8');
  decrypted += decipher.final('utf8');
  return decrypted;
}

function aesEncrypt(data) {
  const key = 'dnf_login_key_123';
  const iv = 'dnf_login_iv_456';

  const cipher = crypto.createCipheriv('aes-128-cbc', key, iv);
  let encrypted = cipher.update(data, 'utf8', 'binary');
  encrypted += cipher.final('binary');
  return encrypted;
}

// 中间件：解析原始POST数据
app.use('/dnflogin/dnf.php', express.raw({ type: '*/*' }));

// 登录请求处理
app.post('/dnflogin/dnf.php', async (req, res) => {
  try {
    const encryptedData = req.body;

    if (!encryptedData) {
      return res.send(aesEncrypt('login_failed|no_data'));
    }

    // 解密客户端数据
    const decryptedData = aesDecrypt(encryptedData);
    console.log('收到解密数据:', decryptedData);

    // 解析登录数据: login|username|password_md5|mac
    const parts = decryptedData.split('|');
    if (parts.length < 4 || parts[0] !== 'login') {
      return res.send(aesEncrypt('login_failed|invalid_format'));
    }

    const [action, username, passwordMd5, mac] = parts;

    // 转发到登录服务进行验证
    const loginServiceUrl = process.env.LOGIN_SERVICE_URL || 'http://localhost:8080';

    try {
      const response = await fetch(`${loginServiceUrl}/auth`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username, passwordMd5, mac })
      });

      const result = await response.json();

      if (result.success) {
        // 登录成功，返回游戏启动参数
        const gameParams = `/path/to/dnf.exe|start_game|${username}`;
        res.send(aesEncrypt(gameParams));
      } else {
        res.send(aesEncrypt('login_failed|invalid_credentials'));
      }
    } catch (error) {
      console.error('登录服务调用失败:', error);
      res.send(aesEncrypt('login_failed|service_unavailable'));
    }

  } catch (error) {
    console.error('处理登录请求失败:', error);
    res.send(aesEncrypt('login_failed|server_error'));
  }
});

// 代理其他请求到游戏服务
app.use('/game', createProxyMiddleware({
  target: process.env.GAME_SERVICE_URL || 'http://localhost:9090',
  changeOrigin: true,
  pathRewrite: { '^/game': '' }
}));

// 根路径
app.get('/', (req, res) => {
  res.json({ message: 'DNF Login API Gateway', status: 'running', version: '1.0.0' });
});

// 健康检查
app.get('/health', (req, res) => {
  res.json({ status: 'ok', service: 'api-gateway' });
});

// 捕获所有未匹配的请求，返回404并记录
app.use((req, res) => {
  console.log(`404 - 未找到路由: ${req.method} ${req.url}`);
  res.status(404).json({ error: 'Page not found', message: 'The requested page could not be found.' });
});

app.listen(PORT, () => {
  console.log(`DNF API网关运行在端口 ${PORT}`);
});