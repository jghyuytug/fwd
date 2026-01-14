import re

# Read the file
with open('src/database/charac_info_orm.c', 'r') as f:
    content = f.read()

# Step 1: Replace qresult.values[ with values[
content = content.replace('qresult.values[', 'values[')

# Step 2: Fix GetByCharacNo function - add values array declaration
get_by_charac_no_pattern = r'(int CharacInfo_GetByCharacNo\(DBConnectionManager\* manager, int charac_no, CharacInfo\* info\) \{\s+char query\[2048\];)\s+(DBQueryResult qresult;)'
get_by_charac_no_replacement = r'\1\n    char* values[50];\n    \2'
content = re.sub(get_by_charac_no_pattern, get_by_charac_no_replacement, content)

# Step 3: Fix GetByName function - add values array declaration  
get_by_name_pattern = r'(int CharacInfo_GetByName\(DBConnectionManager\* manager, const char\* charac_name, CharacInfo\* info\) \{\s+char query\[2048\];)\s+(DBQueryResult qresult;)'
get_by_name_replacement = r'\1\n    char* values[50];\n    \2'
content = re.sub(get_by_name_pattern, get_by_name_replacement, content)

# Step 4: Fix GetByMemberId function - add values array declaration
get_by_member_pattern = r'(int CharacInfo_GetByMemberId\(DBConnectionManager\* manager, int m_id, CharacInfo\* results, int max_count\) \{\s+char query\[2048\];)\s+(DBQueryResult qresult;)'
get_by_member_replacement = r'\1\n    char* values[50];\n    \2'
content = re.sub(get_by_member_pattern, get_by_member_replacement, content)

# Step 5: Fix error checking and add FetchRow calls
# Pattern for GetByCharacNo and GetByName
fetch_pattern1 = r'(ret = DBConnectionManager_ExecuteQuery\(manager, DB_TYPE_CAIN, query, &qresult\);)\s+(if \(ret < 0 \|\| qresult\.row_count == 0\))'
fetch_replacement1 = r'\1\n    \n    if (ret <= 0 || DBQueryResult_FetchRow(&qresult, values) <= 0)'
content = re.sub(fetch_pattern1, fetch_replacement1, content)

# Step 6: Fix GetByMemberId loop - needs different pattern
content = content.replace(
    'if (ret < 0 || qresult.row_count == 0) {',
    'if (ret <= 0) {'
)

# Step 7: Add FetchRow to GetCountByMemberId
content = content.replace(
    'count = atoi(values[0]);',
    'if (DBQueryResult_FetchRow(&qresult, values) > 0) {\n        count = atoi(values[0]);\n    } else {\n        count = -1;\n    }'
)

# Step 8: Fix Exists function
exists_pattern = r'(CharacInfo_Exists.*?ret = DBConnectionManager_ExecuteQuery.*?qresult\);)\s+(if \(ret < 0 \|\| qresult\.row_count == 0\))'
content = re.sub(exists_pattern, r'\1\n    \n    if (ret <= 0)', content, flags=re.DOTALL)

# Step 9: Add values declaration to Exists function
exists_decl_pattern = r'(int CharacInfo_Exists\(.*?\) \{\s+char query\[512\];)\s+(DBQueryResult qresult;)'
exists_decl_replacement = r'\1\n    char* values[1];\n    \2'
content = re.sub(exists_decl_pattern, exists_decl_replacement, content, flags=re.DOTALL)

# Step 10: Add FetchRow to Exists
content = re.sub(
    r'(CharacInfo_Exists.*?ret = DBConnectionManager_ExecuteQuery.*?;)\s+int count = atoi\(values\[0\]\);',
    r'\1\n    \n    if (DBQueryResult_FetchRow(&qresult, values) <= 0) {\n        DBQueryResult_Free(&qresult);\n        return 0;\n    }\n    \n    int count = atoi(values[0]);',
    content,
    flags=re.DOTALL
)

# Step 11: Add values declaration to GetCountByMemberId
get_count_decl_pattern = r'(int CharacInfo_GetCountByMemberId\(.*?\) \{\s+char query\[512\];)\s+(DBQueryResult qresult;)'
get_count_decl_replacement = r'\1\n    char* values[1];\n    \2'
content = re.sub(get_count_decl_pattern, get_count_decl_replacement, content, flags=re.DOTALL)

# Step 12: Fix GetByMemberId loop to use FetchRow
get_by_member_loop = r'for \(i = 0; i < count; i\+\+\) \{\s+int offset = i \* 50;\s+memset\(&results\[i\], 0, sizeof\(CharacInfo\)\);'
get_by_member_loop_replacement = '''while (i < max_count && DBQueryResult_FetchRow(&qresult, values) > 0) {
        memset(&results[i], 0, sizeof(CharacInfo));'''
content = re.sub(get_by_member_loop, get_by_member_loop_replacement, content)

# Step 13: Remove offset calculations in GetByMemberId
content = re.sub(r'values\[offset \+ (\d+)\]', r'values[\1]', content)

# Step 14: Update return value in GetByMemberId 
content = content.replace('return count;', 'return i;')

# Write the fixed file
with open('src/database/charac_info_orm_fixed.c', 'w') as f:
    f.write(content)

print("Fixed API calls. Output: src/database/charac_info_orm_fixed.c")
