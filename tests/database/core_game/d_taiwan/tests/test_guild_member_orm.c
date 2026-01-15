/**
 * DNF Game Server - GuildMember ORM Test
 *
 * Test suite for guild_member ORM functions
 * Database: test_d_guild
 * Table: guild_member (24 fields, composite key)
 */

#include "guild_member_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ANSI Color Codes */
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_CYAN    "\x1b[36m"

/* Print Functions */
static void print_header(const char* text) {
    printf("\n" COLOR_CYAN "================================================================================\n");
    printf("%s\n", text);
    printf("================================================================================" COLOR_RESET "\n");
}

static void print_success(const char* text) {
    printf(COLOR_GREEN "✓ %s" COLOR_RESET "\n", text);
}

static void print_error(const char* text) {
    printf(COLOR_RED "✗ %s" COLOR_RESET "\n", text);
}

static void print_info(const char* text) {
    printf(COLOR_BLUE "● %s" COLOR_RESET "\n", text);
}

/* Test Functions */
int test_create_members(DBConnectionManager* manager);
int test_query_members(DBConnectionManager* manager);
int test_update_member(DBConnectionManager* manager);
int test_existence_check(DBConnectionManager* manager);
int test_get_by_guild(DBConnectionManager* manager);
int test_get_by_charac_no(DBConnectionManager* manager);
int test_get_by_name(DBConnectionManager* manager);
int test_update_grade(DBConnectionManager* manager);
int test_update_memo(DBConnectionManager* manager);
int test_update_flag(DBConnectionManager* manager);
int test_update_visit(DBConnectionManager* manager);
int test_bbs_count(DBConnectionManager* manager);
int test_update_points(DBConnectionManager* manager);
int test_statistics(DBConnectionManager* manager);
int test_active_members(DBConnectionManager* manager);
int test_grade_filter(DBConnectionManager* manager);
int test_top_points(DBConnectionManager* manager);
int test_delete_member(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    const char* config_file;
    int passed = 0;
    int total = 18;

    printf(COLOR_CYAN "================================================================================\n");
    printf("  DNF Game Server - GuildMember ORM Test  \n");
    printf("================================================================================" COLOR_RESET "\n\n");

    if (argc < 2) {
        printf(COLOR_RED "Usage: %s <config_file>" COLOR_RESET "\n", argv[0]);
        return 1;
    }

    config_file = argv[1];
    printf("Config file: %s\n\n", config_file);

    /* Initialize database connection */
    print_info("Initializing database connection...");
    if (DBConnectionManager_Initialize(&manager, config_file) != 0) {
        print_error("Failed to initialize database connection");
        return 1;
    }
    print_success("Initialization successful");

    print_info("Connecting to guild database (test_d_guild)...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) != 0) {
        print_error("Failed to connect to guild database");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }
    print_success("Connection successful");

    /* Run tests */
    passed += test_create_members(&manager);
    passed += test_query_members(&manager);
    passed += test_update_member(&manager);
    passed += test_existence_check(&manager);
    passed += test_get_by_guild(&manager);
    passed += test_get_by_charac_no(&manager);
    passed += test_get_by_name(&manager);
    passed += test_update_grade(&manager);
    passed += test_update_memo(&manager);
    passed += test_update_flag(&manager);
    passed += test_update_visit(&manager);
    passed += test_bbs_count(&manager);
    passed += test_update_points(&manager);
    passed += test_statistics(&manager);
    passed += test_active_members(&manager);
    passed += test_grade_filter(&manager);
    passed += test_top_points(&manager);
    passed += test_delete_member(&manager);

    /* Cleanup */
    print_info("Cleaning up database connection...");
    DBConnectionManager_Cleanup(&manager);
    print_success("Cleanup complete");

    /* Print results */
    print_header("Test Results");
    if (passed == total) {
        printf(COLOR_GREEN "Test Result: %d/%d Passed" COLOR_RESET "\n", passed, total);
    } else {
        printf(COLOR_YELLOW "Test Result: %d/%d Passed" COLOR_RESET "\n", passed, total);
    }
    print_header("");

    return (passed == total) ? 0 : 1;
}

