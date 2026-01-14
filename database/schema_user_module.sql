-- ============================================================
-- User Module Database Schema
-- Purpose: Database tables for User module (Phase 6.1)
-- Database: taiwan_cain (main game database)
-- ============================================================

-- Drop existing tables (if recreating)
-- DROP TABLE IF EXISTS user_sessions;
-- DROP TABLE IF EXISTS characters;
-- DROP TABLE IF EXISTS user_accounts;

-- ============================================================
-- Table: user_accounts
-- Purpose: Store user account information
-- ============================================================

CREATE TABLE IF NOT EXISTS user_accounts (
    -- Primary Key
    account_id INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,

    -- Basic Information
    account_name VARCHAR(64) UNIQUE NOT NULL,
    password_hash VARCHAR(128) NOT NULL,

    -- Status
    status TINYINT UNSIGNED DEFAULT 0 COMMENT '0=Active, 1=Blocked, 2=Banned, 3=Deleted, 4=Pending',
    user_type TINYINT UNSIGNED DEFAULT 1 COMMENT '0=Guest, 1=Player, 2=Premium, 10=GM, 11=Admin',

    -- Registration
    register_date INT UNSIGNED NOT NULL,
    register_ip VARCHAR(64),

    -- Login Information
    last_login_date INT UNSIGNED,
    last_login_ip VARCHAR(64),
    current_login_time INT UNSIGNED,
    is_currently_online TINYINT UNSIGNED DEFAULT 0,

    -- Block/Ban Information
    is_blocked TINYINT UNSIGNED DEFAULT 0,
    block_start_time INT UNSIGNED DEFAULT 0,
    block_expire_time INT UNSIGNED DEFAULT 0 COMMENT '0 = permanent ban',
    block_reason VARCHAR(256),
    block_by_gm_id INT UNSIGNED DEFAULT 0,

    -- VIP Information
    vip_level TINYINT UNSIGNED DEFAULT 0 COMMENT '0=None, 1=Bronze, 2=Silver, 3=Gold, 4=Platinum, 5=Diamond',
    vip_expire_time INT UNSIGNED DEFAULT 0,
    vip_points INT UNSIGNED DEFAULT 0,
    vip_benefits_flags TINYINT UNSIGNED DEFAULT 0,

    -- Security Information
    security_question VARCHAR(256),
    security_answer_hash VARCHAR(128),
    email VARCHAR(128),
    phone VARCHAR(32),
    two_factor_enabled TINYINT UNSIGNED DEFAULT 0,
    last_password_change INT UNSIGNED,

    login_attempt_count INT UNSIGNED DEFAULT 0,
    last_failed_login INT UNSIGNED,
    last_failed_ip VARCHAR(64),

    -- Statistics
    total_login_count INT UNSIGNED DEFAULT 0,
    total_play_time INT UNSIGNED DEFAULT 0 COMMENT 'Total play time in seconds',
    character_count TINYINT UNSIGNED DEFAULT 0,
    deleted_char_count TINYINT UNSIGNED DEFAULT 0,

    total_gold_earned BIGINT UNSIGNED DEFAULT 0,
    total_gold_spent BIGINT UNSIGNED DEFAULT 0,

    dungeon_cleared INT UNSIGNED DEFAULT 0,
    pvp_matches INT UNSIGNED DEFAULT 0,
    pvp_wins INT UNSIGNED DEFAULT 0,
    pvp_losses INT UNSIGNED DEFAULT 0,

    quests_completed INT UNSIGNED DEFAULT 0,
    achievements_unlocked INT UNSIGNED DEFAULT 0,

    -- Warehouse
    warehouse_id INT UNSIGNED DEFAULT 0,
    warehouse_slots_unlocked TINYINT UNSIGNED DEFAULT 0,
    warehouse_gold BIGINT UNSIGNED DEFAULT 0,

    -- Special Flags
    is_gm TINYINT UNSIGNED DEFAULT 0,
    gm_level TINYINT UNSIGNED DEFAULT 0,
    is_return_user TINYINT UNSIGNED DEFAULT 0,
    return_user_expire INT UNSIGNED DEFAULT 0,
    is_first_login TINYINT UNSIGNED DEFAULT 1,
    tutorial_completed TINYINT UNSIGNED DEFAULT 0,

    -- PC Room (网吧)
    pc_room_no INT UNSIGNED DEFAULT 0,
    pc_room_benefits_flags TINYINT UNSIGNED DEFAULT 0,

    -- School/Organization
    school_no INT UNSIGNED DEFAULT 0,
    school_name VARCHAR(128),

    -- Economic Controls
    gold_trade_restricted TINYINT UNSIGNED DEFAULT 0,
    gold_trade_restrict_expire INT UNSIGNED DEFAULT 0,
    item_trade_restricted TINYINT UNSIGNED DEFAULT 0,
    item_trade_restrict_expire INT UNSIGNED DEFAULT 0,

    -- Anti-Cheat
    hack_detection_flags INT UNSIGNED DEFAULT 0,
    last_hack_detection_time INT UNSIGNED DEFAULT 0,
    is_blacklisted TINYINT UNSIGNED DEFAULT 0,

    -- Timestamps
    created_at INT UNSIGNED NOT NULL,
    updated_at INT UNSIGNED NOT NULL,

    -- Indexes
    INDEX idx_account_name (account_name),
    INDEX idx_email (email),
    INDEX idx_status (status),
    INDEX idx_last_login (last_login_date),
    INDEX idx_is_online (is_currently_online),
    INDEX idx_vip_level (vip_level),
    INDEX idx_is_gm (is_gm)

) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci
COMMENT='User account information';

