/**
 * Swagger配置
 * 配置OpenAPI文档和Swagger UI
 */

import swaggerJsdoc from 'swagger-jsdoc';
import { SwaggerUiOptions } from 'swagger-ui-express';

const API_PREFIX = process.env.API_PREFIX || '/api/v1';

/**
 * Swagger JSDoc配置
 */
const swaggerOptions: swaggerJsdoc.Options = {
  definition: {
    openapi: '3.0.0',
    info: {
      title: 'DNF GM Tools API',
      version: '1.0.0',
      description: '地下城与勇士游戏管理工具后端API文档',
      contact: {
        name: 'GM Tools 开发团队',
        email: 'support@example.com'
      },
      license: {
        name: 'MIT',
        url: 'https://opensource.org/licenses/MIT'
      }
    },
    servers: [
      {
        url: `http://localhost:${process.env.PORT || 3000}${API_PREFIX}`,
        description: '开发环境'
      },
      {
        url: `https://api-test.example.com${API_PREFIX}`,
        description: '测试环境'
      },
      {
        url: `https://api.example.com${API_PREFIX}`,
        description: '生产环境'
      }
    ],
    components: {
      securitySchemes: {
        bearerAuth: {
          type: 'http',
          scheme: 'bearer',
          bearerFormat: 'JWT',
          description: 'JWT身份认证。格式: Bearer {token}'
        }
      },
      schemas: {
        // 通用响应结构
        ApiResponse: {
          type: 'object',
          properties: {
            code: {
              type: 'integer',
              description: 'HTTP状态码',
              example: 200
            },
            message: {
              type: 'string',
              description: '响应消息',
              example: '操作成功'
            },
            data: {
              type: 'object',
              description: '响应数据'
            }
          }
        },

        // 错误响应
        ErrorResponse: {
          type: 'object',
          properties: {
            code: {
              type: 'integer',
              description: 'HTTP状态码',
              example: 400
            },
            message: {
              type: 'string',
              description: '错误消息',
              example: '参数错误'
            },
            error: {
              type: 'string',
              description: '错误代码',
              example: 'INVALID_PARAMETERS'
            }
          }
        },

        // 分页响应
        PaginationResponse: {
          type: 'object',
          properties: {
            code: {
              type: 'integer',
              example: 200
            },
            message: {
              type: 'string',
              example: '查询成功'
            },
            data: {
              type: 'object',
              properties: {
                items: {
                  type: 'array',
                  items: {
                    type: 'object'
                  },
                  description: '数据列表'
                },
                total: {
                  type: 'integer',
                  description: '总记录数',
                  example: 100
                },
                page: {
                  type: 'integer',
                  description: '当前页码',
                  example: 1
                },
                page_size: {
                  type: 'integer',
                  description: '每页记录数',
                  example: 20
                }
              }
            }
          }
        },

        // 认证相关
        LoginRequest: {
          type: 'object',
          required: ['username', 'password'],
          properties: {
            username: {
              type: 'string',
              description: 'GM账号',
              example: 'admin'
            },
            password: {
              type: 'string',
              description: '密码',
              example: 'Admin123!@#'
            }
          }
        },

        LoginResponse: {
          type: 'object',
          properties: {
            access_token: {
              type: 'string',
              description: '访问令牌（有效期2小时）'
            },
            refresh_token: {
              type: 'string',
              description: '刷新令牌（有效期7天）'
            },
            token_type: {
              type: 'string',
              example: 'Bearer'
            },
            expires_in: {
              type: 'integer',
              description: '过期时间（秒）',
              example: 7200
            },
            gm_info: {
              type: 'object',
              properties: {
                gm_id: { type: 'integer' },
                username: { type: 'string' },
                gm_level: { type: 'integer' },
                permissions: {
                  type: 'array',
                  items: { type: 'string' }
                }
              }
            }
          }
        },

        // 角色相关
        Character: {
          type: 'object',
          properties: {
            char_no: { type: 'integer', description: '角色ID' },
            char_name: { type: 'string', description: '角色名称' },
            lev: { type: 'integer', description: '等级' },
            job: { type: 'integer', description: '职业ID' },
            grow_type: { type: 'integer', description: '转职类型' },
            create_time: { type: 'string', format: 'date-time', description: '创建时间' },
            delete_flag: { type: 'integer', description: '删除标记' }
          }
        },

        // 装备相关
        Equipment: {
          type: 'object',
          properties: {
            slot_no: { type: 'integer', description: '装备槽位' },
            item_id: { type: 'integer', description: '物品ID' },
            upgrade: { type: 'integer', description: '强化等级' },
            amplify_option: { type: 'integer', description: '增幅等级' },
            seperate_upgrade: { type: 'integer', description: '锻造等级' }
          }
        },

        // GM操作相关
        GrantItemRequest: {
          type: 'object',
          required: ['char_no', 'item_id', 'amount', 'reason'],
          properties: {
            char_no: {
              type: 'integer',
              description: '目标角色ID',
              example: 123456
            },
            item_id: {
              type: 'integer',
              description: '物品ID',
              example: 100001
            },
            amount: {
              type: 'integer',
              description: '数量',
              minimum: 1,
              maximum: 999,
              example: 10
            },
            reason: {
              type: 'string',
              description: '操作原因',
              minLength: 5,
              maxLength: 200,
              example: '活动奖励补发'
            },
            expire_days: {
              type: 'integer',
              description: '过期天数（可选）',
              minimum: 1,
              maximum: 365,
              example: 30
            }
          }
        },

        SendMailRequest: {
          type: 'object',
          required: ['char_no', 'title', 'content'],
          properties: {
            char_no: {
              type: 'integer',
              description: '目标角色ID',
              example: 123456
            },
            title: {
              type: 'string',
              description: '邮件标题',
              maxLength: 100,
              example: '系统邮件'
            },
            content: {
              type: 'string',
              description: '邮件内容',
              maxLength: 500,
              example: '这是一封测试邮件'
            },
            item_id: {
              type: 'integer',
              description: '附件物品ID（可选）',
              example: 100001
            },
            item_amount: {
              type: 'integer',
              description: '附件物品数量（可选）',
              example: 5
            },
            gold: {
              type: 'integer',
              description: '附件金币（可选）',
              example: 100000
            }
          }
        },

        BanAccountRequest: {
          type: 'object',
          required: ['account_id', 'ban_type', 'duration', 'reason'],
          properties: {
            account_id: {
              type: 'integer',
              description: '账号ID',
              example: 1001
            },
            ban_type: {
              type: 'string',
              enum: ['LOGIN', 'GAME', 'CHAT', 'TRADE'],
              description: '封禁类型'
            },
            duration: {
              type: 'integer',
              description: '封禁时长（小时），0表示永久',
              minimum: 0,
              example: 24
            },
            reason: {
              type: 'string',
              description: '封禁原因',
              minLength: 5,
              maxLength: 200,
              example: '使用外挂'
            }
          }
        },

        // 审计日志相关
        OperationLog: {
          type: 'object',
          properties: {
            log_id: { type: 'integer' },
            gm_id: { type: 'integer' },
            gm_username: { type: 'string' },
            operation_type: {
              type: 'string',
              enum: ['ITEM_GRANT', 'MAIL_SEND', 'ACCOUNT_BAN', 'ACCOUNT_UNBAN', 'LEVEL_MODIFY', 'MONEY_MODIFY']
            },
            target_type: {
              type: 'string',
              enum: ['CHARACTER', 'ACCOUNT']
            },
            target_id: { type: 'integer' },
            operation_details: { type: 'object' },
            before_state: { type: 'object' },
            after_state: { type: 'object' },
            result: {
              type: 'string',
              enum: ['SUCCESS', 'FAILURE']
            },
            error_message: { type: 'string' },
            ip_address: { type: 'string' },
            user_agent: { type: 'string' },
            operation_time: { type: 'string', format: 'date-time' }
          }
        }
      },

      // 通用响应
      responses: {
        UnauthorizedError: {
          description: '未授权 - Token无效或已过期',
          content: {
            'application/json': {
              schema: {
                $ref: '#/components/schemas/ErrorResponse'
              },
              example: {
                code: 401,
                message: '未授权访问',
                error: 'UNAUTHORIZED'
              }
            }
          }
        },

        ForbiddenError: {
          description: '禁止访问 - 权限不足',
          content: {
            'application/json': {
              schema: {
                $ref: '#/components/schemas/ErrorResponse'
              },
              example: {
                code: 403,
                message: '权限不足',
                error: 'FORBIDDEN'
              }
            }
          }
        },

        NotFoundError: {
          description: '资源不存在',
          content: {
            'application/json': {
              schema: {
                $ref: '#/components/schemas/ErrorResponse'
              },
              example: {
                code: 404,
                message: '资源不存在',
                error: 'NOT_FOUND'
              }
            }
          }
        },

        ValidationError: {
          description: '参数验证失败',
          content: {
            'application/json': {
              schema: {
                $ref: '#/components/schemas/ErrorResponse'
              },
              example: {
                code: 400,
                message: '参数验证失败',
                error: 'VALIDATION_ERROR'
              }
            }
          }
        }
      },

      // 通用参数
      parameters: {
        PageParam: {
          name: 'page',
          in: 'query',
          description: '页码（从1开始）',
          schema: {
            type: 'integer',
            minimum: 1,
            default: 1
          }
        },

        PageSizeParam: {
          name: 'page_size',
          in: 'query',
          description: '每页记录数',
          schema: {
            type: 'integer',
            minimum: 1,
            maximum: 100,
            default: 20
          }
        },

        StartDateParam: {
          name: 'start_date',
          in: 'query',
          description: '开始日期（ISO 8601格式）',
          schema: {
            type: 'string',
            format: 'date-time'
          }
        },

        EndDateParam: {
          name: 'end_date',
          in: 'query',
          description: '结束日期（ISO 8601格式）',
          schema: {
            type: 'string',
            format: 'date-time'
          }
        }
      }
    },

    // 全局安全要求
    security: [
      {
        bearerAuth: []
      }
    ],

    // 标签分类
    tags: [
      {
        name: 'Auth',
        description: '认证相关接口'
      },
      {
        name: 'Players',
        description: '玩家查询接口'
      },
      {
        name: 'Operations',
        description: 'GM操作接口'
      },
      {
        name: 'Audit',
        description: '审计日志接口'
      }
    ]
  },

  // API文档源文件路径
  apis: [
    './src/routes/*.ts',
    './src/controllers/*.ts',
    './src/models/*.ts',
    './src/docs/swagger/*.ts'
  ]
};

/**
 * Swagger UI配置
 */
export const swaggerUiOptions: SwaggerUiOptions = {
  customCss: '.swagger-ui .topbar { display: none }',
  customSiteTitle: 'DNF GM Tools API文档',
  swaggerOptions: {
    persistAuthorization: true,
    displayRequestDuration: true,
    filter: true,
    tryItOutEnabled: true
  }
};

/**
 * 生成Swagger规范
 */
export const swaggerSpec = swaggerJsdoc(swaggerOptions);