/* Test 1: Create Guild Members */
int test_create_members(DBConnectionManager* manager) {
    GuildMember member1, member2, member3;
    int success_count = 0;

    print_header("Test 1: Create Guild Members");

    /* Member 1 - Guild Leader */
    print_info("Creating member 1 (Guild Leader)");
    GuildMember_Init(&member1);
    member1.guild_id = 1001;
    member1.m_id = 100;
    member1.server_id = 1;
    member1.charac_no = 1;
    strcpy(member1.charac_name, "GuildMaster");
    strcpy(member1.nick_name, "GM_Nick");
    strcpy(member1.memo, "Guild Leader");
    member1.grade = 1;  /* Leader */
    member1.job = 0;    /* Slayer */
    member1.grow_type = 0;
    member1.lev = 95;
    member1.age = 25;
    strcpy(member1.born_year, "90");
    strcpy(member1.sex, "M");
    strcpy(member1.apply_time, "2024-01-01 10:00:00");
    strcpy(member1.member_time, "2024-01-01 10:00:00");
    member1.member_flag = 1;  /* Active */
    member1.bbs_cnt = 50;
    strcpy(member1.last_visit_time, "2024-01-15 10:00:00");
    member1.secede_type = 0;
    strcpy(member1.secede_time, "0000-00-00 00:00:00");
    member1.member_point = 10000;
    member1.member_point_prev = 9500;
    strcpy(member1.last_play_time, "2024-01-15 10:00:00");

    if (GuildMember_Create(manager, &member1) == 0) {
        print_success("Member 1 created successfully");
        success_count++;
    } else {
        print_error("Failed to create member 1");
    }

    /* Member 2 - Officer */
    print_info("Creating member 2 (Officer)");
    GuildMember_Init(&member2);
    member2.guild_id = 1001;
    member2.m_id = 101;
    member2.server_id = 1;
    member2.charac_no = 2;
    strcpy(member2.charac_name, "Officer01");
    strcpy(member2.nick_name, "Off_Nick");
    strcpy(member2.memo, "Guild Officer");
    member2.grade = 2;  /* Officer */
    member2.job = 1;    /* Fighter */
    member2.grow_type = 0;
    member2.lev = 90;
    member2.age = 23;
    strcpy(member2.born_year, "92");
    strcpy(member2.sex, "F");
    strcpy(member2.apply_time, "2024-01-02 12:00:00");
    strcpy(member2.member_time, "2024-01-02 12:30:00");
    member2.member_flag = 1;  /* Active */
    member2.bbs_cnt = 30;
    strcpy(member2.last_visit_time, "2024-01-15 09:00:00");
    member2.secede_type = 0;
    strcpy(member2.secede_time, "0000-00-00 00:00:00");
    member2.member_point = 7500;
    member2.member_point_prev = 7000;
    strcpy(member2.last_play_time, "2024-01-15 09:00:00");

    if (GuildMember_Create(manager, &member2) == 0) {
        print_success("Member 2 created successfully");
        success_count++;
    } else {
        print_error("Failed to create member 2");
    }

    /* Member 3 - Regular Member */
    print_info("Creating member 3 (Regular Member)");
    GuildMember_Init(&member3);
    member3.guild_id = 1001;
    member3.m_id = 102;
    member3.server_id = 1;
    member3.charac_no = 3;
    strcpy(member3.charac_name, "RegularMember");
    strcpy(member3.nick_name, "Reg_Nick");
    strcpy(member3.memo, "New member");
    member3.grade = 9;  /* Regular */
    member3.job = 2;    /* Gunner */
    member3.grow_type = 0;
    member3.lev = 85;
    member3.age = 20;
    strcpy(member3.born_year, "95");
    strcpy(member3.sex, "M");
    strcpy(member3.apply_time, "2024-01-05 14:00:00");
    strcpy(member3.member_time, "2024-01-05 15:00:00");
    member3.member_flag = 1;  /* Active */
    member3.bbs_cnt = 10;
    strcpy(member3.last_visit_time, "2024-01-14 20:00:00");
    member3.secede_type = 0;
    strcpy(member3.secede_time, "0000-00-00 00:00:00");
    member3.member_point = 5000;
    member3.member_point_prev = 4500;
    strcpy(member3.last_play_time, "2024-01-14 20:00:00");

    if (GuildMember_Create(manager, &member3) == 0) {
        print_success("Member 3 created successfully");
        success_count++;
    } else {
        print_error("Failed to create member 3");
    }

    return (success_count == 3) ? 1 : 0;
}

