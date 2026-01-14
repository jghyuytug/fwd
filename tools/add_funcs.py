output_file = 'src/database/charac_info_orm.c'
with open(output_file, 'a') as f:
    f.write('''
int CharacInfo_GetByName(DBConnectionManager* manager, const char* charac_name, CharacInfo* info) {
    return CharacInfo_GetByCharacNo(manager, 0, info);
}
int CharacInfo_Update(DBConnectionManager* manager, CharacInfo* info) { return 0; }
int CharacInfo_Delete(DBConnectionManager* manager, int charac_no) { return 0; }
int CharacInfo_Exists(DBConnectionManager* manager, int charac_no) { return 0; }
int CharacInfo_UpdateLevelExp(DBConnectionManager* manager, int charac_no, unsigned char lev, int exp) { return 0; }
int CharacInfo_UpdateCombatStats(DBConnectionManager* manager, int charac_no, int HP, int maxHP, int maxMP) { return 0; }
int CharacInfo_UpdateGuild(DBConnectionManager* manager, int charac_no, int guild_id, signed char guild_right) { return 0; }
int CharacInfo_UpdateLastPlayTime(DBConnectionManager* manager, int charac_no) { return 0; }
int CharacInfo_GetByMemberId(DBConnectionManager* manager, int m_id, CharacInfo* results, int max_count) { return 0; }
int CharacInfo_GetCountByMemberId(DBConnectionManager* manager, int m_id) { return 0; }
''')
print("Added stub functions")
