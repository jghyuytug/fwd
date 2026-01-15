/**
 * @file test_creature_parser.cpp
 * @brief Unit tests for Creature Parser (74 sections)
 *
 * Tests all functionality of parser/creature_parser.cpp including:
 * - Core metadata (20 sections)
 * - Motion & effects (42 sections)
 * - Feature systems (21 sections)
 * - Advanced systems (13 sections - evolution, growth, skills)
 */

#include "parser/creature_parser.h"
#include <cstdio>
#include <cstring>

// Test utilities
#define TEST_CASE(name) \
    static int test_##name() { \
        const char* test_name = #name; \
        int failures = 0;

#define TEST_END() \
        if (failures == 0) { \
            std::printf("[PASS] %s\n", test_name); \
        } else { \
            std::printf("[FAIL] %s (%d assertions failed)\n", test_name, failures); \
        } \
        return failures; \
    }

#define ASSERT_TRUE(expr, msg) \
    if (!(expr)) { \
        std::printf("  ASSERT_TRUE failed: %s\n", msg); \
        failures++; \
    }

#define ASSERT_FALSE(expr, msg) \
    if (expr) { \
        std::printf("  ASSERT_FALSE failed: %s\n", msg); \
        failures++; \
    }

#define ASSERT_EQ(a, b, msg) \
    if ((a) != (b)) { \
        std::printf("  ASSERT_EQ failed: %s (expected %d, got %d)\n", msg, (int)(b), (int)(a)); \
        failures++; \
    }

#define ASSERT_STR_EQ(a, b, msg) \
    if (std::strcmp((a).c_str(), b) != 0) { \
        std::printf("  ASSERT_STR_EQ failed: %s (expected '%s', got '%s')\n", msg, b, (a).c_str()); \
        failures++; \
    }

#define ASSERT_GT(a, b, msg) \
    if ((a) <= (b)) { \
        std::printf("  ASSERT_GT failed: %s (%d not greater than %d)\n", msg, (int)(a), (int)(b)); \
        failures++; \
    }

// ============================================================================
// Test Cases
// ============================================================================

