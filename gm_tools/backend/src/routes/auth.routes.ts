/**
 * 认证路由
 * 定义认证相关的API端点
 */

import { Router } from 'express';
import * as authController from '../controllers/auth.controller';
import { authenticate } from '../middlewares/auth';

const router = Router();

/**
 * @swagger
 * /auth/login:
 *   post:
 *     tags: [Auth]
 *     summary: 用户登录
 *     description: GM账号登录，返回JWT access token和refresh token
 *     security: []
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             $ref: '#/components/schemas/LoginRequest'
 *     responses:
 *       200:
 *         description: 登录成功
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 code:
 *                   type: integer
 *                   example: 200
 *                 message:
 *                   type: string
 *                   example: '登录成功'
 *                 data:
 *                   $ref: '#/components/schemas/LoginResponse'
 *       400:
 *         $ref: '#/components/responses/ValidationError'
 *       401:
 *         description: 用户名或密码错误
 *         content:
 *           application/json:
 *             schema:
 *               $ref: '#/components/schemas/ErrorResponse'
 *             example:
 *               code: 401
 *               message: '用户名或密码错误'
 *               error: 'INVALID_CREDENTIALS'
 */
router.post('/login', authController.login);

/**
 * @swagger
 * /auth/refresh:
 *   post:
 *     tags: [Auth]
 *     summary: 刷新Access Token
 *     description: 使用refresh token获取新的access token
 *     security: []
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required:
 *               - refresh_token
 *             properties:
 *               refresh_token:
 *                 type: string
 *                 description: 刷新令牌
 *     responses:
 *       200:
 *         description: Token刷新成功
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 code:
 *                   type: integer
 *                   example: 200
 *                 message:
 *                   type: string
 *                   example: 'Token刷新成功'
 *                 data:
 *                   type: object
 *                   properties:
 *                     access_token:
 *                       type: string
 *                     token_type:
 *                       type: string
 *                       example: 'Bearer'
 *                     expires_in:
 *                       type: integer
 *                       example: 7200
 *       401:
 *         description: Refresh token无效或已过期
 *         content:
 *           application/json:
 *             schema:
 *               $ref: '#/components/schemas/ErrorResponse'
 */
router.post('/refresh', authController.refresh);

/**
 * @swagger
 * /auth/logout:
 *   post:
 *     tags: [Auth]
 *     summary: 用户登出
 *     description: 使当前access token失效
 *     responses:
 *       200:
 *         description: 登出成功
 *         content:
 *           application/json:
 *             schema:
 *               $ref: '#/components/schemas/ApiResponse'
 *       401:
 *         $ref: '#/components/responses/UnauthorizedError'
 */
router.post('/logout', authenticate, authController.logout);

/**
 * @swagger
 * /auth/me:
 *   get:
 *     tags: [Auth]
 *     summary: 获取当前用户信息
 *     description: 返回当前登录用户的详细信息
 *     responses:
 *       200:
 *         description: 获取成功
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 code:
 *                   type: integer
 *                   example: 200
 *                 message:
 *                   type: string
 *                 data:
 *                   type: object
 *                   properties:
 *                     gm_id:
 *                       type: integer
 *                     username:
 *                       type: string
 *                     email:
 *                       type: string
 *                     gm_level:
 *                       type: integer
 *                     permissions:
 *                       type: array
 *                       items:
 *                         type: string
 *       401:
 *         $ref: '#/components/responses/UnauthorizedError'
 */
router.get('/me', authenticate, authController.getCurrentUser);

/**
 * @swagger
 * /auth/2fa/setup:
 *   post:
 *     tags: [Auth]
 *     summary: 设置双因素认证
 *     description: 生成2FA密钥和二维码
 *     responses:
 *       200:
 *         description: 设置成功
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 code:
 *                   type: integer
 *                 message:
 *                   type: string
 *                 data:
 *                   type: object
 *                   properties:
 *                     secret:
 *                       type: string
 *                       description: 2FA密钥
 *                     qr_code:
 *                       type: string
 *                       description: 二维码URL
 *       401:
 *         $ref: '#/components/responses/UnauthorizedError'
 */
router.post('/2fa/setup', authenticate, authController.setupTwoFactor);

/**
 * @swagger
 * /auth/2fa/verify:
 *   post:
 *     tags: [Auth]
 *     summary: 验证并启用双因素认证
 *     description: 验证2FA代码并启用
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required:
 *               - token
 *             properties:
 *               token:
 *                 type: string
 *                 description: 6位验证码
 *                 example: '123456'
 *     responses:
 *       200:
 *         description: 验证成功，2FA已启用
 *       400:
 *         description: 验证码错误
 *       401:
 *         $ref: '#/components/responses/UnauthorizedError'
 */
router.post('/2fa/verify', authenticate, authController.verifyTwoFactor);

/**
 * @swagger
 * /auth/2fa/disable:
 *   post:
 *     tags: [Auth]
 *     summary: 禁用双因素认证
 *     description: 禁用当前账号的2FA
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required:
 *               - password
 *             properties:
 *               password:
 *                 type: string
 *                 description: 当前密码
 *     responses:
 *       200:
 *         description: 2FA已禁用
 *       400:
 *         description: 密码错误
 *       401:
 *         $ref: '#/components/responses/UnauthorizedError'
 */
router.post('/2fa/disable', authenticate, authController.disableTwoFactor);

/**
 * @swagger
 * /auth/trusted-devices:
 *   get:
 *     tags: [Auth]
 *     summary: 获取信任设备列表
 *     description: 返回当前用户的所有信任设备
 *     responses:
 *       200:
 *         description: 获取成功
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 code:
 *                   type: integer
 *                 message:
 *                   type: string
 *                 data:
 *                   type: object
 *                   properties:
 *                     devices:
 *                       type: array
 *                       items:
 *                         type: object
 *                         properties:
 *                           device_id:
 *                             type: string
 *                           device_name:
 *                             type: string
 *                           last_used:
 *                             type: string
 *                             format: date-time
 *       401:
 *         $ref: '#/components/responses/UnauthorizedError'
 */
router.get('/trusted-devices', authenticate, authController.getTrustedDevices);

/**
 * @swagger
 * /auth/trusted-devices/{device_id}:
 *   delete:
 *     tags: [Auth]
 *     summary: 撤销信任设备
 *     description: 从信任设备列表中移除指定设备
 *     parameters:
 *       - name: device_id
 *         in: path
 *         required: true
 *         description: 设备ID
 *         schema:
 *           type: string
 *     responses:
 *       200:
 *         description: 撤销成功
 *       404:
 *         description: 设备不存在
 *       401:
 *         $ref: '#/components/responses/UnauthorizedError'
 */
router.delete('/trusted-devices/:device_id', authenticate, authController.revokeTrustedDevice);

export default router;