-- ============================================================
-- Table: characters
-- Purpose: Store character information
-- ============================================================

CREATE TABLE IF NOT EXISTS characters (
    -- Primary Key
    character_id INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,

    -- Account Reference
    account_id INT UNSIGNED NOT NULL,

    -- Basic Information
    character_name VARCHAR(64) UNIQUE NOT NULL,
    status TINYINT UNSIGNED DEFAULT 0 COMMENT '0=Active, 1=PendingDelete, 2=Deleted',

    -- Job and Level
    job_class TINYINT UNSIGNED NOT NULL,
    job_grow_type TINYINT UNSIGNED NOT NULL,
    level SMALLINT UNSIGNED DEFAULT 1,
    experience BIGINT UNSIGNED DEFAULT 0,
    next_level_exp BIGINT UNSIGNED DEFAULT 1000,

    sp_points INT DEFAULT 0,
    tp_points INT DEFAULT 0,
    stat_points SMALLINT UNSIGNED DEFAULT 0,

    -- Appearance
    gender TINYINT UNSIGNED DEFAULT 0 COMMENT '0=Male, 1=Female',
    skin_color TINYINT UNSIGNED DEFAULT 0,
    hair_style TINYINT UNSIGNED DEFAULT 0,
    hair_color TINYINT UNSIGNED DEFAULT 0,
    face_style TINYINT UNSIGNED DEFAULT 0,
    eye_color TINYINT UNSIGNED DEFAULT 0,
    body_type TINYINT UNSIGNED DEFAULT 0,
    voice_type TINYINT UNSIGNED DEFAULT 0,

    -- Position
    current_zone INT DEFAULT 0,
    current_map INT DEFAULT 0,
    position_x FLOAT DEFAULT 0,
    position_y FLOAT DEFAULT 0,
    position_z FLOAT DEFAULT 0,
    facing_angle FLOAT DEFAULT 0,

    -- Base Attributes
    strength SMALLINT UNSIGNED DEFAULT 10,
    intelligence SMALLINT UNSIGNED DEFAULT 10,
    vitality SMALLINT UNSIGNED DEFAULT 10,
    spirit SMALLINT UNSIGNED DEFAULT 10,

    -- Combat Attributes (Calculated)
    max_hp INT DEFAULT 100,
    current_hp INT DEFAULT 100,
    max_mp INT DEFAULT 100,
    current_mp INT DEFAULT 100,

    physical_attack INT DEFAULT 10,
    magical_attack INT DEFAULT 10,
    physical_defense INT DEFAULT 5,
    magical_defense INT DEFAULT 5,

    attack_speed INT DEFAULT 100,
    move_speed INT DEFAULT 100,
    cast_speed INT DEFAULT 100,

    critical_rate FLOAT DEFAULT 0.05,
    critical_damage FLOAT DEFAULT 1.5,

    hp_regen INT DEFAULT 1,
    mp_regen INT DEFAULT 1,

    -- Currency
    gold BIGINT UNSIGNED DEFAULT 0,
    silver BIGINT UNSIGNED DEFAULT 0,
    cera_point INT UNSIGNED DEFAULT 0,
    used_cera INT UNSIGNED DEFAULT 0,
    mileage INT DEFAULT 0,
    win_point INT DEFAULT 0,

    -- Inventory
    inventory_id INT UNSIGNED DEFAULT 0,
    inventory_slots_total TINYINT UNSIGNED DEFAULT 27,
    inventory_slots_used TINYINT UNSIGNED DEFAULT 0,

    -- Equipment Slots
    equipment_weapon INT UNSIGNED DEFAULT 0,
    equipment_top INT UNSIGNED DEFAULT 0,
    equipment_bottom INT UNSIGNED DEFAULT 0,
    equipment_head INT UNSIGNED DEFAULT 0,
    equipment_shoulder INT UNSIGNED DEFAULT 0,
    equipment_belt INT UNSIGNED DEFAULT 0,
    equipment_shoes INT UNSIGNED DEFAULT 0,
    equipment_bracelet INT UNSIGNED DEFAULT 0,
    equipment_necklace INT UNSIGNED DEFAULT 0,
    equipment_ring INT UNSIGNED DEFAULT 0,
    equipment_support INT UNSIGNED DEFAULT 0,
    equipment_magic_stone INT UNSIGNED DEFAULT 0,

    -- Guild
    guild_id INT UNSIGNED DEFAULT 0,
    guild_rank TINYINT UNSIGNED DEFAULT 0,
    guild_contribution INT UNSIGNED DEFAULT 0,

    -- Party
    party_index SMALLINT DEFAULT -1,
    party_position TINYINT UNSIGNED DEFAULT 0,

    -- Dungeon
    dungeon_clear_count INT UNSIGNED DEFAULT 0,
    fatigue_value SMALLINT UNSIGNED DEFAULT 156,
    fatigue_max SMALLINT UNSIGNED DEFAULT 156,
    last_fatigue_reset INT UNSIGNED DEFAULT 0,

    -- Quest
    quests_completed_count SMALLINT UNSIGNED DEFAULT 0,
    quests_active_count SMALLINT UNSIGNED DEFAULT 0,

    -- PVP
    pvp_ranking INT DEFAULT 0,
    pvp_points INT DEFAULT 0,
    pvp_win_count SMALLINT UNSIGNED DEFAULT 0,
    pvp_lose_count SMALLINT UNSIGNED DEFAULT 0,
    pvp_draw_count SMALLINT UNSIGNED DEFAULT 0,
    pvp_win_rate FLOAT DEFAULT 0,

    -- Achievement
    achievements_unlocked_count SMALLINT UNSIGNED DEFAULT 0,

    -- Title and Badge
    current_title_id SMALLINT UNSIGNED DEFAULT 0,
    title_count SMALLINT UNSIGNED DEFAULT 0,
    badge_0 TINYINT UNSIGNED DEFAULT 0,
    badge_1 TINYINT UNSIGNED DEFAULT 0,
    badge_2 TINYINT UNSIGNED DEFAULT 0,
    badge_3 TINYINT UNSIGNED DEFAULT 0,
    badge_4 TINYINT UNSIGNED DEFAULT 0,
    badge_5 TINYINT UNSIGNED DEFAULT 0,
    badge_6 TINYINT UNSIGNED DEFAULT 0,
    badge_7 TINYINT UNSIGNED DEFAULT 0,
    badge_8 TINYINT UNSIGNED DEFAULT 0,
    badge_9 TINYINT UNSIGNED DEFAULT 0,

    -- Statistics
    total_play_time INT UNSIGNED DEFAULT 0,
    total_kills INT UNSIGNED DEFAULT 0,
    total_deaths INT UNSIGNED DEFAULT 0,
    total_damage_dealt BIGINT UNSIGNED DEFAULT 0,
    total_damage_received BIGINT UNSIGNED DEFAULT 0,

    -- Special Flags
    is_awakened TINYINT UNSIGNED DEFAULT 0,
    awakening_level TINYINT UNSIGNED DEFAULT 0,
    is_in_combat TINYINT UNSIGNED DEFAULT 0,
    is_in_dungeon TINYINT UNSIGNED DEFAULT 0,
    is_in_pvp TINYINT UNSIGNED DEFAULT 0,
    is_trading TINYINT UNSIGNED DEFAULT 0,
    trade_partner_id INT UNSIGNED DEFAULT 0,
    can_trade TINYINT UNSIGNED DEFAULT 1,
    can_drop TINYINT UNSIGNED DEFAULT 1,

    -- Revival
    revival_time INT UNSIGNED DEFAULT 0,
    revival_count TINYINT UNSIGNED DEFAULT 0,

    -- Timestamps
    create_date INT UNSIGNED NOT NULL,
    last_login INT UNSIGNED,
    last_logout INT UNSIGNED,
    delete_date INT UNSIGNED DEFAULT 0,
    delete_expire INT UNSIGNED DEFAULT 0,

    created_at INT UNSIGNED NOT NULL,
    updated_at INT UNSIGNED NOT NULL,

    -- Foreign Key
    FOREIGN KEY (account_id) REFERENCES user_accounts(account_id) ON DELETE CASCADE,

    -- Indexes
    INDEX idx_account_id (account_id),
    INDEX idx_character_name (character_name),
    INDEX idx_status (status),
    INDEX idx_level (level),
    INDEX idx_job_class (job_class),
    INDEX idx_guild_id (guild_id),
    INDEX idx_delete_date (delete_date),
    INDEX idx_last_login (last_login)

) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci
COMMENT='Character information';