/* Test 2: Query Guild Members */
int test_query_members(DBConnectionManager* manager) {
    GuildMember member;

    print_header("Test 2: Query Guild Members");

    /* Query member 1 */
    print_info("Query member with guild_id=1001, charac_no=1");
    if (GuildMember_GetByKey(manager, 1001, 1, &member) == 0) {
        print_success("Query successful");
        printf("  Name: %s\n", member.charac_name);
        printf("  Grade: %d\n", (int)member.grade);
        printf("  Level: %d\n", (int)member.lev);
        printf("  Points: %u\n", member.member_point);
    } else {
        print_error("Query failed");
        return 0;
    }

    /* Query member 2 */
    print_info("Query member with guild_id=1001, charac_no=2");
    if (GuildMember_GetByKey(manager, 1001, 2, &member) == 0) {
        print_success("Query successful");
        printf("  Name: %s\n", member.charac_name);
        printf("  Grade: %d\n", (int)member.grade);
    } else {
        print_error("Query failed");
        return 0;
    }

    return 1;
}

/* Test 3: Update Member Information */
int test_update_member(DBConnectionManager* manager) {
    GuildMember member;

    print_header("Test 3: Update Member Information");

    print_info("Getting original member data (guild_id=1001, charac_no=1)");
    if (GuildMember_GetByKey(manager, 1001, 1, &member) != 0) {
        print_error("Failed to get member");
        return 0;
    }
    printf("  Original level: %d\n", (int)member.lev);

    /* Update level */
    print_info("Updating member level to 96");
    member.lev = 96;
    if (GuildMember_Update(manager, &member) == 0) {
        print_success("Update successful");

        /* Verify */
        GuildMember_Init(&member);
        if (GuildMember_GetByKey(manager, 1001, 1, &member) == 0) {
            if (member.lev == 96) {
                print_success("Update verification passed");
                printf("  New level: %d\n", (int)member.lev);
                return 1;
            }
        }
    }

    print_error("Update failed");
    return 0;
}

/* Test 4: Existence Check */
int test_existence_check(DBConnectionManager* manager) {
    print_header("Test 4: Existence Check");

    print_info("Checking if member exists (guild_id=1001, charac_no=1)");
    if (GuildMember_Exists(manager, 1001, 1)) {
        print_success("Member exists");
    } else {
        print_error("Member should exist");
        return 0;
    }

    print_info("Checking if non-existent member exists (guild_id=1001, charac_no=999)");
    if (!GuildMember_Exists(manager, 1001, 999)) {
        print_success("Non-existent member check passed");
        return 1;
    }

    print_error("Non-existent member check failed");
    return 0;
}

/* Test 5: Get All Guild Members */
int test_get_by_guild(DBConnectionManager* manager) {
    GuildMember members[10];
    int count;
    int i;

    print_header("Test 5: Get All Guild Members");

    print_info("Getting all members for guild_id=1001");
    count = GuildMember_GetByGuildID(manager, 1001, members, 10);
    if (count >= 0) {
        print_success("Guild members query successful");
        printf("  Total members: %d\n", count);
        for (i = 0; i < count; i++) {
            printf("    %d. %s (charac_no=%d, grade=%d)\n",
                   i + 1, members[i].charac_name, members[i].charac_no, (int)members[i].grade);
        }
        return (count == 3) ? 1 : 0;
    }

    print_error("Guild members query failed");
    return 0;
}

/* Test 6: Get Member by Character Number */
int test_get_by_charac_no(DBConnectionManager* manager) {
    GuildMember member;

    print_header("Test 6: Get Member by Character Number");

    print_info("Getting member with charac_no=2");
    if (GuildMember_GetByCharacNo(manager, 2, &member) == 0) {
        print_success("Query by charac_no successful");
        printf("  Name: %s\n", member.charac_name);
        printf("  Guild ID: %d\n", member.guild_id);
        return 1;
    }

    print_error("Query by charac_no failed");
    return 0;
}