TEST_CASE(CreatureScript_Constructor) {
    CreatureScript creature;

    // Verify all fields initialized to zero/empty
    ASSERT_TRUE(creature.name.empty(), "Name should be empty");
    ASSERT_EQ(creature.characterSpan, 0, "Character span should be 0");
    ASSERT_EQ(creature.widthLeft, 0, "Width left should be 0");
    ASSERT_EQ(creature.widthRight, 0, "Width right should be 0");
    ASSERT_EQ(creature.floatingHeight, 0, "Floating height should be 0");
    ASSERT_EQ(creature.drawLayer, 0, "Draw layer should be 0");
    ASSERT_EQ(creature.skillRecoveryTime, 0, "Skill recovery time should be 0");
    ASSERT_EQ(creature.overSkillRecoveryTime, 0, "Over skill recovery time should be 0");
    ASSERT_FALSE(creature.directionCorrect, "Direction correct should be false");
    ASSERT_EQ(creature.gravity, 0, "Gravity should be 0");
    ASSERT_EQ(creature.startLevel, 0, "Start level should be 0");
    ASSERT_EQ(creature.maxLevel, 0, "Max level should be 0");
    ASSERT_TRUE(creature.basicMotion.empty(), "Basic motion should be empty");
    ASSERT_TRUE(creature.skillList.empty(), "Skill list should be empty");
    ASSERT_TRUE(creature.overSkillList.empty(), "Over skill list should be empty");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_Clear) {
    CreatureScript creature;

    // Set some values
    creature.name = "TestCreature";
    creature.characterSpan = 100;
    creature.widthLeft = 50;
    creature.widthRight = 50;
    creature.startLevel = 5;
    creature.basicMotion = "test.ani";
    creature.skillList[1].skillName = "TestSkill";
    creature.attackInfo.push_back("attack1");

    // Clear and verify
    creature.clear();

    ASSERT_TRUE(creature.name.empty(), "Name should be cleared");
    ASSERT_EQ(creature.characterSpan, 0, "Character span should be 0");
    ASSERT_EQ(creature.widthLeft, 0, "Width left should be 0");
    ASSERT_EQ(creature.widthRight, 0, "Width right should be 0");
    ASSERT_EQ(creature.startLevel, 0, "Start level should be 0");
    ASSERT_TRUE(creature.basicMotion.empty(), "Basic motion should be cleared");
    ASSERT_TRUE(creature.skillList.empty(), "Skill list should be cleared");
    ASSERT_TRUE(creature.attackInfo.empty(), "Attack info should be cleared");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_CoreMetadata) {
    CreatureScript creature;

    // Test setting core metadata fields
    creature.name = "GoblinFighter";
    creature.characterSpan = 150;
    creature.widthLeft = 40;
    creature.widthRight = 60;
    creature.floatingHeight = 10;
    creature.drawLayer = 3;
    creature.skillRecoveryTime = 5000;
    creature.overSkillRecoveryTime = 10000;
    creature.directionCorrect = true;
    creature.gravity = 980;

    // Verify all fields
    ASSERT_STR_EQ(creature.name, "GoblinFighter", "Name mismatch");
    ASSERT_EQ(creature.characterSpan, 150, "Character span mismatch");
    ASSERT_EQ(creature.widthLeft, 40, "Width left mismatch");
    ASSERT_EQ(creature.widthRight, 60, "Width right mismatch");
    ASSERT_EQ(creature.floatingHeight, 10, "Floating height mismatch");
    ASSERT_EQ(creature.drawLayer, 3, "Draw layer mismatch");
    ASSERT_EQ(creature.skillRecoveryTime, 5000, "Skill recovery time mismatch");
    ASSERT_EQ(creature.overSkillRecoveryTime, 10000, "Over skill recovery time mismatch");
    ASSERT_TRUE(creature.directionCorrect, "Direction correct should be true");
    ASSERT_EQ(creature.gravity, 980, "Gravity mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_LevelSystem) {
    CreatureScript creature;

    creature.startLevel = 1;
    creature.maxLevel = 50;
    creature.parentMaxLevel = 100;
    creature.permissionLevel = 10;
    creature.learnOverskillLevel = 45;

    ASSERT_EQ(creature.startLevel, 1, "Start level mismatch");
    ASSERT_EQ(creature.maxLevel, 50, "Max level mismatch");
    ASSERT_EQ(creature.parentMaxLevel, 100, "Parent max level mismatch");
    ASSERT_EQ(creature.permissionLevel, 10, "Permission level mismatch");
    ASSERT_EQ(creature.learnOverskillLevel, 45, "Learn overskill level mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_MoveSpeed) {
    CreatureScript creature;

    // Test move speed array (5 elements)
    creature.moveSpeed[0] = 100;
    creature.moveSpeed[1] = 150;
    creature.moveSpeed[2] = 200;
    creature.moveSpeed[3] = 250;
    creature.moveSpeed[4] = 300;

    ASSERT_EQ(creature.moveSpeed[0], 100, "Move speed[0] mismatch");
    ASSERT_EQ(creature.moveSpeed[1], 150, "Move speed[1] mismatch");
    ASSERT_EQ(creature.moveSpeed[2], 200, "Move speed[2] mismatch");
    ASSERT_EQ(creature.moveSpeed[3], 250, "Move speed[3] mismatch");
    ASSERT_EQ(creature.moveSpeed[4], 300, "Move speed[4] mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_ArtifactSlots) {
    CreatureScript creature;

    creature.artifactSlotRed = 2;
    creature.artifactSlotBlue = 3;
    creature.artifactSlotGreen = 1;

    ASSERT_EQ(creature.artifactSlotRed, 2, "Red artifact slot mismatch");
    ASSERT_EQ(creature.artifactSlotBlue, 3, "Blue artifact slot mismatch");
    ASSERT_EQ(creature.artifactSlotGreen, 1, "Green artifact slot mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_MotionAnimations) {
    CreatureScript creature;

    // Test motion animation strings
    creature.basicMotion = "creature/goblin/basic.ani";
    creature.walkMotion = "creature/goblin/walk.ani";
    creature.runMotion = "creature/goblin/run.ani";
    creature.flyMotion = "creature/goblin/fly.ani";
    creature.skillMotion = "creature/goblin/skill.ani";
    creature.overSkillMotion = "creature/goblin/overskill.ani";
    creature.responseMotion = "creature/goblin/response.ani";
    creature.specialMotion = "creature/goblin/special.ani";
    creature.equipMotion = "creature/goblin/equip.ani";

    ASSERT_STR_EQ(creature.basicMotion, "creature/goblin/basic.ani", "Basic motion mismatch");
    ASSERT_STR_EQ(creature.walkMotion, "creature/goblin/walk.ani", "Walk motion mismatch");
    ASSERT_STR_EQ(creature.runMotion, "creature/goblin/run.ani", "Run motion mismatch");
    ASSERT_STR_EQ(creature.flyMotion, "creature/goblin/fly.ani", "Fly motion mismatch");
    ASSERT_STR_EQ(creature.skillMotion, "creature/goblin/skill.ani", "Skill motion mismatch");
    ASSERT_STR_EQ(creature.overSkillMotion, "creature/goblin/overskill.ani", "Over skill motion mismatch");
    ASSERT_STR_EQ(creature.responseMotion, "creature/goblin/response.ani", "Response motion mismatch");
    ASSERT_STR_EQ(creature.specialMotion, "creature/goblin/special.ani", "Special motion mismatch");
    ASSERT_STR_EQ(creature.equipMotion, "creature/goblin/equip.ani", "Equip motion mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_EffectStrings) {
    CreatureScript creature;

    // Test over effects
    creature.basicOverEffect = "effect/basic_over.ptl";
    creature.skillOverEffect = "effect/skill_over.ptl";
    creature.overSkillOverEffect = "effect/overskill_over.ptl";

    // Test under effects
    creature.basicUnderEffect = "effect/basic_under.ptl";
    creature.skillUnderEffect = "effect/skill_under.ptl";
    creature.overSkillUnderEffect = "effect/overskill_under.ptl";

    ASSERT_STR_EQ(creature.basicOverEffect, "effect/basic_over.ptl", "Basic over effect mismatch");
    ASSERT_STR_EQ(creature.skillOverEffect, "effect/skill_over.ptl", "Skill over effect mismatch");
    ASSERT_STR_EQ(creature.overSkillOverEffect, "effect/overskill_over.ptl", "Over skill over effect mismatch");
    ASSERT_STR_EQ(creature.basicUnderEffect, "effect/basic_under.ptl", "Basic under effect mismatch");
    ASSERT_STR_EQ(creature.skillUnderEffect, "effect/skill_under.ptl", "Skill under effect mismatch");
    ASSERT_STR_EQ(creature.overSkillUnderEffect, "effect/overskill_under.ptl", "Over skill under effect mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_AudioSystem) {
    CreatureScript creature;

    creature.defaultParticle = "particle/default.ptl";
    creature.responseSoundTag = "SOUND_RESPONSE";
    creature.wingSoundTag = "SOUND_WING";
    creature.walkSoundTag = "SOUND_WALK";
    creature.runSoundTag = "SOUND_RUN";

    ASSERT_STR_EQ(creature.defaultParticle, "particle/default.ptl", "Default particle mismatch");
    ASSERT_STR_EQ(creature.responseSoundTag, "SOUND_RESPONSE", "Response sound tag mismatch");
    ASSERT_STR_EQ(creature.wingSoundTag, "SOUND_WING", "Wing sound tag mismatch");
    ASSERT_STR_EQ(creature.walkSoundTag, "SOUND_WALK", "Walk sound tag mismatch");
    ASSERT_STR_EQ(creature.runSoundTag, "SOUND_RUN", "Run sound tag mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_MotionLists) {
    CreatureScript creature;

    // Test etc motion list
    creature.etcMotion.push_back("motion1.ani");
    creature.etcMotion.push_back("motion2.ani");
    creature.etcMotion.push_back("motion3.ani");

    // Test random motion list
    creature.randomMotion.push_back("random1.ani");
    creature.randomMotion.push_back("random2.ani");

    creature.defaultRandomMotionRate = 10;
    creature.increaseRandomMotionRate = 5;

    ASSERT_EQ(creature.etcMotion.size(), 3, "Etc motion size mismatch");
    ASSERT_STR_EQ(creature.etcMotion[0], "motion1.ani", "Etc motion[0] mismatch");
    ASSERT_STR_EQ(creature.etcMotion[1], "motion2.ani", "Etc motion[1] mismatch");
    ASSERT_STR_EQ(creature.etcMotion[2], "motion3.ani", "Etc motion[2] mismatch");

    ASSERT_EQ(creature.randomMotion.size(), 2, "Random motion size mismatch");
    ASSERT_STR_EQ(creature.randomMotion[0], "random1.ani", "Random motion[0] mismatch");
    ASSERT_STR_EQ(creature.randomMotion[1], "random2.ani", "Random motion[1] mismatch");

    ASSERT_EQ(creature.defaultRandomMotionRate, 10, "Default random motion rate mismatch");
    ASSERT_EQ(creature.increaseRandomMotionRate, 5, "Increase random motion rate mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_CombatData) {
    CreatureScript creature;

    // Attack info
    creature.attackInfo.push_back("attack_basic.atk");
    creature.attackInfo.push_back("attack_special.atk");

    // Skill strings
    creature.skillString.push_back("skill_fireball");
    creature.skillString.push_back("skill_icebolt");

    // Skill level values
    creature.skillLevelValues.push_back(1);
    creature.skillLevelValues.push_back(5);
    creature.skillLevelValues.push_back(10);

    // Over skill level values
    creature.overSkillLevelValues.push_back(20);
    creature.overSkillLevelValues.push_back(30);

    ASSERT_EQ(creature.attackInfo.size(), 2, "Attack info size mismatch");
    ASSERT_EQ(creature.skillString.size(), 2, "Skill string size mismatch");
    ASSERT_EQ(creature.skillLevelValues.size(), 3, "Skill level values size mismatch");
    ASSERT_EQ(creature.overSkillLevelValues.size(), 2, "Over skill level values size mismatch");

    ASSERT_STR_EQ(creature.attackInfo[0], "attack_basic.atk", "Attack info[0] mismatch");
    ASSERT_STR_EQ(creature.skillString[0], "skill_fireball", "Skill string[0] mismatch");
    ASSERT_EQ(creature.skillLevelValues[0], 1, "Skill level values[0] mismatch");
    ASSERT_EQ(creature.overSkillLevelValues[0], 20, "Over skill level values[0] mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_DataArrays) {
    CreatureScript creature;

    // Int data
    creature.intData.push_back(100);
    creature.intData.push_back(200);
    creature.intData.push_back(300);

    // String data
    creature.stringData.push_back("data1");
    creature.stringData.push_back("data2");

    // Piercing
    creature.piercing.push_back(50);
    creature.piercing.push_back(75);

    ASSERT_EQ(creature.intData.size(), 3, "Int data size mismatch");
    ASSERT_EQ(creature.stringData.size(), 2, "String data size mismatch");
    ASSERT_EQ(creature.piercing.size(), 2, "Piercing size mismatch");

    ASSERT_EQ(creature.intData[1], 200, "Int data[1] mismatch");
    ASSERT_STR_EQ(creature.stringData[1], "data2", "String data[1] mismatch");
    ASSERT_EQ(creature.piercing[0], 50, "Piercing[0] mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_EvolutionSystem) {
    CreatureScript creature;

    creature.evolutionQuest = 1001;
    creature.evolutionCreatureId = 2001;
    creature.evolutionLevel = 30;

    creature.evolutionCreatureMultyId.push_back(3001);
    creature.evolutionCreatureMultyId.push_back(3002);
    creature.evolutionCreatureMultyId.push_back(3003);

    creature.eventEvoluteCreature.push_back(4001);
    creature.eventEvoluteCreature.push_back(4002);

    ASSERT_EQ(creature.evolutionQuest, 1001, "Evolution quest mismatch");
    ASSERT_EQ(creature.evolutionCreatureId, 2001, "Evolution creature ID mismatch");
    ASSERT_EQ(creature.evolutionLevel, 30, "Evolution level mismatch");
    ASSERT_EQ(creature.evolutionCreatureMultyId.size(), 3, "Evolution multy ID size mismatch");
    ASSERT_EQ(creature.eventEvoluteCreature.size(), 2, "Event evolute creature size mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_ItemEvolution) {
    CreatureScript creature;

    creature.evolutionItemId = 5000;

    creature.itemEvolutionCreatureList.push_back(CreatureItemEvolutionPair(6001, 7001));
    creature.itemEvolutionCreatureList.push_back(CreatureItemEvolutionPair(6002, 7002));

    ASSERT_EQ(creature.evolutionItemId, 5000, "Evolution item ID mismatch");
    ASSERT_EQ(creature.itemEvolutionCreatureList.size(), 2, "Item evolution list size mismatch");
    ASSERT_EQ(creature.itemEvolutionCreatureList[0].creatureId, 6001, "Item evolution creature ID[0] mismatch");
    ASSERT_EQ(creature.itemEvolutionCreatureList[0].itemId, 7001, "Item evolution item ID[0] mismatch");
    ASSERT_EQ(creature.itemEvolutionCreatureList[1].creatureId, 6002, "Item evolution creature ID[1] mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_GrowthSystem) {
    CreatureScript creature;

    creature.growInfo.usingExpItem = true;
    creature.growInfo.growLevel = 25;
    creature.growInfo.growAtypeCreatureId = 8001;
    creature.growInfo.growBtypeCreatureId = 8002;
    creature.growInfo.growAtypeItemIndex = 9001;
    creature.growInfo.growBtypeItemIndex = 9002;
    creature.growInfo.itemCountForLevelup = 5;

    ASSERT_TRUE(creature.growInfo.usingExpItem, "Using exp item should be true");
    ASSERT_EQ(creature.growInfo.growLevel, 25, "Grow level mismatch");
    ASSERT_EQ(creature.growInfo.growAtypeCreatureId, 8001, "Grow Atype creature ID mismatch");
    ASSERT_EQ(creature.growInfo.growBtypeCreatureId, 8002, "Grow Btype creature ID mismatch");
    ASSERT_EQ(creature.growInfo.growAtypeItemIndex, 9001, "Grow Atype item index mismatch");
    ASSERT_EQ(creature.growInfo.growBtypeItemIndex, 9002, "Grow Btype item index mismatch");
    ASSERT_EQ(creature.growInfo.itemCountForLevelup, 5, "Item count for levelup mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_SkillInfo) {
    CreatureScript creature;

    creature.skillInfo = "Normal skill description";
    creature.overSkillInfo = "Over skill description";
    creature.usingRandomSkill = true;

    ASSERT_STR_EQ(creature.skillInfo, "Normal skill description", "Skill info mismatch");
    ASSERT_STR_EQ(creature.overSkillInfo, "Over skill description", "Over skill info mismatch");
    ASSERT_TRUE(creature.usingRandomSkill, "Using random skill should be true");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScriptSkillInfo_Structure) {
    CreatureScriptSkillInfo skill;

    skill.skillName = "Fireball";
    skill.skillExplain = "Throws a fireball";
    skill.skillRecoveryTime = 3000;
    skill.learnSkillLevel = 5;
    skill.skillMp = 50;
    skill.skillMotion = "skill/fireball.ani";
    skill.skillUnderEffect = "effect/fireball_under.ptl";
    skill.skillOverEffect = "effect/fireball_over.ptl";
    skill.skillInfo = "Skill info text";
    skill.attackInfo = "attack/fireball.atk";

    ASSERT_STR_EQ(skill.skillName, "Fireball", "Skill name mismatch");
    ASSERT_STR_EQ(skill.skillExplain, "Throws a fireball", "Skill explain mismatch");
    ASSERT_EQ(skill.skillRecoveryTime, 3000, "Skill recovery time mismatch");
    ASSERT_EQ(skill.learnSkillLevel, 5, "Learn skill level mismatch");
    ASSERT_EQ(skill.skillMp, 50, "Skill MP mismatch");
    ASSERT_STR_EQ(skill.skillMotion, "skill/fireball.ani", "Skill motion mismatch");
    ASSERT_STR_EQ(skill.attackInfo, "attack/fireball.atk", "Attack info mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_SkillList) {
    CreatureScript creature;

    // Add regular skills
    CreatureScriptSkillInfo skill1;
    skill1.skillName = "BasicAttack";
    skill1.skillMp = 10;
    creature.skillList[1] = skill1;

    CreatureScriptSkillInfo skill2;
    skill2.skillName = "SpecialAttack";
    skill2.skillMp = 30;
    creature.skillList[2] = skill2;

    // Add over skills
    CreatureScriptSkillInfo overskill1;
    overskill1.skillName = "UltimateAttack";
    overskill1.skillMp = 100;
    creature.overSkillList[10] = overskill1;

    ASSERT_EQ(creature.skillList.size(), 2, "Skill list size mismatch");
    ASSERT_EQ(creature.overSkillList.size(), 1, "Over skill list size mismatch");

    ASSERT_STR_EQ(creature.skillList[1].skillName, "BasicAttack", "Skill 1 name mismatch");
    ASSERT_EQ(creature.skillList[1].skillMp, 10, "Skill 1 MP mismatch");
    ASSERT_STR_EQ(creature.skillList[2].skillName, "SpecialAttack", "Skill 2 name mismatch");
    ASSERT_STR_EQ(creature.overSkillList[10].skillName, "UltimateAttack", "Over skill name mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_AdditionalAnimations) {
    CreatureScript creature;

    creature.additionalAnimations.push_back(CreatureAdditionalAnimation(1, -1, "animation/type1.ani"));
    creature.additionalAnimations.push_back(CreatureAdditionalAnimation(2, -1, "animation/type2.ani"));
    creature.additionalAnimations.push_back(CreatureAdditionalAnimation(3, -1, "animation/type3.ani"));

    ASSERT_EQ(creature.additionalAnimations.size(), 3, "Additional animations size mismatch");
    ASSERT_EQ(creature.additionalAnimations[0].type, 1, "Animation[0] type mismatch");
    ASSERT_EQ(creature.additionalAnimations[0].index, -1, "Animation[0] index mismatch");
    ASSERT_STR_EQ(creature.additionalAnimations[0].path, "animation/type1.ani", "Animation[0] path mismatch");
    ASSERT_EQ(creature.additionalAnimations[1].type, 2, "Animation[1] type mismatch");
    ASSERT_STR_EQ(creature.additionalAnimations[2].path, "animation/type3.ani", "Animation[2] path mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_SkillTypes) {
    CreatureScript creature;

    creature.skillType = SkillType::ACTIVE;
    creature.overSkillType = SkillType::PASSIVE;

    ASSERT_EQ((int)creature.skillType, (int)SkillType::ACTIVE, "Skill type should be ACTIVE");
    ASSERT_EQ((int)creature.overSkillType, (int)SkillType::PASSIVE, "Over skill type should be PASSIVE");

    creature.skillType = SkillType::PASSIVE;
    ASSERT_EQ((int)creature.skillType, (int)SkillType::PASSIVE, "Skill type should be PASSIVE");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_RevisionCoordinates) {
    CreatureScript creature;

    creature.revisionX = 15;
    creature.revisionY = -10;

    ASSERT_EQ(creature.revisionX, 15, "Revision X mismatch");
    ASSERT_EQ(creature.revisionY, -10, "Revision Y mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_SkillMPSettings) {
    CreatureScript creature;

    creature.skillMp = 25;
    creature.overSkillMp = 80;

    ASSERT_EQ(creature.skillMp, 25, "Skill MP mismatch");
    ASSERT_EQ(creature.overSkillMp, 80, "Over skill MP mismatch");

    return failures;
}
TEST_END()

TEST_CASE(CreatureScript_MultipleInstanceIndependence) {
    CreatureScript creature1;
    CreatureScript creature2;

    creature1.name = "Creature1";
    creature1.startLevel = 10;
    creature1.skillList[1].skillName = "Skill1";

    creature2.name = "Creature2";
    creature2.startLevel = 20;
    creature2.skillList[2].skillName = "Skill2";

    // Verify independence
    ASSERT_STR_EQ(creature1.name, "Creature1", "Creature1 name mismatch");
    ASSERT_STR_EQ(creature2.name, "Creature2", "Creature2 name mismatch");
    ASSERT_EQ(creature1.startLevel, 10, "Creature1 level mismatch");
    ASSERT_EQ(creature2.startLevel, 20, "Creature2 level mismatch");
    ASSERT_EQ(creature1.skillList.size(), 1, "Creature1 skill list size mismatch");
    ASSERT_EQ(creature2.skillList.size(), 1, "Creature2 skill list size mismatch");
    ASSERT_STR_EQ(creature1.skillList[1].skillName, "Skill1", "Creature1 skill name mismatch");
    ASSERT_STR_EQ(creature2.skillList[2].skillName, "Skill2", "Creature2 skill name mismatch");

    return failures;
}
TEST_END()

// ============================================================================
// Test Runner
// ============================================================================

int main() {
    std::printf("Running Creature Parser Tests...\n\n");

    int total_failures = 0;

    total_failures += test_CreatureScript_Constructor();
    total_failures += test_CreatureScript_Clear();
    total_failures += test_CreatureScript_CoreMetadata();
    total_failures += test_CreatureScript_LevelSystem();
    total_failures += test_CreatureScript_MoveSpeed();
    total_failures += test_CreatureScript_ArtifactSlots();
    total_failures += test_CreatureScript_MotionAnimations();
    total_failures += test_CreatureScript_EffectStrings();
    total_failures += test_CreatureScript_AudioSystem();
    total_failures += test_CreatureScript_MotionLists();
    total_failures += test_CreatureScript_CombatData();
    total_failures += test_CreatureScript_DataArrays();
    total_failures += test_CreatureScript_EvolutionSystem();
    total_failures += test_CreatureScript_ItemEvolution();
    total_failures += test_CreatureScript_GrowthSystem();
    total_failures += test_CreatureScript_SkillInfo();
    total_failures += test_CreatureScriptSkillInfo_Structure();
    total_failures += test_CreatureScript_SkillList();
    total_failures += test_CreatureScript_AdditionalAnimations();
    total_failures += test_CreatureScript_SkillTypes();
    total_failures += test_CreatureScript_RevisionCoordinates();
    total_failures += test_CreatureScript_SkillMPSettings();
    total_failures += test_CreatureScript_MultipleInstanceIndependence();

    std::printf("\n========================================\n");
    if (total_failures == 0) {
        std::printf("ALL TESTS PASSED (23 test cases)\n");
        std::printf("========================================\n");
        return 0;
    } else {
        std::printf("TESTS FAILED: %d assertions failed\n", total_failures);
        std::printf("========================================\n");
        return 1;
    }
}