-- ============================================================
-- Table: user_sessions
-- Purpose: Store active user sessions (online users)
-- Note: Using MEMORY engine for better performance
-- ============================================================

CREATE TABLE IF NOT EXISTS user_sessions (
    -- Primary Key
    session_id INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,

    -- Account Reference
    account_id INT UNSIGNED NOT NULL,
    user_id INT UNSIGNED NOT NULL,
    username VARCHAR(64) NOT NULL,

    -- Authentication
    is_authenticated TINYINT UNSIGNED DEFAULT 0,
    session_state TINYINT UNSIGNED DEFAULT 0 COMMENT '0=Connecting, 1=Authenticated, 2=Lobby, 3=InGame, 4=InDungeon, 5=InPVP, 6=InTrade, 7=Disconnecting',

    -- Active Character
    active_character_id INT UNSIGNED DEFAULT 0,
    active_character_index TINYINT DEFAULT -1,

    -- Network
    socket_fd INT DEFAULT -1,
    client_ip VARCHAR(64),
    client_port SMALLINT UNSIGNED,

    -- Timing
    login_time INT UNSIGNED NOT NULL,
    last_activity INT UNSIGNED NOT NULL,
    last_heartbeat INT UNSIGNED NOT NULL,
    session_duration INT UNSIGNED DEFAULT 0,

    -- Permission
    user_type TINYINT UNSIGNED DEFAULT 1,
    is_gm TINYINT UNSIGNED DEFAULT 0,
    gm_level TINYINT UNSIGNED DEFAULT 0,

    -- Client
    client_version INT UNSIGNED DEFAULT 0,
    client_hash VARCHAR(64),

    -- Anti-Cheat
    packet_sequence INT UNSIGNED DEFAULT 0,
    invalid_packet_count INT UNSIGNED DEFAULT 0,
    suspicious_action_count INT UNSIGNED DEFAULT 0,
    speed_hack_detected TINYINT UNSIGNED DEFAULT 0,
    teleport_hack_detected TINYINT UNSIGNED DEFAULT 0,
    duplicate_login_detected TINYINT UNSIGNED DEFAULT 0,

    -- Statistics
    packets_sent INT UNSIGNED DEFAULT 0,
    packets_received INT UNSIGNED DEFAULT 0,
    bytes_sent BIGINT UNSIGNED DEFAULT 0,
    bytes_received BIGINT UNSIGNED DEFAULT 0,

    -- Flags
    is_in_transaction TINYINT UNSIGNED DEFAULT 0,
    pending_disconnect TINYINT UNSIGNED DEFAULT 0,
    force_disconnect TINYINT UNSIGNED DEFAULT 0,

    -- Metadata
    session_token VARCHAR(128),
    login_ticket INT UNSIGNED,

    -- PC Room
    pc_room_no INT UNSIGNED DEFAULT 0,

    -- Indexes
    INDEX idx_account_id (account_id),
    INDEX idx_user_id (user_id),
    INDEX idx_session_state (session_state),
    INDEX idx_last_activity (last_activity),
    INDEX idx_last_heartbeat (last_heartbeat)

) ENGINE=MEMORY
COMMENT='Active user sessions (in-memory for performance)';