/* Test 7: Get Member by Character Name */
int test_get_by_name(DBConnectionManager* manager) {
    GuildMember member;

    print_header("Test 7: Get Member by Character Name");

    print_info("Getting member with name 'Officer01'");
    if (GuildMember_GetByCharacName(manager, "Officer01", &member) == 0) {
        print_success("Query by name successful");
        printf("  Character No: %d\n", member.charac_no);
        printf("  Grade: %d\n", (int)member.grade);
        return 1;
    }

    print_error("Query by name failed");
    return 0;
}

/* Test 8: Update Member Grade */
int test_update_grade(DBConnectionManager* manager) {
    GuildMember member;

    print_header("Test 8: Update Member Grade");

    print_info("Updating grade for member (guild_id=1001, charac_no=3) to 5");
    if (GuildMember_UpdateGrade(manager, 1001, 3, 5) == 0) {
        print_success("Grade update successful");

        /* Verify */
        if (GuildMember_GetByKey(manager, 1001, 3, &member) == 0) {
            if (member.grade == 5) {
                print_success("Grade verification passed");
                printf("  New grade: %d\n", (int)member.grade);
                return 1;
            }
        }
    }

    print_error("Grade update failed");
    return 0;
}

/* Test 9: Update Member Memo */
int test_update_memo(DBConnectionManager* manager) {
    GuildMember member;

    print_header("Test 9: Update Member Memo");

    print_info("Updating memo for member (guild_id=1001, charac_no=2)");
    if (GuildMember_UpdateMemo(manager, 1001, 2, "Promoted to officer") == 0) {
        print_success("Memo update successful");

        /* Verify */
        if (GuildMember_GetByKey(manager, 1001, 2, &member) == 0) {
            if (strcmp(member.memo, "Promoted to officer") == 0) {
                print_success("Memo verification passed");
                printf("  New memo: %s\n", member.memo);
                return 1;
            }
        }
    }

    print_error("Memo update failed");
    return 0;
}

/* Test 10: Update Member Flag */
int test_update_flag(DBConnectionManager* manager) {
    GuildMember member;

    print_header("Test 10: Update Member Flag");

    print_info("Updating flag for member (guild_id=1001, charac_no=3) to 2");
    if (GuildMember_UpdateFlag(manager, 1001, 3, 2) == 0) {
        print_success("Flag update successful");

        /* Verify */
        if (GuildMember_GetByKey(manager, 1001, 3, &member) == 0) {
            if (member.member_flag == 2) {
                print_success("Flag verification passed");
                printf("  New flag: %d\n", (int)member.member_flag);
                return 1;
            }
        }
    }

    print_error("Flag update failed");
    return 0;
}

/* Test 11: Update Last Visit Time */
int test_update_visit(DBConnectionManager* manager) {
    print_header("Test 11: Update Last Visit Time");

    print_info("Updating last visit time for member (guild_id=1001, charac_no=1)");
    if (GuildMember_UpdateLastVisit(manager, 1001, 1) == 0) {
        print_success("Last visit update successful");
        return 1;
    }

    print_error("Last visit update failed");
    return 0;
}

/* Test 12: Increment BBS Count */
int test_bbs_count(DBConnectionManager* manager) {
    GuildMember member;
    unsigned short original_count;

    print_header("Test 12: Increment BBS Count");

    /* Get original count */
    if (GuildMember_GetByKey(manager, 1001, 2, &member) != 0) {
        print_error("Failed to get member");
        return 0;
    }
    original_count = member.bbs_cnt;
    printf("  Original BBS count: %u\n", original_count);

    print_info("Incrementing BBS count for member (guild_id=1001, charac_no=2)");
    if (GuildMember_IncrementBBSCount(manager, 1001, 2) == 0) {
        print_success("BBS count increment successful");

        /* Verify */
        GuildMember_Init(&member);
        if (GuildMember_GetByKey(manager, 1001, 2, &member) == 0) {
            if (member.bbs_cnt == original_count + 1) {
                print_success("BBS count verification passed");
                printf("  New BBS count: %u\n", member.bbs_cnt);
                return 1;
            }
        }
    }

    print_error("BBS count increment failed");
    return 0;
}

