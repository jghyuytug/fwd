# Configuration Files

This directory contains configuration templates for the DNF Game Server.

## Configuration Files

### 1. server.conf
Main server configuration file containing:
- Server basic settings (port, bind address, max connections)
- Network settings (buffers, timeouts, packet sizes)
- Module parameters (users, inventory, events)
- Security settings (anti-cheat, validation)
- Performance tuning
- Logging configuration
- Maintenance mode settings

### 2. database.conf
Database connection pool configuration:
- Account Database: User accounts and credentials
- Character Database: Character data and progression
- Game Database: Game world state, items, dungeons
- Log Database: Audit logs and statistics

**Important**: Update the `password` fields with actual database credentials before deployment.

### 3. auth_backend.conf
Authentication and session management:
- Authentication methods (database, LDAP)
- Session timeouts and management
- Password hashing settings
- Security policies (IP validation, hardware ID)
- Rate limiting
- Login attempt tracking

## Quick Start

### 1. Database Setup
```bash
# Edit database credentials
vi config/database.conf

# Update the password fields:
# password = your_secure_password
```

### 2. Server Configuration
```bash
# Edit server settings
vi config/server.conf

# Adjust parameters as needed:
# - port (default: 7600)
# - max_connections (default: 10000)
# - max_users (default: 5000)
```

### 3. Authentication Setup
```bash
# Configure authentication
vi config/auth_backend.conf

# Review security settings:
# - session_timeout
# - max_sessions_per_account
# - max_login_attempts
```

### 4. Create Required Directories
```bash
# Create log directory
mkdir -p logs

# Set permissions
chmod 755 logs
```

### 5. Start Server
```bash
# From project root
./bin/df_game_server
```

## Configuration Tips

### Production Deployment
- **Database**: Use separate database servers for each pool
- **Security**: Enable `require_secure_connection` in auth_backend.conf
- **Performance**: Adjust `worker_threads` based on CPU cores
- **Logging**: Set `log_level = warning` or `error` for production

### Development Environment
- **Logging**: Use `log_level = debug` for detailed logs
- **Database**: Can use single MySQL instance with multiple databases
- **Security**: Disable strict validation for testing

### High-Load Servers
- Increase `max_connections` and `max_users`
- Add more `worker_threads`
- Increase database `max_connections` per pool
- Enable `enable_compression` for bandwidth savings

## File Format

All configuration files use INI format:
```ini
[Section]
key = value
# Comments start with #
```

## Security Notes

1. **Never commit real passwords to version control**
2. **Use strong passwords for database connections**
3. **Restrict config file permissions**: `chmod 600 config/*.conf`
4. **Enable IP validation in production**
5. **Use firewall rules to restrict database access**

## Troubleshooting

### Server won't start
- Check configuration syntax
- Verify database credentials
- Ensure required directories exist (logs/)
- Check port availability (default: 7600)

### Database connection errors
- Verify MySQL is running
- Check host/port in database.conf
- Test credentials manually: `mysql -h host -u user -p`
- Check firewall rules

### Authentication issues
- Review auth_backend.conf settings
- Check authentication log: `logs/authentication.log`
- Verify database schema for user accounts

## Advanced Configuration

### Load Balancing
For multi-server setups:
1. Use shared database cluster
2. Implement session persistence
3. Configure load balancer for port 7600

### Monitoring
Enable performance monitoring:
```ini
[Performance]
performance_monitoring = yes
performance_log_interval = 60
```

### Scheduled Maintenance
Configure automatic restart:
```ini
[Maintenance]
scheduled_restart_time = 04:00
restart_warning_interval = 300
```

## Support

For configuration help:
- See docs/CONFIGURATION.md for detailed parameter descriptions
- Check logs/server.log for startup errors
- Review main README.md for build and deployment guide