-- ============================================================
-- Stored Procedures
-- ============================================================

DELIMITER //

-- Create Account
CREATE PROCEDURE sp_create_account(
    IN p_account_name VARCHAR(64),
    IN p_password_hash VARCHAR(128),
    IN p_email VARCHAR(128),
    IN p_register_ip VARCHAR(64),
    OUT p_account_id INT UNSIGNED
)
BEGIN
    DECLARE v_timestamp INT UNSIGNED;
    SET v_timestamp = UNIX_TIMESTAMP();

    INSERT INTO user_accounts (
        account_name, password_hash, email, register_ip,
        register_date, status, user_type,
        created_at, updated_at
    ) VALUES (
        p_account_name, p_password_hash, p_email, p_register_ip,
        v_timestamp, 0, 1,
        v_timestamp, v_timestamp
    );

    SET p_account_id = LAST_INSERT_ID();
END //

-- Authenticate Account
CREATE PROCEDURE sp_authenticate_account(
    IN p_account_name VARCHAR(64),
    IN p_password_hash VARCHAR(128),
    IN p_client_ip VARCHAR(64),
    OUT p_result INT,
    OUT p_account_id INT UNSIGNED
)
BEGIN
    DECLARE v_status TINYINT;
    DECLARE v_is_blocked TINYINT;
    DECLARE v_block_expire INT UNSIGNED;
    DECLARE v_timestamp INT UNSIGNED;

    SET v_timestamp = UNIX_TIMESTAMP();
    SET p_result = -1;
    SET p_account_id = 0;

    -- Check if account exists
    SELECT account_id, status, is_blocked, block_expire_time
    INTO p_account_id, v_status, v_is_blocked, v_block_expire
    FROM user_accounts
    WHERE account_name = p_account_name AND password_hash = p_password_hash;

    IF p_account_id IS NULL THEN
        -- Invalid credentials
        SET p_result = 1;
    ELSEIF v_status = 1 OR v_status = 2 THEN
        -- Account blocked or banned
        SET p_result = 2;
    ELSEIF v_is_blocked = 1 AND (v_block_expire = 0 OR v_block_expire > v_timestamp) THEN
        -- Account currently blocked
        SET p_result = 3;
    ELSE
        -- Success
        SET p_result = 0;

        -- Update login information
        UPDATE user_accounts SET
            last_login_date = v_timestamp,
            last_login_ip = p_client_ip,
            current_login_time = v_timestamp,
            is_currently_online = 1,
            total_login_count = total_login_count + 1,
            is_first_login = 0,
            updated_at = v_timestamp
        WHERE account_id = p_account_id;
    END IF;