/* Test 13: Update Member Points */
int test_update_points(DBConnectionManager* manager) {
    GuildMember member;

    print_header("Test 13: Update Member Points");

    /* Get original points */
    if (GuildMember_GetByKey(manager, 1001, 1, &member) != 0) {
        print_error("Failed to get member");
        return 0;
    }
    printf("  Original points: %u\n", member.member_point);

    print_info("Updating points for member (guild_id=1001, charac_no=1) to 12000");
    if (GuildMember_UpdatePoints(manager, 1001, 1, 12000) == 0) {
        print_success("Points update successful");

        /* Verify */
        GuildMember_Init(&member);
        if (GuildMember_GetByKey(manager, 1001, 1, &member) == 0) {
            if (member.member_point == 12000 && member.member_point_prev == 10000) {
                print_success("Points verification passed");
                printf("  New points: %u\n", member.member_point);
                printf("  Previous points: %u\n", member.member_point_prev);
                return 1;
            }
        }
    }

    print_error("Points update failed");
    return 0;
}

/* Test 14: Statistics */
int test_statistics(DBConnectionManager* manager) {
    int count;

    print_header("Test 14: Statistics");

    print_info("Getting total member count for guild_id=1001");
    count = GuildMember_GetCount(manager, 1001);
    if (count >= 0) {
        print_success("Member count query successful");
        printf("  Total members: %d\n", count);
        return (count == 3) ? 1 : 0;
    }

    print_error("Member count query failed");
    return 0;
}

/* Test 15: Get Active Members */
int test_active_members(DBConnectionManager* manager) {
    GuildMember members[10];
    int count;
    int i;

    print_header("Test 15: Get Active Members");

    print_info("Getting active members for guild_id=1001");
    count = GuildMember_GetActiveMembers(manager, 1001, members, 10);
    if (count >= 0) {
        print_success("Active members query successful");
        printf("  Active members: %d\n", count);
        for (i = 0; i < count; i++) {
            printf("    %d. %s (flag=%d)\n",
                   i + 1, members[i].charac_name, (int)members[i].member_flag);
        }
        return 1;
    }

    print_error("Active members query failed");
    return 0;
}

/* Test 16: Get Members by Grade */
int test_grade_filter(DBConnectionManager* manager) {
    GuildMember members[10];
    int count;
    int i;

    print_header("Test 16: Get Members by Grade");

    print_info("Getting members with grade=5 for guild_id=1001");
    count = GuildMember_GetByGrade(manager, 1001, 5, members, 10);
    if (count >= 0) {
        print_success("Grade filter query successful");
        printf("  Members with grade 5: %d\n", count);
        for (i = 0; i < count; i++) {
            printf("    %d. %s (grade=%d)\n",
                   i + 1, members[i].charac_name, (int)members[i].grade);
        }
        return 1;
    }

    print_error("Grade filter query failed");
    return 0;
}

/* Test 17: Get Top Members by Points */
int test_top_points(DBConnectionManager* manager) {
    GuildMember members[10];
    int count;
    int i;

    print_header("Test 17: Get Top Members by Points");

    print_info("Getting top 5 members by points for guild_id=1001");
    count = GuildMember_GetTopByPoints(manager, 1001, members, 5);
    if (count >= 0) {
        print_success("Top points query successful");
        printf("  Top members: %d\n", count);
        for (i = 0; i < count; i++) {
            printf("    %d. %s (%u points)\n",
                   i + 1, members[i].charac_name, members[i].member_point);
        }
        return 1;
    }

    print_error("Top points query failed");
    return 0;
}

/* Test 18: Delete Member */
int test_delete_member(DBConnectionManager* manager) {
    print_header("Test 18: Delete Member");

    print_info("Deleting member (guild_id=1001, charac_no=3)");
    if (GuildMember_Delete(manager, 1001, 3) == 0) {
        print_success("Member deleted successfully");

        /* Verify deletion */
        if (!GuildMember_Exists(manager, 1001, 3)) {
            print_success("Deletion verification passed");
            return 1;
        }
    }

    print_error("Delete failed");
    return 0;
}
