#ifndef DB_QUEST_H
#define DB_QUEST_H

#include <stdint.h>

typedef struct {
    uint32_t quest_id;
    uint8_t  state;
    uint16_t progress;
    uint16_t goal;
} DBQuestProgress;

typedef struct {
    uint16_t quest_id;
    uint32_t progress;
    uint8_t  completed;
    uint8_t  claimed;
} DailyQuestProgress;

#ifdef __cplusplus
extern "C" {
#endif

int db_save_quest_progress(uint32_t charac_no, uint32_t quest_id, uint8_t state, uint16_t progress);
int db_load_quest_progress(uint32_t charac_no, DBQuestProgress* quests, int max_count);
int db_load_daily_quests(uint32_t charac_no, DailyQuestProgress* quests, int max_count);
int db_save_daily_quest(uint32_t charac_no, uint16_t quest_id, uint32_t progress,
                        uint8_t completed, uint8_t claimed);
int db_load_weekly_quests(uint32_t charac_no, DailyQuestProgress* quests, int max_count);
int db_save_weekly_quest(uint32_t charac_no, uint16_t quest_id, uint32_t progress,
                         uint8_t completed, uint8_t claimed);

#ifdef __cplusplus
}
#endif

#endif /* DB_QUEST_H */