END //

-- Create Character
CREATE PROCEDURE sp_create_character(
    IN p_account_id INT UNSIGNED,
    IN p_character_name VARCHAR(64),
    IN p_job_class TINYINT UNSIGNED,
    IN p_gender TINYINT UNSIGNED,
    OUT p_result INT,
    OUT p_character_id INT UNSIGNED
)
BEGIN
    DECLARE v_char_count TINYINT;
    DECLARE v_timestamp INT UNSIGNED;

    SET v_timestamp = UNIX_TIMESTAMP();
    SET p_result = -1;
    SET p_character_id = 0;

    -- Check character count
    SELECT COUNT(*) INTO v_char_count
    FROM characters
    WHERE account_id = p_account_id AND status = 0;

    IF v_char_count >= 4 THEN
        -- Too many characters
        SET p_result = 1;
    ELSE
        -- Create character
        INSERT INTO characters (
            account_id, character_name, status,
            job_class, job_grow_type, level, experience,
            gender,
            create_date, created_at, updated_at
        ) VALUES (
            p_account_id, p_character_name, 0,
            p_job_class, 0, 1, 0,
            p_gender,
            v_timestamp, v_timestamp, v_timestamp
        );

        SET p_character_id = LAST_INSERT_ID();
        SET p_result = 0;

        -- Update account character count
        UPDATE user_accounts SET
            character_count = character_count + 1,
            updated_at = v_timestamp
        WHERE account_id = p_account_id;
    END IF;
END //

DELIMITER ;

-- ============================================================
-- Initial Data
-- ============================================================

-- Create test admin account
-- Password: admin123 (SHA-256 hash)
INSERT INTO user_accounts (
    account_name, password_hash, status, user_type,
    is_gm, gm_level,
    register_date, created_at, updated_at
) VALUES (
    'admin',
    '240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9', -- admin123
    0, 11,
    1, 10,
    UNIX_TIMESTAMP(), UNIX_TIMESTAMP(), UNIX_TIMESTAMP()
) ON DUPLICATE KEY UPDATE account_id=account_id;

-- Create test player account
-- Password: test123 (SHA-256 hash)
INSERT INTO user_accounts (
    account_name, password_hash, status, user_type,
    register_date, created_at, updated_at
) VALUES (
    'testuser',
    '9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08', -- test123
    0, 1,
    UNIX_TIMESTAMP(), UNIX_TIMESTAMP(), UNIX_TIMESTAMP()
) ON DUPLICATE KEY UPDATE account_id=account_id;

-- ============================================================
-- End of Schema
-- ============================================================
